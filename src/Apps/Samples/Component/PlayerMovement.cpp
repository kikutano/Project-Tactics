#include "PlayerMovement.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {

void PlayerMovementSystem::update(entt::registry& registry) {
	_updateInputs(registry);
}

void PlayerMovementSystem::_updateInputs(entt::registry& registry) {
	float const verticalLimit = 2.5f;

	auto view = registry.view<TranslateItem, Transform, PlayerMovement, PlayerMovementInput>();
	for (auto [entity, translateItem, transform, playerMovement, input] : view.each()) {
		auto& value = input.movePlayer->getInputValue();

		if (value.scalar > 0 && transform.getPosition().y > verticalLimit) {
			translateItem.axis = Vector3::zero;
		} else if (value.scalar < 0 && transform.getPosition().y < -verticalLimit) {
			translateItem.axis = Vector3::zero;
		} else {
			translateItem.axis = Vector3::up * value.scalar;
		}
	}
}

} // namespace tactics::component
