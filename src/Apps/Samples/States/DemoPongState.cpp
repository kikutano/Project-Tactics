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

FsmAction DemoPongState::enter() {
	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.createEntity("Camera"_id, "defaultCamera"_id);

	auto& inputSystem = getService<InputSystem>();
	inputSystem.assignInputMap("pongMapping");

	_playerLeft = sceneSystem.createEntity("playerLeft"_id, "player1"_id);
	_playerRight = sceneSystem.createEntity("playerRight"_id, "player2"_id);
	_ball = sceneSystem.createEntity("ball"_id, "ball"_id);
	_ball = sceneSystem.createEntity("ball1"_id, "ball"_id);
	
	_ball.getComponent<component::TranslateItem>().axis.y = 0.5f;

	Entity scorebarLeftBg = sceneSystem.createEntity("scorebarLeftBg"_id, "scorebarBg"_id);
	Entity scorebarRightBg = sceneSystem.createEntity("scorebarLeftBg"_id, "scorebarBg"_id);
	Entity scoreLeft = sceneSystem.createEntity("scoreLeft"_id, "scoreNumbers"_id);
	Entity background = sceneSystem.createEntity("background"_id, "background"_id);

	auto& playerLeftTransform = _playerLeft.getComponent<component::Transform>();
	auto& playerRightTransform = _playerRight.getComponent<component::Transform>();

	scorebarLeftBg.getComponent<component::Transform>().setPosition(glm::vec3(-3.4f, 2.65f, 0.0f));
	scorebarRightBg.getComponent<component::Transform>().setPosition(glm::vec3(3.4f, 2.65f, 0.0f));

	playerLeftTransform.setPosition(glm::vec3(-4.0f, 0.0f, 0.0f));
	playerRightTransform.setPosition(glm::vec3(4.0f, 0.0f, 0.0f));

	scoreLeft.getComponent<component::Sprite>().spriteIndex = 2;

	return FsmAction::none();
}

void DemoPongState::exit() {
	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.clearScene();
}

FsmAction DemoPongState::update() {
	using namespace tactics::component;

	auto& ecs = getService<EntityComponentSystem>();
	auto& inputSystem = getService<InputSystem>();
	auto& registry = getService<EntityComponentSystem>().sceneRegistry();
	RotateItemSystem::update(registry);
	if (inputSystem.checkAction("exitFromState")) {
		return FsmAction::transition("exit"_id);
	}

	PlayerMovementSystem::update(ecs.sceneRegistry());

	TranslateItemSystem::update(ecs.sceneRegistry().view<Transform, TranslateItem>());
	Rectangle2DColliderSystem::update(ecs.sceneRegistry().view<Transform, Rectangle2DCollider>());

	BallMovementSystem::update(ecs.sceneRegistry(),
							   ecs.sceneRegistry().view<Transform, TranslateItem, BallMovement>(),
							   ecs.sceneRegistry().view<Transform, Rectangle2DCollider>());

	ecs.sceneRegistry().view<CollideWithWall, Transform>().each(
		[&](entt::entity entity, CollideWithWall& collideWithWall, Transform& transform) {
			auto& sceneSystem = getService<SceneSystem>();
			auto scoreLeft = sceneSystem.getEntityByName("scoreLeft"_id);

			if (collideWithWall.wallType == WallType::Left) {
				scoreLeft.getComponent<component::Sprite>().spriteIndex = 3;
			} else if (collideWithWall.wallType == WallType::Right) {
				scoreLeft.getComponent<component::Sprite>().spriteIndex = 4;
			}
			auto& playerLeftTransform = _playerLeft.getComponent<Transform>();
			playerLeftTransform.setPosition(glm::vec3(-4.0f, 0.0f, 0.0f));
			auto& playerRightTransform = _playerRight.getComponent<Transform>();
			playerRightTransform.setPosition(glm::vec3(4.0f, 0.0f, 0.0f));
			transform.setPosition(Vector3::zero);

			registry.remove<CollideWithWall>(entity);
		});

	return FsmAction::none();
}

} // namespace tactics
