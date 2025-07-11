#include "SceneSystem.h"

#include <Libs/Ecs/Component/AlphaBlendedComponent.h>
#include <Libs/Ecs/Component/CameraComponent.h>
#include <Libs/Ecs/Component/DebugDrawingComponent.h>
#include <Libs/Ecs/Component/FrustumComponent.h>
#include <Libs/Ecs/Component/MeshComponent.h>
#include <Libs/Ecs/Component/NameComponent.h>
#include <Libs/Ecs/Component/ParticleEmitterComponent.h>
#include <Libs/Ecs/Component/RenderableComponent.h>
#include <Libs/Ecs/Component/SpriteComponent.h>
#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityComponentSystem.h>
#include <Libs/Ecs/System/CameraSystem.h>
#include <Libs/Resource/Material/Material.h>
#include <Libs/Resource/Mesh/Mesh.h>
#include <Libs/Resource/Prefab/Prefab.h>
#include <Libs/Resource/ResourceSystem.h>

#include <ranges>
#include <regex>

namespace tactics {

SceneSystem::SceneSystem(EntityComponentSystem& ecs, resource::ResourceSystem& resourceSystem)
	: _ecs(ecs)
	, _resourceSystem(resourceSystem) {
	using namespace component;
	auto& registry = _ecs.sceneRegistry();

	registry.on_construct<Mesh>().connect<&SceneSystem::_onMeshConstructed>(this);
	registry.on_destroy<Mesh>().connect<&SceneSystem::_onMeshDestroyed>(this);
	registry.on_construct<Renderable>().connect<&SceneSystem::_onRenderableConstructed>(this);
	registry.on_update<Renderable>().connect<&SceneSystem::_onRenderableUpdated>(this);
	registry.on_construct<CurrentCamera>().connect<&SceneSystem::_onCurrentCameraConstructed>(this);
	registry.on_construct<SpriteAnimation>().connect<&SceneSystem::_onSpriteAnimationConstructed>(this);
	registry.on_update<SpriteAnimation>().connect<&SceneSystem::_onSpriteAnimationUpdated>(this);
}

SceneSystem::~SceneSystem() {}

entt::registry& SceneSystem::getRegistry() {
	return _ecs.sceneRegistry();
}

const entt::registry& SceneSystem::getRegistry() const {
	return _ecs.sceneRegistry();
}

void SceneSystem::clearScene(bool clearCameras) {
	auto& registry = _ecs.sceneRegistry();
	auto meshView = registry.view<component::Mesh>();
	registry.destroy(meshView.begin(), meshView.end());

	auto spriteView = registry.view<component::Sprite>();
	registry.destroy(spriteView.begin(), spriteView.end());

	auto particleView = registry.view<component::ParticleEmitter>();
	registry.destroy(particleView.begin(), particleView.end());

	if (clearCameras) {
		auto cameraView = registry.view<component::Camera>();
		registry.destroy(cameraView.begin(), cameraView.end());
	}
}

Entity SceneSystem::getCurrentCamera() {
	auto view = _ecs.sceneRegistry().view<component::CurrentCamera>();
	if (view.empty()) {
		TACTICS_EXCEPTION("No current camera entity found");
	}
	return Entity::create(*view.begin(), &_ecs.sceneRegistry());
}

void SceneSystem::_onCurrentCameraConstructed(entt::registry&, entt::entity currentCameraEntity) {
	using namespace component;
	_ecs.sceneRegistry().view<CurrentCamera>().each([this, currentCameraEntity](auto entity) {
		if (currentCameraEntity != entity) {
			_ecs.sceneRegistry().remove<CurrentCamera>(entity);
		}
	});
}

void SceneSystem::_onMeshConstructed(entt::registry& registry, entt::entity entity) {
	registry.emplace<component::Renderable>(entity, component::RenderType::Mesh);
}

void SceneSystem::_onMeshDestroyed(entt::registry& registry, entt::entity entity) {
	registry.erase<component::Renderable>(entity);
}

void SceneSystem::_onRenderableConstructed(entt::registry& registry, entt::entity entity) {
	_updateAlphaBlendFlags(registry, entity);
}

void SceneSystem::_onRenderableUpdated(entt::registry& registry, entt::entity entity) {
	_updateAlphaBlendFlags(registry, entity);
}

void SceneSystem::_onSpriteAnimationUpdated(entt::registry& registry, entt::entity entity) {
	using namespace component;
	auto entityWrapper = Entity::create(entity, &registry);
	auto& spriteAnimation = entityWrapper.getComponent<SpriteAnimation>();
	if (entityWrapper.hasComponent<Sprite>()) {
		auto& sprite = entityWrapper.getComponent<Sprite>();
		if (!spriteAnimation.currentAnimation.isEmpty() &&
			!sprite.spriteSheet->animations.contains(spriteAnimation.currentAnimation)) {
			TACTICS_EXCEPTION("Animation [{}] not found in sprite sheet [{}]",
							  spriteAnimation.currentAnimation,
							  sprite.spriteSheet->name);
		}
	}
}

void SceneSystem::_onSpriteAnimationConstructed(entt::registry& registry, entt::entity entity) {
	_onSpriteAnimationUpdated(registry, entity);
}

void SceneSystem::_updateAlphaBlendFlags(entt::registry& registry, entt::entity entity) {
	using namespace component;

	auto isFullyTransparent = true;
	auto isMixedAlphaBlended = false;

	auto& renderable = registry.get<Renderable>(entity);
	switch (renderable.type) {
	case RenderType::Mesh: {
		auto& mesh = registry.get<Mesh>(entity);

		for (auto& material : mesh.materials) {
			isMixedAlphaBlended |= material->parent->hasAlphaBlend;
			isFullyTransparent &= material->parent->hasAlphaBlend;
		}
		break;
	}
	case RenderType::Particle: {
		// TODO(Gerark) For now we assume that all particles are alpha blended
		isMixedAlphaBlended = true;
		isFullyTransparent = true;
		break;
	}
	}

	if (isFullyTransparent) {
		registry.emplace_or_replace<FullyAlphaBlended>(entity);
		registry.emplace_or_replace<AlphaBlended>(entity);
	} else if (isMixedAlphaBlended) {
		registry.remove<FullyAlphaBlended>(entity);
		registry.emplace_or_replace<AlphaBlended>(entity);
	} else {
		registry.remove<FullyAlphaBlended>(entity);
		registry.remove<AlphaBlended>(entity);
	}
}

Entity SceneSystem::createCamera(const HashId& name,
								 const glm::vec3& position,
								 const glm::vec3& direction,
								 const glm::vec3& up,
								 float fov,
								 float near,
								 float far) {
	using namespace component;
	auto entity = Entity::create(name, &_ecs.sceneRegistry());
	auto& transform = entity.addComponent<Transform>();
	transform.setPosition(position);
	transform.setRotation(glm::quatLookAt(direction, up));

	entity.addComponent<Frustum>(fov, near, far, 1.f);
	entity.addComponent<Camera>();
	return entity;
}

Entity SceneSystem::createEntity(const HashId& name,
								 const glm::vec3& position,
								 const HashId& meshName,
								 const std::vector<HashId>& materials,
								 const glm::quat& rotation,
								 const glm::vec3& scale) {
	using namespace component;
	auto entity = Entity::create(name, &_ecs.sceneRegistry());
	auto& transform = entity.addComponent<Transform>();
	transform.setPosition(position);
	transform.setRotation(rotation);
	transform.setScale(scale);

	Mesh meshComp;
	meshComp.mesh = _resourceSystem.getResource<resource::Mesh>(meshName);
	for (auto i = 0u; i < meshComp.mesh->subMeshes.size(); ++i) {
		auto& materialName = i < materials.size() ? materials[i] : materials.back();
		auto material = _resourceSystem.getResource<resource::Material>(materialName);
		meshComp.materials.push_back(resource::Material::createInstance(material));
	}
	entity.addComponent<Mesh>(meshComp);
	return entity;
}

Entity SceneSystem::createEntity(const HashId& name, const HashId& prefabName) {
	auto prefab = _resourceSystem.getResource<resource::Prefab>(prefabName);
	auto entity = _ecs.createEntityFromPrefab(name, prefab->entity);
	return entity;
}

void SceneSystem::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float lifetime) {
	using namespace component;
	auto entity = Entity::create("DebugLine"_id, &_ecs.sceneRegistry());
	entity.addComponent<DebugLine>(start, end, color);
	entity.addComponent<DebugDrawingLifetime>(lifetime);
}

void SceneSystem::drawBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float lifetime) {
	using namespace component;
	auto entity = Entity::create("DebugBox"_id, &_ecs.sceneRegistry());
	entity.addComponent<DebugBox>(position, size, color);
	entity.addComponent<DebugDrawingLifetime>(lifetime);
}

void SceneSystem::drawSphere(const glm::vec3& position, float radius, const glm::vec4& color, float lifetime) {
	using namespace component;
	auto entity = Entity::create("DebugSphere"_id, &_ecs.sceneRegistry());
	entity.addComponent<DebugSphere>(position, radius, color);
	entity.addComponent<DebugDrawingLifetime>(lifetime);
}

Entity SceneSystem::getEntityByName(const HashId& name) {
	auto view = _ecs.sceneRegistry().view<component::Name>();
	for (auto [entity, nameComp] : view.each()) {
		if (nameComp.name == name) {
			return Entity::create(entity, &_ecs.sceneRegistry());
		}
	}

	TACTICS_EXCEPTION("Entity with name {} not found", name.str());
	return Entity();
}

} // namespace tactics
