#include "ComponentReflection.h"

#include "CameraComponent.h"
#include "FrustumComponent.h"
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "BillboardComponent.h"
#include "../EntityUtilities.h"

#include <Libs/Utility/Json/MathJsonSerializer.h>
#include <Libs/Utility/Reflection.h>

#include <entt/entt.hpp>

namespace tactics {

void serializeFromAngleAxis(glm::quat& quat, const nlohmann::ordered_json& jsonData) {
	glm::vec3 axis = jsonData.at("axis");
	float angle = jsonData.at("angle");
	quat = glm::angleAxis(glm::radians(angle), axis);
}

void serializeFromLookAt(glm::quat& quat, const nlohmann::ordered_json& jsonData) {
	glm::vec3 from = jsonData.at("from");
	glm::vec3 to = jsonData.at("to");
	quat = glm::quatLookAt(glm::normalize(to - from), Vector3::up);
}

void ComponentReflection::initializeBasicTypes() {
	entt::meta<glm::vec2>()
		.type(hash("vec2"))
		.data<&glm::vec2::x>(hash("x"))
		.data<&glm::vec2::y>(hash("y"));

	entt::meta<glm::vec3>()
		.type(hash("vec3"))
		.data<&glm::vec3::x>(hash("x"))
		.data<&glm::vec3::y>(hash("y"))
		.data<&glm::vec3::z>(hash("z"));

	entt::meta<glm::quat>()
		.type(hash("quat"))
		.func<&serializeFromAngleAxis>(hash("fromAngleAxis"))
		.func<&serializeFromLookAt>(hash("fromLookAt"))
		.data<&glm::quat::x>(hash("x"))
		.data<&glm::quat::y>(hash("y"))
		.data<&glm::quat::z>(hash("z"))
		.data<&glm::quat::w>(hash("w"));
}

void ComponentReflection::initializeCommonComponents() {
	using namespace component;
	defineComponentsReflection<
		Billboard,
		Camera,
		CurrentCamera,
		Frustum,
		Mesh,
		Sprite,
		SpriteAnimation,
		Transform>();
}

}
