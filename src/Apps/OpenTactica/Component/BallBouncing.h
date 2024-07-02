#pragma once

#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityUtilities.h>
#include <Libs/Utility/Reflection.h>

#include <glm/glm.hpp>

namespace tactics::component {
struct BallBouncing {
	float speed;
	glm::vec3 axis;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(BallBouncing, speed, axis);
	static void defineReflection();
};

class BallBouncingSystem {
public:
	static void update(const ecs_view<Transform, BallBouncing>& view);
};
} // namespace tactics::component
