#include "Entity.h"

#include "Component/NameComponent.h"

namespace tactics {

Entity::Entity(const HashId& name, entt::registry* registry) : _registry(registry) {
	_entity = registry->create();
	_registry->emplace<component::Name>(_entity, name);
}

Entity::Entity() {
	_entity = entt::null;
}

void Entity::destroy() {
	if (_entity != entt::null && _registry->valid(_entity)) {
		_registry->destroy(_entity);
	}
}

const HashId& Entity::getName() const {
	// Why we're not calling getComponent<component::Name>() directly?
	// If the entity has no component::Name we end up in a stack overflow otherwise.
	// This is because getComponent<component::Name>() calls getName() which calls getComponent<component::Name>() and
	// so on.
	if (!hasComponent<component::Name>()) {
		static auto noNameComponent = "[No Name Component]"_id;
		return noNameComponent;
	}

	return getComponent<component::Name>().name;
}

Entity Entity::create(const HashId& name, entt::registry* registry) {
	return Entity(name, registry);
}

Entity Entity::create(entt::entity entity, entt::registry* registry) {
	Entity entityObject;
	entityObject._entity = entity;
	entityObject._registry = registry;
	return entityObject;
}

bool Entity::operator==(entt::entity entity) const {
	return _entity == entity;
}

Entity::operator bool() const {
	return _entity != entt::null;
}

} // namespace tactics
