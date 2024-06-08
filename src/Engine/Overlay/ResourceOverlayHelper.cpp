#include "ResourceOverlayHelper.h"

#include "CustomOverlayColors.h"

#include <Libs/Resource/IniFile/IniFile.h>
#include <Libs/Resource/Mesh/Mesh.h>
#include <Libs/Resource/Material/Material.h>
#include <Libs/Resource/ResourcePack/ResourcePack.h>
#include <Libs/Resource/Shader/Shader.h>
#include <Libs/Resource/Texture/Texture.h>

namespace tactics {

void ResourceOverlayHelper::drawResource(const resource::ResourceInfo& resourceInfo, resource::ResourceType resourceType) {
	using namespace resource;

	ImGui::PushStyleColor(ImGuiCol_TabActive, toColor(resourceType, 0.4f));
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, toColor(resourceType, 0.4f));
	ImGui::BeginTabBar(resourceInfo.getName().c_str());
	ImGui::PopStyleColor(2);

	if (resourceInfo.isLoaded()) {
		if (_beginTabItem("Info", resourceType)) {
			auto& resource = *resourceInfo.getResource();
			switch (resourceType) {
			case ResourceType::Texture: {
				drawTResource<Texture>(resource);
				break;
			}
			case ResourceType::Mesh: {
				drawTResource<Mesh>(resource);
				break;
			}
			case ResourceType::Material: {
				drawTResource<Material>(resource);
				break;
			}
			case ResourceType::Shader: {
				drawTResource<Shader>(resource);
				break;
			}
			case ResourceType::IniFile: {
				drawTResource<IniFile>(resource);
				break;
			}
			}
			ImGui::EndTabItem();
		}
	}

	if (!resourceInfo.getData().is_null()) {
		if (_beginTabItem("Json Data", resourceType)) {
			ImGui::TextWrapped("%s", resourceInfo.getData().dump(4).c_str());
			ImGui::EndTabItem();
		}
	}

	ImGui::EndTabBar();
}

void ResourceOverlayHelper::drawResource(const resource::Texture& texture) {
	using namespace tactics::resource;
	ImGui::Text("Size: %dx%d", texture.info.width, texture.info.height);
	auto width = static_cast<float>(texture.info.width);
	auto height = static_cast<float>(texture.info.height);
	auto ratio = height / width;
	if (ImGui::ImageButton((void*)(intptr_t)texture.rendererId, ImVec2(100, 100 * ratio), ImVec2(0, 1), ImVec2(1, 0), 1)) {
		ImGui::OpenPopup("Texture Viewer");
	}

	// In order to show the X button on the top right corner we have to pass a bool to the BeginPopupModal.
	// It doesn't matter if the bool is always true as the popup is controlled by other means.
	bool popupOpened = true;
	if (ImGui::BeginPopupModal("Texture Viewer", &popupOpened, ImGuiWindowFlags_AlwaysAutoResize)) {
		auto previewWidth = std::min(width, 512.f);
		ImGui::Image((void*)(intptr_t)texture.rendererId, {previewWidth, previewWidth * ratio}, ImVec2(0, 1), ImVec2(1, 0), {1, 1, 1, 1});
		ImGui::EndPopup();
	}
}

void ResourceOverlayHelper::drawResource(const resource::IniFile& iniFile) {
	using namespace resource;
	auto& data = iniFile.fileHandle->getContent();
	for (auto& [sectionName, section] : data) {
		ImGui::TextColored(toColor(ResourceType::IniFile), "[%s]", sectionName.c_str());
		for (auto& [key, value] : section) {
			ImGui::Text("%s = %s", key.c_str(), value.as<std::string>().c_str());
		}
		ImGui::Separator();
	}
}

void ResourceOverlayHelper::drawResource(const resource::Mesh& mesh) {
	using namespace resource;
	ImGui::Text("Vertices: %d", mesh.getTotalVertexCount());
	ImGui::Text("Tris: %d", mesh.getTotalTrisCount());
}

void ResourceOverlayHelper::drawResource(const resource::Material& material) {
	using namespace resource;
	auto color = toColor(ResourceType::Material);
	ImGui::TextColored(color, "Shader:");
	ImGui::SameLine();
	ImGui::Text("%s", material.shader->name.c_str());
	for (auto&& [key, value] : material.getInts()) {
		ImGui::TextColored(color, "%s", key.c_str());
		ImGui::SameLine();
		ImGui::Text("%d", value);
	}
	for (auto&& [key, value] : material.getFloats()) {
		ImGui::TextColored(color, "%s", key.c_str());
		ImGui::SameLine();
		ImGui::Text("%.2f", value);
	}
	for (auto&& [key, value] : material.getVectors2()) {
		ImGui::TextColored(color, "%s", key.c_str());
		ImGui::SameLine();
		ImGui::Text("%.2f %.2f", value.x, value.y);
	}
	for (auto&& [key, value] : material.getVectors3()) {
		ImGui::TextColored(color, "%s", key.c_str());
		ImGui::SameLine();
		ImGui::Text("%.2f %.2f %.2f", value.x, value.y, value.z);
	}
	for (auto&& [key, value] : material.getVectors4()) {
		ImGui::TextColored(color, "%s", key.c_str());
		ImGui::SameLine();
		ImGui::Text("%.2f %.2f %.2f %.2f", value.x, value.y, value.z, value.w);
	}
	for (auto&& [key, value] : material.getTextures()) {
		ImGui::TextColored(color, "%s", key.c_str());
		ImGui::SameLine();
		ImGui::Text("%s", value->name.c_str());
	}
	for (auto&& [key, value] : material.getMatrices()) {
		ImGui::TextColored(color, "%s", key.c_str());
		ImGui::Text("%.2f %.2f %.2f %.2f", value[0][0], value[0][1], value[0][2], value[0][3]);
		ImGui::Text("%.2f %.2f %.2f %.2f", value[1][0], value[1][1], value[1][2], value[1][3]);
		ImGui::Text("%.2f %.2f %.2f %.2f", value[2][0], value[2][1], value[2][2], value[2][3]);
		ImGui::Text("%.2f %.2f %.2f %.2f", value[3][0], value[3][1], value[3][2], value[3][3]);

	}
}

void ResourceOverlayHelper::drawResource(const resource::Shader& shader) {
	using namespace resource;
	ImGui::TextColored(toColor(ResourceType::Shader), "Vertex Shader");
	ImGui::TextWrapped("%s", shader.vertexSource.c_str());

	ImGui::Separator();
	ImGui::TextColored(toColor(ResourceType::Shader), "Fragment Shader");
	ImGui::TextWrapped("%s", shader.fragmentSource.c_str());
}

bool ResourceOverlayHelper::_beginTabItem(const char* name, resource::ResourceType resourceType) {
	ImGui::PushStyleColor(ImGuiCol_Tab, toColor(resourceType, 0.4f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_TabActive, toColor(resourceType, 0.45f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TabHovered, toColor(resourceType, 0.5f, 0.8f));
	bool tabActive = ImGui::BeginTabItem(name);
	ImGui::PopStyleColor(3);
	return tabActive;
}

ImVec4 ResourceOverlayHelper::toColor(resource::ResourceType resourceType, float multiplier, float alphaMultiplier) {
	auto& colors = CustomOverlayColors::getColors();
	ImVec4 result{};
	switch (resourceType) {
	case resource::ResourceType::IniFile:
		result = colors.IniFileColor;
		break;
	case resource::ResourceType::Material:
		result = colors.MaterialColor;
		break;
	case resource::ResourceType::Mesh:
		result = colors.MeshColor;
		break;
	case resource::ResourceType::Prefab:
		result = colors.PrefabColor;
		break;
	case resource::ResourceType::Shader:
		result = colors.ShaderColor;
		break;
	case resource::ResourceType::Texture:
		result = colors.TextureColor;
		break;
	default:
		result = colors.ResourceColor;
		break;
	}
	result.x *= multiplier;
	result.y *= multiplier;
	result.z *= multiplier;
	result.w *= alphaMultiplier;
	return result;
}

}
