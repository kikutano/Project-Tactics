#include "ResourcePackManager.h"

#include "../ResourceManager.h"

#include <Libs/Utility/Exception.h>

#include <fstream>

namespace tactics::resource {

ResourcePackManager::ResourcePackManager(FileSystem& fileSystem, const ResourceProvider& resourceProvider)
	: _fileSystem(fileSystem)
	, _resourceProvider(resourceProvider) {
}

void ResourcePackManager::loadPackDefinition(const std::filesystem::path& packDefinitionPath) {
	auto fileHandle = _loadPackDefinition(packDefinitionPath);

	for (auto&& [packName, packData] : fileHandle->getContent().items()) {
		auto& pack = createPack(packName, false);

		for (auto&& [resourceType, value] : packData.items()) {
			auto& packGroup = pack.getOrCreatePackGroup(fromString<ResourceType>(resourceType));
			for (auto&& [resourceName, data] : value.items()) {
				packGroup.addResource(resourceName, data);
			}
		}
	}
}

void ResourcePackManager::loadPack(std::string_view packName) {
	auto& pack = _getResourcePack(packName);
	pack.load(_resourceProvider);
}

void ResourcePackManager::unloadPack(std::string_view packName) {
	auto& pack = _getResourcePack(packName);
	pack.unload(_resourceProvider);
}

Pack& ResourcePackManager::createPack(std::string_view packName, bool isManuallyCreated) {
	if (_packs.contains(packName)) {
		throw TACTICS_EXCEPTION("Can't create custom pack [{}]. A pack with the same name already exists.", packName);
	}

	auto pack = std::make_unique<Pack>(packName, isManuallyCreated);
	return *_packs.insert({packName, std::move(pack)}).first->second;
}

void ResourcePackManager::unloadAllPacks() {
	for (auto&& [packName, pack] : _packs) {
		pack->unload(_resourceProvider);
	}
}

Pack& ResourcePackManager::_getResourcePack(std::string_view packName) {
	if (!_packs.contains(packName)) {
		throw TACTICS_EXCEPTION("Can't find resource pack with name [{}]. The Resource Pack does not exists", packName);
	}

	return *_packs[packName];
}

void ResourcePackManager::loadExternalResource(std::string_view packName, std::shared_ptr<BaseResource> resource) {
	auto& pack = _getResourcePack(packName);
	pack.loadExternalResource(_resourceProvider, resource);
}

void ResourcePackManager::loadExternalResource(std::string_view packName, std::string_view resourceName, ResourceType type, const nlohmann::json& data) {
	auto& pack = _getResourcePack(packName);
	pack.loadExternalResource(_resourceProvider, resourceName, type, data);
}

void ResourcePackManager::forEachPack(const std::function<void(const Pack&)>& callback) {
	for (auto&& [packName, pack] : _packs) {
		callback(*pack);
	}
}

std::unique_ptr<FileHandle<nlohmann::ordered_json>> ResourcePackManager::_loadPackDefinition(const std::filesystem::path& packDefinitionPath) {
	auto jsonFileHandle = _fileSystem.createJsonFileHandle(packDefinitionPath);
	jsonFileHandle->load();

	if (jsonFileHandle->getContent().empty()) {
		throw TACTICS_EXCEPTION("Pack definition file [{}] is empty", packDefinitionPath.string());
	}
	return jsonFileHandle;
}

}
