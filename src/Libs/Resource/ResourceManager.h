#pragma once

#include "Resource.h"
#include "ResourceLoader.h"

#include <Libs/Utility/Exception.h>
#include <Libs/Utility/Log/Log.h>

#include <memory>
#include <unordered_map>

namespace tactics::resource {

template<typename TResource> using ResourceMap = std::unordered_map<ResourceId, std::shared_ptr<TResource>>;

/**
 * Base class for all the manager of a single Resource type. The user generally doesn't touch this
 */
class BaseResourceManager {
public:
	virtual ~BaseResourceManager() = default;

	virtual ResourceType getType() const = 0;
	virtual void unload(ResourceId resourceId) = 0;
	virtual std::shared_ptr<BaseResource> load(HashId name, const json& data) = 0;
	virtual std::shared_ptr<BaseResource> getResource(HashId name) = 0;
	virtual std::shared_ptr<BaseResource> getResource(ResourceId id) = 0;
	virtual std::shared_ptr<BaseResource> getResource(HashId name) const = 0;
	virtual std::shared_ptr<BaseResource> getResource(ResourceId id) const = 0;
	virtual void forEachResource(const std::function<void(const BaseResource&)>& callback) const = 0;
	virtual void forEachResource(const std::function<void(BaseResource&)>& callback) = 0;
	virtual void registerResource(std::shared_ptr<BaseResource> resource) = 0;
	virtual unsigned int getResourceCount() const = 0;

	template<typename TResource> void forEachTResource(const std::function<void(const TResource&)>& callback) const {
		forEachResource(
			[&callback](const BaseResource& resource) { callback(static_cast<const TResource&>(resource)); });
	}

	template<typename TResource> void forEachTResource(const std::function<void(TResource&)>& callback) {
		forEachResource([&callback](BaseResource& resource) { callback(static_cast<TResource&>(resource)); });
	}
};

template<typename T>
concept IsResourceLoader = std::is_base_of_v<ResourceLoader, T>;

template<typename T, typename Ret, typename... Args>
concept has_method = requires(T t, Args... args) {
	{ t.load(args...) } -> std::same_as<Ret>;
};

/**
 * A manager class for a single resource type. This is what is mostly used to define a manager for a resource ( like
 * textures, shaders, etc.. ) Some examples: ResourceManager<Texture, TextureLoader>, ResourceManager<Shader,
 * ShaderLoader>, and so on
 */
template<typename TResource, IsResourceLoader TResourceLoader> class ResourceManager : public BaseResourceManager {
public:
	ResourceManager(std::unique_ptr<TResourceLoader> loader) : _loader(std::move(loader)) {}

	std::shared_ptr<BaseResource> getResource(HashId name) override final {
		return _getTResource(name);
	}

	std::shared_ptr<BaseResource> getResource(ResourceId id) override final {
		return _getTResource(id);
	}

	std::shared_ptr<BaseResource> getResource(HashId name) const final {
		return _getTResource(name);
	}

	std::shared_ptr<BaseResource> getResource(ResourceId id) const final {
		return _getTResource(id);
	}

	ResourceType getType() const override final {
		return TResource::TYPE;
	}

	std::shared_ptr<BaseResource> load(HashId name, const json& data) override final {
		std::shared_ptr<TResource> resource;
		LOG_TRACE(Log::Resource, "Loading [{}]", name);
		if constexpr (has_method<TResourceLoader, std::shared_ptr<TResource>, const json&>) {
			resource = _loader->load(data);
			resource->name = name;
		} else {
			resource = _loader->load(name, data);
		}
		if (!resource) {
			TACTICS_EXCEPTION("Failed to load resource from descriptor. Resource Type: {} - Descriptor: {}",
							  toString(TResource::TYPE),
							  data.dump());
		}
		_registerResource(resource);
		return resource;
	}

	void unload(ResourceId resourceId) override final {
		auto itr = _resources.find(resourceId);
		if (itr == _resources.end()) {
			TACTICS_EXCEPTION(
				"Attempt to remove a resource which is not registered in the Resource Manager. Resource Id: {}",
				resourceId);
		}

		auto useCount = itr->second.use_count();
		if (useCount > 1) {
			TACTICS_EXCEPTION(
				"Attempt to remove a resource which is still in use. Resource Id: {} - Name: {} - Type: {} - RefCount: "
				"{} but should be 1",
				itr->second->id,
				itr->second->name,
				toString(itr->second->type),
				useCount);
		}

		_resources.erase(itr);
	}

	void forEachResource(const std::function<void(const BaseResource&)>& callback) const override final {
		for (auto& [id, resource] : _resources) {
			callback(*resource);
		}
	}

	void forEachResource(const std::function<void(BaseResource&)>& callback) override final {
		for (auto& [id, resource] : _resources) {
			callback(*resource);
		}
	}

	void registerResource(std::shared_ptr<BaseResource> resource) override final {
		if (resource->type != getType()) {
			TACTICS_EXCEPTION(
				"Attempt to register a resource of the wrong type. Resource Type: {} - Expected Type: {} - Name: {} - "
				"ID: {}",
				resource->type,
				TResource::TYPE,
				resource->name,
				resource->id);
		}
		_registerResource(std::dynamic_pointer_cast<TResource>(resource));
	}

	unsigned int getResourceCount() const override final {
		return static_cast<unsigned int>(_resources.size());
	}

private:
	std::shared_ptr<TResource> _getTResource(ResourceId id) {
		return std::const_pointer_cast<TResource>(static_cast<const ResourceManager*>(this)->_getTResource(id));
	}

	std::shared_ptr<TResource> _getTResource(HashId name) {
		return std::const_pointer_cast<TResource>(static_cast<const ResourceManager*>(this)->_getTResource(name));
	}

	const std::shared_ptr<TResource> _getTResource(ResourceId id) const {
		if (!_resources.contains(id)) {
			TACTICS_EXCEPTION("Resource with id \"{}\" does not exist. Can't find resource.", id);
		}

		return _resources.at(id);
	}

	const std::shared_ptr<TResource> _getTResource(HashId name) const {
		auto itr = std::ranges::find_if(_resources, [name](const auto& pair) { return pair.second->name == name; });

		if (itr == _resources.end()) {
			TACTICS_EXCEPTION("Resource with name [{}] does not exist. Can't find resource in [{}] manager.",
							  name,
							  toString(getType()));
		}

		return itr->second;
	}

	void _registerResource(std::shared_ptr<TResource> resource) {
		if (_resources.contains(resource->id)) {
			TACTICS_EXCEPTION("Attempt to register a resource with the same id. Resource Id: {} - Name: {} - Type: {}",
							  resource->id,
							  resource->name,
							  toString(resource->type));
		}

		auto itr = std::ranges::find_if(_resources,
										[resource](const auto& pair) { return pair.second->name == resource->name; });
		if (itr != _resources.end()) {
			TACTICS_EXCEPTION(
				"Attempt to register a resource with the same name. Resource Id: {} - Name: {} - Type: {}",
				resource->id,
				resource->name,
				toString(resource->type));
		}

		_resources.insert({resource->id, std::move(resource)});
	}

	ResourceMap<TResource> _resources;
	std::unique_ptr<TResourceLoader> _loader;
};

} // namespace tactics::resource
