#include "Rectangle2DCollider.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {
void Rectangle2DCollider::defineReflection() {
	componentReflection<Rectangle2DCollider>("Rectangle2DCollider")
		.data<&Rectangle2DCollider::heightFromCenter>("heightFromCenter"_id)
		.data<&Rectangle2DCollider::widthFromCenter>("widthFromCenter"_id)
		.data<&Rectangle2DCollider::center>("center"_id);
}

void Rectangle2DColliderSystem::update(const ecs_view<Transform, Rectangle2DCollider>& view) {
	view.each([](auto& transform) { center = transform.getPosition(); });
}

} // namespace tactics::component
