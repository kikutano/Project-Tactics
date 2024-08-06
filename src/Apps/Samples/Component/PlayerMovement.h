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

	void moveUp(TranslateItem& translateItem);
	void moveDown(TranslateItem& translateItem);

	REFLECT(PlayerMovement, active);
};

struct PlayerMovementInput {
	resource::InputAction::Ptr movePlayer;

	REFLECT(PlayerMovementInput, movePlayer);
};

class PlayerMovementSystem {
public:
	static void update(entt::registry& registry);

private:
	static void _updateInputs(entt::registry& registry);
};
} // namespace tactics::component
