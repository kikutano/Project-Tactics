#include "DemoSimpleState.h"

#include "../Component/FreeCamera.h"
#include "../Component/RotateAroundPoint.h"
#include "../Component/RotateItem.h"

#include <Engine/Scene/SceneSystem.h>

#include <Libs/Ecs/Component/CameraComponent.h>
#include <Libs/Ecs/Component/MeshComponent.h>
#include <Libs/Ecs/Component/ParticleEmitterComponent.h>
#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityComponentSystem.h>
#include <Libs/Input/InputSystem.h>
#include <Libs/Rendering/GeometryBuilder.h>
#include <Libs/Rendering/Particle/ParticleSystem.h>
#include <Libs/Rendering/RenderSystem.h>
#include <Libs/Resource/Input/InputMap.h>
#include <Libs/Resource/ResourceSystem.h>
#include <Libs/Utility/String/Math.h>
#include <Libs/Utility/Time/EngineTime.h>

#include <array>
#include <glm/glm.hpp>

namespace tactics {

FsmAction DemoSimpleState::enter() {
	_createCamera("freeCamera"_id);
	_createPlane();
	_createTeapot();
	_createCrate();
	_createQuads();
	_createExtraRotatingQuads();
	_createCustomQuadWithCustomResources();
	_createParticleEffect();
	_setupInputMap();

	auto& inputSystem = getService<InputSystem>();
	inputSystem.lockMouseToWindow(true);

	// Draw 4 green lines for 5 seconds
	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.drawLine({-20, 50, -50}, {20, 50, -50}, Color::green, 5);
	sceneSystem.drawLine({-20, 70, -50}, {20, 70, -50}, Color::green, 5);
	sceneSystem.drawLine({-20, 50, -50}, {-20, 70, -50}, Color::green, 5);
	sceneSystem.drawLine({20, 50, -50}, {20, 70, -50}, Color::green, 5);

	return FsmAction::none();
}

void DemoSimpleState::exit() {
	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.clearScene();

	auto& resourceSystem = getService<resource::ResourceSystem>();
	resourceSystem.unloadPack("CustomPack"_id);
	resourceSystem.removePack("CustomPack"_id);

	auto& inputSystem = getService<InputSystem>();
	inputSystem.lockMouseToWindow(false);
}

FsmAction DemoSimpleState::update() {
	auto& registry = getService<EntityComponentSystem>().sceneRegistry();
	component::RotateItemSystem::update(registry);
	component::RotateAroundPointSystem::update(registry);
	component::FreeCameraSystem::update(registry);

	auto& inputSystem = getService<InputSystem>();
	if (inputSystem.checkAction("exitFromState")) {
		return FsmAction::transition("exit"_id);
	}

	// Some examples of how to draw debug shapes
	auto& sceneSystem = getService<SceneSystem>();
	_timer += EngineTime::fixedDeltaTime<float>();
	auto maxLimit = 20.f;
	for (auto i = 0; i < 10; ++i) {
		sceneSystem.drawLine({glm::cos(_timer + i * 2) * maxLimit, 50, -50},
							 {glm::sin(_timer + i * 2) * maxLimit, 70, -50},
							 {(glm::sin(_timer + i * 2) + 1) / 2, (glm::cos(_timer + i * 2) + 1) / 2, 0, 1});
	}
	sceneSystem.drawSphere({-50, 60, -50}, 10, Color::cyan);
	sceneSystem.drawBox({50, 60, -50}, {10, 10, 10}, Color::red);
	return FsmAction::none();
}

void DemoSimpleState::_setupInputMap() {
	auto& inputSystem = getService<InputSystem>();
	auto& resourceSystem = getService<resource::ResourceSystem>();

	auto inputMap = resourceSystem.getResource<resource::InputMap>("defaultInputMap"_id);
	inputSystem.assignInputMap(inputMap, 0);
}

void DemoSimpleState::_createCrate() {
	auto& sceneSystem = getService<SceneSystem>();
	auto& resourceSystem = getService<resource::ResourceSystem>();

	auto crate = sceneSystem.createEntity("crate"_id, {40.0f, 5.0f, 0.0f}, "cube"_id, {"texturedUnlit"_id});
	crate.getComponent<component::Transform>().setScale({10, 10, 10});
	crate.updateComponent<component::Mesh>([&resourceSystem](auto& mesh) {
		mesh.materials[0]->set("u_Texture", resourceSystem.getResource<resource::Texture>("crate"_id));
	});
}

void DemoSimpleState::_createTeapot() {
	auto& sceneSystem = getService<SceneSystem>();

	auto teapot = sceneSystem.createEntity("teapot"_id, {0.0f, 0.0f, 0.0f}, "teapot"_id, {"coloredUnlit"_id});
	auto& transform = teapot.getComponent<component::Transform>();
	transform.setRotation(glm::radians(90.0f), Vector3::up);
	transform.setScale({5, 5, 5});
	teapot.updateComponent<component::Mesh>([](auto& mesh) { mesh.materials[0]->set("u_Color", Color::gray); });
}

void DemoSimpleState::_createPlane() {
	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.createEntity("plane"_id, "plane"_id);
}

void DemoSimpleState::_createQuads() {
	auto& sceneSystem = getService<SceneSystem>();
	auto& resourceSystem = getService<resource::ResourceSystem>();

	const int width = 4;
	const int height = 4;
	for (auto x = -width / 2.f; x < width / 2.f; x += 1.f) {
		for (auto y = -height / 2.f; y < height / 2.f; y += 1.f) {
			auto quad = sceneSystem.createEntity("quadTest"_id,
												 {-50.0f + y * 20.f, 10.0f, x * 10.f},
												 "quad"_id,
												 {"texturedUnlitWithAlpha"_id});
			quad.getComponent<component::Transform>().setScale({15, 15, 15});
			quad.getComponent<component::Mesh>().materials[0]->set(
				"u_Texture",
				resourceSystem.getResource<resource::Texture>("tacticsIcon"_id));
		}
	}
}

void DemoSimpleState::_createExtraRotatingQuads() {
	auto& sceneSystem = getService<SceneSystem>();

	const int width = 4;
	const int height = 4;
	const int distance = 3;
	glm::vec3 offset = {40, 25, 0};
	for (auto x = -width / 2; x < width / 2; ++x) {
		for (auto y = -height / 2; y < height / 2; ++y) {
			auto quad = sceneSystem.createEntity("rotatingQuad"_id, "simpleRotatingQuad"_id);
			auto position = glm::vec3{x * distance, y * distance, 0.0f};
			quad.getComponent<component::Transform>().setPosition(offset + position);
		}
	}

	offset = {52, 25, 0};
	for (auto x = -width / 2; x < width / 2; ++x) {
		for (auto y = -height / 2; y < height / 2; ++y) {
			auto quad = sceneSystem.createEntity("rotatingQuad"_id, "simpleRotatingQuad"_id);
			auto position = glm::vec3{x * distance, y * distance, 0.0f};
			quad.getComponent<component::Transform>().setPosition(offset + position);
			quad.getComponent<component::RotateItem>().axis = Vector3::up;
		}
	}

	offset = {28, 25, 0};
	for (auto x = -width / 2; x < width / 2; ++x) {
		for (auto y = -height / 2; y < height / 2; ++y) {
			auto quad = sceneSystem.createEntity("rotatingQuad"_id, "simpleRotatingQuad"_id);
			auto position = glm::vec3{x * distance, y * distance, 0.0f};
			quad.getComponent<component::Transform>().setPosition(offset + position);
			quad.getComponent<component::RotateItem>().axis = Vector3::forward;
		}
	}
}

void DemoSimpleState::_createCustomQuadWithCustomResources() {
	auto& sceneSystem = getService<SceneSystem>();
	auto& resourceSystem = getService<resource::ResourceSystem>();

	// Example of how to create a custom resource programmatically ( in this case a mesh )
	// with a custom material/shader and add it to the resource system
	resourceSystem.createManualPack("CustomPack"_id);
	auto geometryBuilder = GeometryBuilder({{3}, {2}});
	geometryBuilder.beginSubMesh();
	geometryBuilder.addVertex({-10.f, -10.f, 0.0f, 0, 0});
	geometryBuilder.addVertex({10.f, -10.f, 0.0f, 1, 0});
	geometryBuilder.addVertex({10, 10.0f, 0.0f, 1, 1});
	geometryBuilder.addVertex({-10, 10.0f, 0.0f, 0, 1});
	geometryBuilder.addIndices({0, 1, 2});
	geometryBuilder.addIndices({2, 3, 0});
	geometryBuilder.endSubMesh();
	auto triangleMesh = geometryBuilder.build("customQuadMesh"_id);
	resourceSystem.loadExternalResource("CustomPack"_id, triangleMesh);

	// We can also create a resource by simulating the usual pack loading
	json descriptor = {{"vertexShader", "common/shaders/default.vert"}, {"fragmentShader", R"(
				#version 330 core
				layout(location = 0) out vec4 color;
				uniform vec4 u_Color;
				void main()
				{
					color = u_Color;
				}
			)"}};
	resourceSystem.loadExternalResource<resource::Shader>("CustomPack"_id, "CustomShader"_id, descriptor);

	auto material = std::make_shared<resource::Material>("colorOnly"_id);
	material->shader = resourceSystem.getResource<resource::Shader>("CustomShader"_id);
	material->set("u_Color", {0.204f, 0.608f, 0.922f, 1.0f});
	resourceSystem.loadExternalResource("CustomPack"_id, material);

	// Now I can use the quad mesh by applying the custom material with a specialized fragment shader
	auto customQuad =
		sceneSystem.createEntity("customQuad"_id, {0.0f, 40.0f, 0.0f}, "customQuadMesh"_id, {"colorOnly"_id});
	customQuad.addComponent<component::RotateItem>(5.f, Vector3::forward);
}

void DemoSimpleState::_createParticleEffect() {
	auto& sceneSystem = getService<SceneSystem>();
	auto& particleSystem = getService<ParticleSystem>();
	auto position = glm::vec3{-90.0f, 5.0f, -60.0f};

	// Use colors of the raimbow to define the next array
	const std::array colors = std::to_array<glm::vec4>({
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // Red
		glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), // Orange
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // Yellow
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // Green
		glm::vec4(0.0f, 0.5f, 1.0f, 1.0f), // Cyan
		glm::vec4(0.5f, 0.0f, 1.0f, 1.0f), // Blue
		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)  // Magenta
	});

	auto createParticleEffect = [&sceneSystem,
								 &particleSystem](auto&& name, auto&& prefab, auto&& position, auto&& color) {
		auto particleEffect = sceneSystem.createEntity(name, prefab);
		auto transform = &particleEffect.getComponent<component::Transform>();
		transform->setPosition(position);
		transform->setScale(10);
		auto effectId = particleEffect.getComponent<component::ParticleEmitter>().maybeEffectId;
		auto copiedConfig = particleSystem.getEffectConfig(*effectId);
		auto& colorOverLifetime = std::get<firebolt::ColorOverLifetime>(copiedConfig.updaters[0]);
		// change start color from blue to red based on x coordinate
		colorOverLifetime.startColor = color;
		particleSystem.updateEffectConfig(*effectId, copiedConfig);
	};

	for (const auto& color : colors) {
		position.x += 20;
		createParticleEffect("fire"_id, "fireEffect"_id, position, color);
		createParticleEffect("ember"_id, "emberEffect"_id, position, color);
	}
}

} // namespace tactics
