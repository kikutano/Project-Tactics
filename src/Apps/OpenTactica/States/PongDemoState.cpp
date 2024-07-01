#include "PongDemoState.h"

#include "../Component/TranslateItem.h"

#include <Engine/Scene/SceneSystem.h>
#include <Libs/Ecs/Component/TransformComponent.h>

#include <Engine/Scene/SceneSystem.h>

#include <Libs/Ecs/EntityComponentSystem.h>
#include <Libs/Ecs/Component/TransformComponent.h>

namespace tactics {

FsmAction PongDemoState::enter() {
	auto& sceneSystem = getService<SceneSystem>();

	_stickLeft = sceneSystem.createEntity("stickLeft"_id, "player"_id);
	_stickRight = sceneSystem.createEntity("stickRight"_id, "player"_id);
	_ball = sceneSystem.createEntity("ball"_id, "ball"_id);

	auto& stickLeftTransform = _stickLeft.getComponent<component::Transform>();
	auto& stickRightTransform = _stickRight.getComponent<component::Transform>();
	auto& ballTransform = _ball.getComponent<component::Transform>();

	stickLeftTransform.setPosition(glm::vec3(-4.5f, 0.0f, 0.0f));
	stickRightTransform.setPosition(glm::vec3(4.5f, 0.0f, 0.0f));
	ballTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	_stickLeft.getComponent<component::TranslateItem>().axis = Vector3::up;
	_stickLeft.getComponent<component::TranslateItem>().speed = 5.0f;

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
		_stickLeft.getComponent<component::TranslateItem>().axis = Vector3::down;
	} else if (event.keysym.sym == SDLK_UP) {
		_stickLeft.getComponent<component::TranslateItem>().axis = Vector3::up;
	}

	return FsmEventAction::none();
}

}
