#include "PongDemoState.h"

#include "../Component/TranslateItem.h"

#include <Engine/Scene/SceneSystem.h>

#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityComponentSystem.h>

namespace tactics {

FsmAction PongDemoState::enter() {
	auto& sceneSystem = getService<SceneSystem>();

	_playerLeft = sceneSystem.createEntity("playerLeft"_id, "player"_id);
	_playerRight = sceneSystem.createEntity("playerRight"_id, "player"_id);
	_ball = sceneSystem.createEntity("ball"_id, "ball"_id);

	auto& playerLeftTransform = _playerLeft.getComponent<component::Transform>();
	auto& playerRightTransform = _playerRight.getComponent<component::Transform>();
	auto& ballTransform = _ball.getComponent<component::Transform>();

	playerLeftTransform.setPosition(glm::vec3(-4.5f, 0.0f, 0.0f));
	playerRightTransform.setPosition(glm::vec3(4.5f, 0.0f, 0.0f));
	ballTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;
	_playerLeft.getComponent<component::TranslateItem>().speed = 2.0f;

	return FsmAction::none();
}

void PongDemoState::exit() {
	auto& sceneSystem = getService<SceneSystem>();
	sceneSystem.clearScene();
}

FsmAction PongDemoState::update() {
	auto& ecs = getService<EntityComponentSystem>();
	component::TranslateItemSystem::update(ecs.sceneRegistry().view<component::Transform, component::TranslateItem>());

	return FsmAction::none();
}

FsmEventAction PongDemoState::onKeyPress(SDL_KeyboardEvent& event) {
	if (event.keysym.sym == SDLK_ESCAPE) {
		return FsmEventAction::transition("exit"_id);
	} else if (event.keysym.sym == SDLK_DOWN) {
		_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::down;
	} else if (event.keysym.sym == SDLK_UP) {
		_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::up;
	} else {
		_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;
	}

	return FsmEventAction::none();
}

FsmEventAction PongDemoState::onKeyRelease(SDL_KeyboardEvent&) {
	_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;

	return FsmEventAction::none();
}

} // namespace tactics
