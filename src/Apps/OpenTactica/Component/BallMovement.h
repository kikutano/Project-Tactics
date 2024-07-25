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
	static void update(const ecs_view<Transform, TranslateItem, BallMovement>& view);
	static void _updateBallCollisionWithWall(Transform& transform, TranslateItem& translateItem);
	static void _updateBallCollisionWithPlayer();
};
} // namespace tactics::component
