#include "RenderSystem.h"

#include "DebugMessageHandler.h"
#include "RenderQueue.h"
#include "RenderStep/RenderStep.h"

#include <Libs/Utility/Exception.h>
#include <Libs/Utility/Ini/IniMathConverter.h>
#include <Libs/Utility/Log/Log.h>

#include <SDL.h>
#include <glad/gl.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_sdl2.h>

namespace tactics {

RenderSystem::RenderSystem(std::shared_ptr<resource::IniFile> configFile) : _configFile(configFile) {
	_setupGlAttributes();
	_createWindow();
	_initializeGlContext();
	_initializeImGui();

	auto useDebugMessages = _getConfigValue("enableDebugMessages", false);
	if (useDebugMessages) {
		_debugMessageHandler = std::make_unique<DebugMessageHandler>();
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

RenderSystem::~RenderSystem() {
	_shutdownImGui();
	SDL_GL_DeleteContext(_oglContext);
	LOG_TRACE(Log::Rendering, "OpenGL Context Destroyed");
	SDL_DestroyWindow(_window);
	LOG_TRACE(Log::Rendering, "Window Destroyed");
}

RenderQueue& RenderSystem::createRenderQueue() {
	auto renderQueue = std::make_unique<RenderQueue>();
	_renderQueues.push_back(std::move(renderQueue));
	return *_renderQueues.back().get();
}

void RenderSystem::destroyRenderQueues() {
	_renderQueues.clear();
}

void RenderSystem::_setupGlAttributes() {
	_setupDoubleBuffer();
	_setupOglVersion();
	_setupOglContextProfile();
}

void RenderSystem::_createWindow() {
	auto size = _getConfigValue("windowSize", glm::u32vec2{1280, 720});
	auto position = _getConfigValue("windowPosition", glm::u32vec2{100, 100});
	auto windowTitle = _getConfigValue("windowTitle", std::string("Project-Tactics-Sample"));
	auto fullscreen = _getConfigValue("fullscreen", false);
	auto resize = _getConfigValue("windowResize", false);
	Uint32 flags = SDL_WINDOW_OPENGL;
	flags |= resize ? SDL_WINDOW_RESIZABLE : 0u;
	flags |= fullscreen ? SDL_WINDOW_FULLSCREEN : 0u;

	_window = SDL_CreateWindow(windowTitle.c_str(), position.x, position.y, size.x, size.y, flags);
	LOG_TRACE(Log::Rendering, "Window Created {}x{}", size.x, size.y);
	if (_window == nullptr) {
		TACTICS_EXCEPTION("Failed to open window: %s\n", SDL_GetError());
	}

	_windowSize = {size.x, size.y};
}

void RenderSystem::_initializeGlContext() {
	_oglContext = SDL_GL_CreateContext(_window);
	SDL_GL_MakeCurrent(_window, _oglContext);
	_setupVSync();

	int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	if (version == 0) {
		TACTICS_EXCEPTION("Failed to initialize OpenGL context\n");
	}

	LOG_TRACE(Log::Rendering, "Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
}

void RenderSystem::render() {
	RenderStepInfo renderInfo{_viewport, getWindowSize()};

	for (auto& renderQueue : _renderQueues) {
		renderQueue->execute(renderInfo);
	}
	SDL_GL_SwapWindow(_window);
}

void RenderSystem::_initializeImGui() {
	ImGui_ImplSDL2_InitForOpenGL(_window, _oglContext);
	ImGui_ImplOpenGL3_Init();
	LOG_TRACE(Log::Rendering, "ImGui Backend Initialized");
}

void RenderSystem::_shutdownImGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	LOG_TRACE(Log::Rendering, "ImGui Backend Shutdown");
}

void RenderSystem::_setupOglVersion() {
	auto majorVersion = _getConfigValue("majorVersion", 3);
	auto minorVersion = _getConfigValue("minorVersion", 2);
	auto useDebugMessages = _getConfigValue("enableDebugMessages", false);

	// We're using OpenGL 4.3 to get access to the glDebugMessageCallback and get better error message. We might
	// decrease the version if this is the only functionality we need and we're releasing a final version which doesn't
	// require the log part
	if (useDebugMessages) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		majorVersion = std::max(4, majorVersion);
		minorVersion = std::max(3, minorVersion);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);
}

void RenderSystem::_setupVSync() {
	auto enableVSyncStr = _getConfigValue("enableVSync", std::string("true"));
	auto swapInterval = 0;
	if (enableVSyncStr == "true") {
		swapInterval = 1;
	} else if (enableVSyncStr == "false") {
		swapInterval = 0;
	} else if (enableVSyncStr == "adaptive") {
		swapInterval = -1;
	} else {
		TACTICS_EXCEPTION("Invalid value for enableVSync in the config file. Expected 'true/false/adaptive', got %s",
						  enableVSyncStr.c_str());
	}
	SDL_GL_SetSwapInterval(swapInterval);
}

void RenderSystem::_setupOglContextProfile() {
	auto contextProfile = _getConfigValue("contextProfile", std::string("core"));
	if (contextProfile == "core") {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	} else if (contextProfile == "compatibility") {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	} else if (contextProfile == "es") {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	} else {
		TACTICS_EXCEPTION("Invalid rendering context profile provided: %s, check your ini config file.",
						  contextProfile);
	}
}

void RenderSystem::_setupDoubleBuffer() {
	auto enableDoubleBuffer = _getConfigValue("doubleBuffer", true);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, enableDoubleBuffer ? 1 : 0);
}

const glm::u32vec2& RenderSystem::getWindowSize() const {
	return _windowSize;
}

void RenderSystem::setViewport(const glm::vec2& position, const glm::vec2& size, glm::vec4 clearColor) {
	_viewport.position = position;
	_viewport.size = size;
	_viewport.clearColor = clearColor;
}

const Viewport& RenderSystem::getViewport() const {
	return _viewport;
}

bool RenderSystem::onEvent(const SDL_Event& event) {
	switch (event.type) {
	case SDL_WINDOWEVENT: {
		if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			_windowSize = {event.window.data1, event.window.data2};
		}
		break;
	}
	}
	return false;
}

} // namespace tactics
