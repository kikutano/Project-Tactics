#include "Rectangle2DCollider.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {

void Rectangle2DCollider::defineReflection() {
	componentReflection<Rectangle2DCollider>("rectangle2DCollider");
}

void Rectangle2DColliderSystem::update(const ecs_view<Transform, Rectangle2DCollider>& view) {
	view.each([](auto& transform, auto& translateItem) {
		_updateBallCollisionWithWall(transform, translateItem);
	});
}

} // namespace tactics::component
