#include "BallMovement.h"

#include "Rectangle2DCollider.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {

void BallMovementSystem::update(const ecs_view<Transform, TranslateItem, BallMovement>& view,
								const ecs_view<Transform, Rectangle2DCollider>& viewCollider) {
	view.each([&viewCollider](auto& ballTransform, auto& translateItem, auto& /* ballMovement*/) {
		_updateCollisionWithWall(ballTransform, translateItem);

		viewCollider.each([&ballTransform, &translateItem](auto& playerTransform, auto& collider) {
			_updateCollisionWithPlayer(ballTransform, translateItem, playerTransform, collider);
		});
	});
}

void BallMovementSystem::_updateCollisionWithWall(Transform& transform, TranslateItem& translateItem) {
	if (transform.getPosition().y > 2.5f) {
		translateItem.axis.y *= -1.0f;
	} else if (transform.getPosition().y < -2.5f) {
		translateItem.axis.y *= -1.0f;
	}

	if (transform.getPosition().x > 5.0f) {
		translateItem.axis.x *= -1.0f;
		transform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	} else if (transform.getPosition().x < -5.0f) {
		translateItem.axis.x *= -1.0f;
		transform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	}
}

void BallMovementSystem::_updateCollisionWithPlayer(Transform& ballTransform,
													TranslateItem& ballTranslate,
													Transform& /*playerTransform*/,
													Rectangle2DCollider& playerCollider) {
	auto nextPos =
		ballTransform.getPosition() + ballTranslate.axis * ballTranslate.speed * EngineTime::fixedDeltaTime<float>();
	if (playerCollider.intersect(nextPos)) {
		ballTranslate.axis.x *= -1;
	}
}

} // namespace tactics::component
