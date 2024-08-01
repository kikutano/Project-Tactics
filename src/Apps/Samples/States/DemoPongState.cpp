#include "DemoPongState.h"

#include "../Component/BallMovement.h"
#include "../Component/PlayerMovement.h"
#include "../Component/TranslateItem.h"

#include <Engine/Scene/SceneSystem.h>

#include <Libs/Ecs/Component/CameraComponent.h>
#include <Libs/Ecs/Component/MeshComponent.h>
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

	_playerLeft = sceneSystem.createEntity("playerLeft"_id, "player"_id);
	_playerRight = sceneSystem.createEntity("playerRight"_id, "player"_id);
	_ball = sceneSystem.createEntity("ball"_id, "ball"_id);

	auto& playerLeftTransform = _playerLeft.getComponent<component::Transform>();
	auto& playerRightTransform = _playerRight.getComponent<component::Transform>();

	playerLeftTransform.setPosition(glm::vec3(-4.0f, 0.0f, 0.0f));
	playerRightTransform.setPosition(glm::vec3(4.0f, 0.0f, 0.0f));

	return FsmAction::none();
}

void DemoPongState::exit() {
	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.clearScene();
}

FsmAction DemoPongState::update() {
	auto& ecs = getService<EntityComponentSystem>();
	auto& inputSystem = getService<InputSystem>();
	if (inputSystem.checkAction("exitFromState")) {
		return FsmAction::transition("exit"_id);
	} else if (inputSystem.checkAction("playerMovement")) {
		component::PlayerMovementSystem::update(ecs.sceneRegistry());
		//Log::trace(Log::Game, "Player movement action detected");
	}

	component::TranslateItemSystem::update(ecs.sceneRegistry().view<component::Transform, component::TranslateItem>());
	component::Rectangle2DColliderSystem::update(
		ecs.sceneRegistry().view<component::Transform, component::Rectangle2DCollider>());

	component::BallMovementSystem::update(
		ecs.sceneRegistry().view<component::Transform, component::TranslateItem, component::BallMovement>(),
		ecs.sceneRegistry().view<component::Transform, component::Rectangle2DCollider>());

	return FsmAction::none();
}

/*
FsmEventAction PongDemoState::onKeyPress(SDL_KeyboardEvent& event) {
	if (event.keysym.sym == SDLK_ESCAPE) {
		return FsmEventAction::transition("exit"_id);
	}

	auto& ecs = getService<EntityComponentSystem>();
	component::PlayerMovementSystem::onKeyPress(
		event,
		ecs.sceneRegistry().view<component::Transform, component::TranslateItem, component::PlayerMovement>());

	return FsmEventAction::none();
}

FsmEventAction PongDemoState::onKeyRelease(SDL_KeyboardEvent& event) {
	auto& ecs = getService<EntityComponentSystem>();
	component::PlayerMovementSystem::onKeyRelease(
		event,
		ecs.sceneRegistry().view<component::Transform, component::TranslateItem, component::PlayerMovement>());

	return FsmEventAction::none();
}
*/
} // namespace tactics
