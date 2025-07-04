#pragma once

#include "TranslateItem.h"

#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityUtilities.h>
#include <Libs/Resource/Input/InputAction.h>
#include <Libs/Utility/Reflection.h>

#include <SDL.h>
#include <glm/glm.hpp>

namespace tactics::component {
struct PlayerMovement {
	bool active;

	COMPONENT(PlayerMovement, active);
};

struct PlayerMovementInput {
	resource::InputAction::Ptr movePlayer;

	COMPONENT(PlayerMovementInput, movePlayer);
};

class PlayerMovementSystem {
public:
	static void update(entt::registry& registry);

private:
	static void _updateInputs(entt::registry& registry);
};
} // namespace tactics::component
