#include "PlayerMovement.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {

void PlayerMovementSystem::update(entt::registry& registry) {
	_updateInputs(registry);
}

void PlayerMovementSystem::_updateInputs(entt::registry& registry) {
	auto view = registry.view<TranslateItem, PlayerMovement, PlayerMovementInput>();
	for (auto [entity, translateItem, playerMovement, input] : view.each()) {
		auto& value = input.movePlayer->getInputValue();
		translateItem.axis = Vector3::up * value.scalar;
	}
}

} // namespace tactics::component
