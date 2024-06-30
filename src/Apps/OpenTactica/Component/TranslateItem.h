#pragma once

#include <Libs/Ecs/EntityUtilities.h>
#include <Libs/Ecs/Component/TransformComponent.h>
#include <Libs/Utility/Reflection.h>

#include <glm/glm.hpp>

namespace tactics::component {
struct TranslateItem {
	float speed;
	glm::vec3 axis;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(TranslateItem, speed, axis);
	static void defineReflection();
};

class TranslateItemSystem {
public:
	static void update(const ecs_view<Transform, TranslateItem>& view);
};
}
