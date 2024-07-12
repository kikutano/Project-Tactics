#include "PongDemoState.h"

#include "../Component/TranslateItem.h"

#include <Engine/Scene/SceneSystem.h>

#include <Libs/Ecs/Component/CameraComponent.h>
#include <Libs/Ecs/Component/MeshComponent.h>
#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityComponentSystem.h>
#include <Libs/Rendering/GeometryBuilder.h>
#include <Libs/Rendering/RenderSystem.h>
#include <Libs/Resource/ResourceSystem.h>
#include <Libs/Utility/Math.h>

#include <glm/glm.hpp>

namespace tactics {

FsmAction PongDemoState::enter() {
	auto& sceneSystem = getService<SceneSystem>();

	_playerLeft = sceneSystem.createEntity("playerLeft"_id, "player"_id);
	_playerRight = sceneSystem.createEntity("playerRight"_id, "player"_id);
	_ball = sceneSystem.createEntity("ball"_id, "ball"_id);

	auto& playerLeftTransform = _playerLeft.getComponent<component::Transform>();
	auto& playerRightTransform = _playerRight.getComponent<component::Transform>();
	auto& ballTransform = _ball.getComponent<component::Transform>();

	playerLeftTransform.setPosition(glm::vec3(-4.0f, 0.0f, 0.0f));
	playerRightTransform.setPosition(glm::vec3(4.0f, 0.0f, 0.0f));
	ballTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;
	_playerLeft.getComponent<component::TranslateItem>().speed = 4.0f;
	_playerRight.getComponent<component::TranslateItem>().axis = Vector3::zero;
	_playerRight.getComponent<component::TranslateItem>().speed = 4.0f;

	// auto& resourceSystem = getService<resource::ResourceSystem>();

	/*auto leftStickCollUpRightXPos = _playerLeft.getComponent<component::Transform>().getPosition().x + 0.1f;
	auto leftStickCollUpRightYPos = _playerLeft.getComponent<component::Transform>().getPosition().y + 0.5f;

	auto crate = sceneSystem.createEntity({leftStickCollUpRightXPos, leftStickCollUpRightYPos, 0.0f}, "cube"_id,
	{"texturedUnlit"_id}); crate.getComponent<component::Transform>().setScale({0.1, 0.1, 0.1});
	crate.updateComponent<component::Mesh>([&resourceSystem](auto& mesh) {
		mesh.materials[0]->set("u_Texture", resourceSystem.getResource<resource::Texture>("crate"_id));
	});*/

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
	_updateBallCollisionWithWall();
	_updateBallCollisionWithPlayer();

	return FsmAction::none();
}

void PongDemoState::_updateBallCollisionWithWall() {
	if (_ball.getComponent<component::Transform>().getPosition().y > 2.5f) {
		_ball.getComponent<component::TranslateItem>().axis.y *= -1.0f;
	} else if (_ball.getComponent<component::Transform>().getPosition().y < -2.5f) {
		_ball.getComponent<component::TranslateItem>().axis.y *= -1.0f;
	}

	if (_ball.getComponent<component::Transform>().getPosition().x > 5.0f) {
		_ball.getComponent<component::TranslateItem>().axis.x *= -1.0f;
		_ball.getComponent<component::Transform>().setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	} else if (_ball.getComponent<component::Transform>().getPosition().x < -5.0f) {
		_ball.getComponent<component::TranslateItem>().axis.x *= -1.0f;
		_ball.getComponent<component::Transform>().setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	}
}

void PongDemoState::_updateBallCollisionWithPlayer() {
	auto ballYPos = _ball.getComponent<component::Transform>().getPosition().y;
	auto ballXPos = _ball.getComponent<component::Transform>().getPosition().x;
	auto ballDir = _ball.getComponent<component::TranslateItem>().axis;

	// left player bouncing
	auto leftStickCollUpRightXPos = _playerLeft.getComponent<component::Transform>().getPosition().x + 0.2f;
	auto leftStickCollUpRightYPos = _playerLeft.getComponent<component::Transform>().getPosition().y + 0.5f;
	auto leftStickCollDownRightYPos = _playerLeft.getComponent<component::Transform>().getPosition().y - 0.5f;

	if (ballDir.x < 0) {
		if (ballYPos < leftStickCollUpRightYPos && ballYPos > leftStickCollDownRightYPos) {
			if (ballXPos < leftStickCollUpRightXPos + 0.01f && ballXPos > leftStickCollUpRightXPos - 0.01f) {
				_ball.getComponent<component::TranslateItem>().axis.x = 1.0f;
			}
		}
	}

	// right player bouncing
	auto rightStickCollUpRightXPos = _playerRight.getComponent<component::Transform>().getPosition().x - 0.2f;
	auto rightStickCollUpRightYPos = _playerRight.getComponent<component::Transform>().getPosition().y + 0.5f;
	auto rightStickCollDownRightYPos = _playerRight.getComponent<component::Transform>().getPosition().y - 0.5f;

	if (ballDir.x > 0) {
		if (ballYPos < rightStickCollUpRightYPos && ballYPos > rightStickCollDownRightYPos) {
			if (ballXPos > rightStickCollUpRightXPos - 0.01f && ballXPos < rightStickCollUpRightXPos + 0.01f) {
				_ball.getComponent<component::TranslateItem>().axis.x = -1.0f;
			}
		}
	}
}

FsmEventAction PongDemoState::onKeyPress(SDL_KeyboardEvent& event) {
	if (event.keysym.sym == SDLK_ESCAPE) { return FsmEventAction::transition("exit"_id); }

	if (event.keysym.sym == SDLK_s) {
		if (_playerLeft.getComponent<component::Transform>().getPosition().y > -2.1f) {
			_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::down;
		} else {
			_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;
		}
	} else if (event.keysym.sym == SDLK_w) {
		if (_playerLeft.getComponent<component::Transform>().getPosition().y < 2.1f) {
			_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::up;
		} else {
			_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;
		}
	}

	if (event.keysym.sym == SDLK_l) {
		if (_playerRight.getComponent<component::Transform>().getPosition().y > -2.1f) {
			_playerRight.getComponent<component::TranslateItem>().axis = Vector3::down;
		} else {
			_playerRight.getComponent<component::TranslateItem>().axis = Vector3::zero;
		}
	} else if (event.keysym.sym == SDLK_o) {
		if (_playerRight.getComponent<component::Transform>().getPosition().y < 2.1f) {
			_playerRight.getComponent<component::TranslateItem>().axis = Vector3::up;
		} else {
			_playerRight.getComponent<component::TranslateItem>().axis = Vector3::zero;
		}
	}

	return FsmEventAction::none();
}

FsmEventAction PongDemoState::onKeyRelease(SDL_KeyboardEvent& event) {
	if (event.keysym.sym == SDLK_s || event.keysym.sym == SDLK_w) {
		_playerLeft.getComponent<component::TranslateItem>().axis = Vector3::zero;
	}

	if (event.keysym.sym == SDLK_l || event.keysym.sym == SDLK_o) {
		_playerRight.getComponent<component::TranslateItem>().axis = Vector3::zero;
	}

	return FsmEventAction::none();
}

} // namespace tactics
