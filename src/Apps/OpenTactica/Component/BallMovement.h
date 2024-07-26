#pragma once

#include "TranslateItem.h"

#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityUtilities.h>
#include <Libs/Utility/Reflection.h>

#include <glm/glm.hpp>

namespace tactics::component {
struct BallMovement {
	float speed;
	glm::vec3 axis;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(BallMovement, speed, axis);
	static void defineReflection();
};

class BallMovementSystem {
public:
	static void update(const ecs_view<Transform, TranslateItem, BallMovement>& view,
					   const ecs_view<Transform, Rectangle2DCollider>& viewCollider);
	static void _updateCollisionWithWall(Transform& transform, TranslateItem& translateItem);
	static void _updateCollisionWithPlayer(Transform& ballTransform, Rectangle2DCollider& collider);
};
} // namespace tactics::component
