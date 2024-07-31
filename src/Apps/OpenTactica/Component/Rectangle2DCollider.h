#pragma once

#include "Rectangle2DCollider.h"
#include "TranslateItem.h"

#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Ecs/EntityUtilities.h>
#include <Libs/Utility/Reflection.h>

#include <glm/glm.hpp>

namespace tactics::component {

struct Rectangle2DCollider {
	float heightFromCenter;
	float widthFromCenter;
	glm::vec3 center = Vector3::zero;

public:
	NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Rectangle2DCollider, center, heightFromCenter, widthFromCenter);
	static void defineReflection();
	void updateCenter(const glm::vec3& newCenter);
	const bool intersect(const glm::vec3& position);
};

class Rectangle2DColliderSystem {
public:
	//const float getUpperLimitPosition() const { return getUpperLeftCornerPosition(); }

	static void update(const ecs_view<Transform, Rectangle2DCollider>& view);
};
} // namespace tactics::component
