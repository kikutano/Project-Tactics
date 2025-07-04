#pragma once

#include "Rectangle2DCollider.h"
#include "TranslateItem.h"

#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityUtilities.h>
#include <Libs/Utility/Reflection.h>

#include <glm/glm.hpp>

namespace tactics::component {
struct BallMovement {
	float speed;
	glm::vec3 axis;

	COMPONENT(BallMovement, speed, axis)
};

enum class WallType {
	Left,
	Right,
};

struct CollideWithWall {
	WallType wallType;

	COMPONENT(CollideWithWall, wallType)
};

class BallMovementSystem {
public:
	static void update(entt::registry& registry,
					   const ecs_view<Transform, TranslateItem, BallMovement>& view,
					   const ecs_view<Transform, Rectangle2DCollider>& viewCollider);
	static void _updateCollisionWithWall(entt::registry& registry,
										 entt::entity entity,
										 Transform& transform,
										 TranslateItem& translateItem);
	static void _updateCollisionWithPlayer(Transform& ballTransform,
										   TranslateItem& ballTranslate,
										   Transform& playerTransform,
										   Rectangle2DCollider& playerCollider);
};
} // namespace tactics::component
