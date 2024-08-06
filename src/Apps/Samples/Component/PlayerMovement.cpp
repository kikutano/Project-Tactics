#include "PlayerMovement.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {

void PlayerMovementSystem::update(entt::registry& registry) {
	_updateInputs(registry);
}

void PlayerMovementSystem::_updateInputs(entt::registry& registry) {
	auto view = registry.view<TranslateItem, PlayerMovement, PlayerMovementInput>();
	for (auto [entity, translateItem, playerMovement, input] : view.each()) {
		if (input.movePlayer->isTriggered()) {
			auto& value = input.movePlayer->getInputValue();
			if (value.scalar > 0) {
				playerMovement.moveUp(translateItem);
			} else if (value.scalar < 0) {
				playerMovement.moveDown(translateItem);
			} else {
				translateItem.axis = Vector3::zero;
			}
		}
	}
}

void PlayerMovement::moveUp(TranslateItem& translateItem) {
	translateItem.axis = Vector3::up;
}

void PlayerMovement::moveDown(TranslateItem& translateItem) {
	translateItem.axis = Vector3::down;
}

} // namespace tactics::component
