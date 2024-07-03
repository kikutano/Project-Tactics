#include "PongDemoState.h"

#include "../Component/TranslateItem.h"

#include <Engine/Scene/SceneSystem.h>

#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityComponentSystem.h>
#include <Libs/Utility/Log/Log.h>

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

	// wall bouncing
	if (_ball.getComponent<component::Transform>().getPosition().y > 2.5f) {
		_ball.getComponent<component::TranslateItem>().axis.y *= -1.0f;
	} else if (_ball.getComponent<component::Transform>().getPosition().y < -2.5f) {
		_ball.getComponent<component::TranslateItem>().axis.y *= -1.0f;
	}

	if (_ball.getComponent<component::Transform>().getPosition().x > 5.0f) {
		_ball.getComponent<component::TranslateItem>().axis.x *= -1.0f;
	} else if (_ball.getComponent<component::Transform>().getPosition().x < -5.0f) {
		_ball.getComponent<component::TranslateItem>().axis.x *= -1.0f;
	}

	// player bouncing
	auto ballYPos = _ball.getComponent<component::Transform>().getPosition().y;
	auto ballXPos = _ball.getComponent<component::Transform>().getPosition().x;

	auto leftStickCollUpRightXPos = _playerLeft.getComponent<component::Transform>().getPosition().x + 0.1f;
	auto leftStickCollUpRightYPos = _playerLeft.getComponent<component::Transform>().getPosition().y + 0.3f;

	auto leftStickCollUpLeftXPos = _ball.getComponent<component::Transform>().getPosition().x - 1.0f;
	// auto leftStickCollUpLeftYPos = _ball.getComponent<component::Transform>().getPosition().y + 1.0f;

	// auto leftStickCollDownRightXPos = _ball.getComponent<component::Transform>().getPosition().x + 1.0f;
	auto leftStickCollDownRightYPos = _playerLeft.getComponent<component::Transform>().getPosition().y - 0.3f;

	// auto leftStickCollDownLeftXPos = _ball.getComponent<component::Transform>().getPosition().x - 1.0f;
	// auto leftStickCollDownLeftYPos = _ball.getComponent<component::Transform>().getPosition().y - 1.0f;

	if (ballYPos < leftStickCollUpRightYPos && ballYPos > leftStickCollDownRightYPos) {
		auto ballXDir = _ball.getComponent<component::TranslateItem>().axis.x;

		if (ballXDir < 0) {
			if (ballXPos < leftStickCollUpRightXPos) { _ball.getComponent<component::TranslateItem>().axis.x *= -1.0f; }
		} else {
			if (ballXPos < leftStickCollUpRightXPos && ballXPos > leftStickCollUpLeftXPos) {
				_ball.getComponent<component::TranslateItem>().axis.x *= -1.0f;
			}
		}
	}

	return FsmAction::none();
}

FsmEventAction PongDemoState::onKeyPress(SDL_KeyboardEvent& event) {
	if (event.keysym.sym == SDLK_ESCAPE) {
		return FsmEventAction::transition("exit"_id);
	} else if (event.keysym.sym == SDLK_DOWN) {
		if (_playerLeft.getComponent<component::Transform>().getPosition().y > -2.1f) {
			_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::down;
		} else {
			_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;
		}
	} else if (event.keysym.sym == SDLK_UP) {
		if (_playerLeft.getComponent<component::Transform>().getPosition().y < 2.1f) {
			_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::up;
		} else {
			_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;
		}
	}

	return FsmEventAction::none();
}

FsmEventAction PongDemoState::onKeyRelease(SDL_KeyboardEvent&) {
	_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;

	return FsmEventAction::none();
}

} // namespace tactics
