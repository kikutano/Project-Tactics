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
	SDL_KeyCode keyCodeUp;
	SDL_KeyCode keyCodeDown;

	void moveDown();
	void moveUp();

	REFLECT(PlayerMovement, active);
};

struct PlayerMovementInput {
	resource::InputAction::Ptr playerMovement;

	REFLECT(PlayerMovementInput, playerMovement);
};

class PlayerMovementSystem {
public:
	// static void onKeyPress(SDL_KeyboardEvent& event, const ecs_view<Transform, TranslateItem, PlayerMovement>& view);
	// static void onKeyRelease(SDL_KeyboardEvent& event, const ecs_view<Transform, TranslateItem, PlayerMovement>&
	// view);
	static void update(entt::registry& registry);

private:
	static void _updateInputs(entt::registry& registry);
};
} // namespace tactics::component
