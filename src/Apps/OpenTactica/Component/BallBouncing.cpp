#include "BallBouncing.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {

void BallBouncing::defineReflection() {
	componentReflection<BallBouncing>("ballBouncing")
		.data<&BallBouncing::speed>("speed"_id)
		.data<&BallBouncing::axis>("axis"_id);
}

void BallBouncingSystem::update(const ecs_view<Transform, BallBouncing>& view) {
	view.each([](auto& transform, auto& translateItem) {
		transform.translate(translateItem.axis * translateItem.speed * EngineTime::fixedDeltaTime<float>());
	});
}

} // namespace tactics::component
