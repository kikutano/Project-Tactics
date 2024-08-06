#include "PlayerMovement.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {

void PlayerMovementSystem::update(entt::registry& registry) {
	_updateInputs(registry);
}

void PlayerMovementSystem::_updateInputs(entt::registry& registry) {
	auto view = registry.view<PlayerMovement, PlayerMovementInput>();
	for (auto [entity, playerMovement, input] : view.each()) {
		if (input.movePlayer->isTriggered()) {
			auto& value = input.movePlayer->getInputValue();
			if (value.scalar > 0) {
				playerMovement.moveUp();
			} else {
				playerMovement.moveDown();
			}
		}
	}
}

void PlayerMovement::moveUp() {
	Log::trace(Log::Game, "up!");
}

void PlayerMovement::moveDown() {
	Log::trace(Log::Game, "down!");
}

// void PlayerMovementSystem::onKeyPress(SDL_KeyboardEvent& event,
//									  const ecs_view<Transform, TranslateItem, PlayerMovement>& view) {
//	view.each([event](auto& transform, auto& translateItem, auto& playerMovement) {
//		if (event.keysym.sym == playerMovement.keyCodeDown) {
//			if (transform.getPosition().y > -2.1f) {
//				translateItem.axis = Vector3::down;
//			} else {
//				translateItem.axis = Vector3::zero;
//			}
//		} else if (event.keysym.sym == playerMovement.keyCodeUp) {
//			if (transform.getPosition().y < 2.1f) {
//				translateItem.axis = Vector3::up;
//			} else {
//				translateItem.axis = Vector3::zero;
//			}
//		}
//	});
// }
//
// void PlayerMovementSystem::onKeyRelease(SDL_KeyboardEvent& event,
//										const ecs_view<Transform, TranslateItem, PlayerMovement>& view) {
//	view.each([event](auto& /*transform*/, auto& translateItem, auto& playerMovement) {
//		if (event.keysym.sym == playerMovement.keyCodeUp || event.keysym.sym == playerMovement.keyCodeDown) {
//			translateItem.axis = Vector3::zero;
//		}
//	});
// }

} // namespace tactics::component
