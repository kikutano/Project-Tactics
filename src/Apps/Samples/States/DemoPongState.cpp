#include "DemoPongState.h"

#include "../Component/BallMovement.h"
#include "../Component/PlayerMovement.h"
#include "../Component/RotateItem.h"
#include "../Component/TranslateItem.h"

#include <Engine/Scene/SceneSystem.h>

#include <Libs/Ecs/Component/CameraComponent.h>
#include <Libs/Ecs/Component/MeshComponent.h>
#include <Libs/Ecs/Component/SpriteComponent.h>
#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityComponentSystem.h>
#include <Libs/Input/InputSystem.h>
#include <Libs/Rendering/GeometryBuilder.h>
#include <Libs/Rendering/RenderSystem.h>
#include <Libs/Resource/ResourceSystem.h>
#include <Libs/Utility/Math.h>

namespace tactics {
void DemoPongState::resetPlayerPosition() {
	using namespace tactics::component;
	using namespace glm;

	auto& playerLeftTransform = _playerLeft.getComponent<Transform>();
	auto& playerRightTransform = _playerRight.getComponent<Transform>();
	playerLeftTransform.setPosition(vec3(-4.5f, 0.0f, 0.0f));
	playerRightTransform.setPosition(vec3(4.5f, 0.0f, 0.0f));
}

FsmAction DemoPongState::enter() {
	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.createEntity("Camera"_id, "defaultCamera"_id);

	auto& inputSystem = getService<InputSystem>();
	inputSystem.assignInputMap("pongMapping");

	_playerLeft = sceneSystem.createEntity("playerLeft"_id, "player1"_id);
	_playerRight = sceneSystem.createEntity("playerRight"_id, "player2"_id);
	_ball = sceneSystem.createEntity("ball"_id, "ball"_id);

	Entity scorebarLeftBg = sceneSystem.createEntity("scorebarLeftBg"_id, "scorebarBg"_id);
	Entity scorebarRightBg = sceneSystem.createEntity("scorebarLeftBg"_id, "scorebarBg"_id);
	Entity scoreLeft = sceneSystem.createEntity("scoreLeft"_id, "scoreNumbers"_id);
	Entity scoreRight = sceneSystem.createEntity("scoreRight"_id, "scoreNumbers"_id);
	Entity background = sceneSystem.createEntity("background"_id, "background"_id);

	resetPlayerPosition();

	scorebarLeftBg.getComponent<component::Transform>().setPosition(glm::vec3(-3.4f, 2.65f, 0.0f));
	scorebarRightBg.getComponent<component::Transform>().setPosition(glm::vec3(3.4f, 2.65f, 0.0f));

	scoreLeft.getComponent<component::Sprite>().spriteIndex = 0;
	scoreRight.getComponent<component::Sprite>().spriteIndex = 0;

	scoreLeft.getComponent<component::Transform>().setPosition(glm::vec3(-3.4f, 2.65f, 0.01f));
	scoreRight.getComponent<component::Transform>().setPosition(glm::vec3(3.4f, 2.65f, 0.01f));

	return FsmAction::none();
}

void DemoPongState::exit() {
	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.clearScene();
}

FsmAction DemoPongState::update() {
	using namespace tactics::component;

	auto& ecs = getService<EntityComponentSystem>();
	const auto& inputSystem = getService<InputSystem>();
	auto& registry = getService<EntityComponentSystem>().sceneRegistry();
	RotateItemSystem::update(registry);
	if (inputSystem.checkAction("exitFromState")) {
		return FsmAction::transition("exit"_id);
	}

	PlayerMovementSystem::update(ecs.sceneRegistry());

	TranslateItemSystem::update(ecs.sceneRegistry().view<Transform, TranslateItem>());
	Rectangle2DColliderSystem::update(ecs.sceneRegistry().view<Transform, Rectangle2DCollider>());

	BallMovementSystem::update(ecs.sceneRegistry(),
							   ecs.sceneRegistry().view<Transform, TranslateItem, BallMovement, RotateItem>(),
							   ecs.sceneRegistry().view<Transform, Rectangle2DCollider>());

	ecs.sceneRegistry().view<CollideWithWall, Transform, TranslateItem, RotateItem>().each(
		[&](entt::entity entity,
			CollideWithWall& collideWithWall,
			Transform& transform,
			TranslateItem& translate,
			RotateItem& rotateItem) {
			auto& sceneSystem = getService<SceneSystem>();
			auto scoreLeft = sceneSystem.getEntityByName("scoreLeft"_id);
			auto scoreRight = sceneSystem.getEntityByName("scoreRight"_id);

			if (collideWithWall.wallType == WallType::Left) {
				if (scoreLeft.getComponent<component::Sprite>().spriteIndex >= 9) {
					scoreLeft.getComponent<component::Sprite>().spriteIndex = 0;
				}
				scoreLeft.getComponent<component::Sprite>().spriteIndex++;
			} else if (collideWithWall.wallType == WallType::Right) {
				if (scoreRight.getComponent<component::Sprite>().spriteIndex >= 9) {
					scoreRight.getComponent<component::Sprite>().spriteIndex = 0;
				}
				scoreRight.getComponent<component::Sprite>().spriteIndex++;
			}

			transform.setPosition(Vector3::zero);
			translate.speed = 2.5f;
			rotateItem.speed = 4.0f;

			resetPlayerPosition();
			registry.remove<CollideWithWall>(entity);
		});

	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.drawBox({0, 0, 0}, {1, 1, 1}, Color::red);

	return FsmAction::none();
}

} // namespace tactics
