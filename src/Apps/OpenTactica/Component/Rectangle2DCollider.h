#pragma once

#include "TranslateItem.h"

#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityUtilities.h>
#include <Libs/Utility/Reflection.h>

#include <glm/glm.hpp>

namespace tactics::component {
struct Rectangle2DCollider {
	float heightFromCenter;
	float widthFromCenter;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Rectangle2DCollider, heightFromCenter, widthFromCenter);
	static void defineReflection();
};

class Rectangle2DColliderSystem {
public:
	static void update(const ecs_view<Transform, Rectangle2DCollider>& view);
};
} // namespace tactics::component
