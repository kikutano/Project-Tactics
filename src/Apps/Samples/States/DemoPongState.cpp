#include "DemoPongState.h"

#include "../Component/BallMovement.h"
#include "../Component/PlayerMovement.h"
#include "../Component/TranslateItem.h"
#include "../Component/RotateItem.h"

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
	auto& ecs = getService<EntityComponentSystem>();
	auto& inputSystem = getService<InputSystem>();
	auto& registry = getService<EntityComponentSystem>().sceneRegistry();
	component::RotateItemSystem::update(registry);
	if (inputSystem.checkAction("exitFromState")) {
		return FsmAction::transition("exit"_id);
	}

	component::PlayerMovementSystem::update(ecs.sceneRegistry());

	component::TranslateItemSystem::update(ecs.sceneRegistry().view<component::Transform, component::TranslateItem>());
	component::Rectangle2DColliderSystem::update(
		ecs.sceneRegistry().view<component::Transform, component::Rectangle2DCollider>());

	component::BallMovementSystem::update(
		ecs.sceneRegistry().view<component::Transform, component::TranslateItem, component::BallMovement>(),
		ecs.sceneRegistry().view<component::Transform, component::Rectangle2DCollider>());

	return FsmAction::none();
}

} // namespace tactics
