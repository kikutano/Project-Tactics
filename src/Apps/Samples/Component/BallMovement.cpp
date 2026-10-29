#include "BallMovement.h"

#include "Rectangle2DCollider.h"
#include "RotateItem.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {

void BallMovementSystem::update(entt::registry& registry,
								const ecs_view<Transform, TranslateItem, BallMovement, RotateItem>& view,
								const ecs_view<Transform, Rectangle2DCollider>& viewCollider) {
	view.each([&registry, &viewCollider](auto entity,
										 auto& ballTransform,
										 auto& translateItem,
										 auto& /* ballMovement*/,
										 auto& rotateItem) {
		_updateCollisionWithWall(registry, entity, ballTransform, translateItem);

		viewCollider.each([&ballTransform, &translateItem, &rotateItem](auto& playerTransform, auto& collider) {
			_updateCollisionWithPlayer(ballTransform, translateItem, playerTransform, collider, rotateItem);
		});
	});
}

void BallMovementSystem::_updateCollisionWithWall(entt::registry& registry,
												  entt::entity entity,
												  Transform& transform,
												  TranslateItem& translateItem) {
	if (transform.getPosition().y > 2.5f) {
		translateItem.axis.y *= -1.0f;
	} else if (transform.getPosition().y < -2.5f) {
		translateItem.axis.y *= -1.0f;
	}

	if (transform.getPosition().x > 5.0f) {
		translateItem.axis.x *= -1.0f;
		registry.emplace_or_replace<CollideWithWall>(entity, WallType::Right);
	} else if (transform.getPosition().x < -5.0f) {
		translateItem.axis.x *= -1.0f;
		registry.emplace_or_replace<CollideWithWall>(entity, WallType::Left);
	}
}

void BallMovementSystem::_updateCollisionWithPlayer(Transform& ballTransform,
													TranslateItem& ballTranslate,
													Transform& /*playerTransform*/,
													Rectangle2DCollider& playerCollider,
													RotateItem& rotateItem) {
	auto nextPos =
		ballTransform.getPosition() + ballTranslate.axis * ballTranslate.speed * EngineTime::fixedDeltaTime<float>();
	if (playerCollider.intersect(nextPos)) {
		ballTranslate.axis.x *= -1;

		if (ballTranslate.speed < 4.5f) {
			ballTranslate.speed *= 1.1f;
			rotateItem.speed *= 1.1f;
		}

		auto pos = ballTransform.getPosition();

		// Adjust the position of the ball to avoid getting stuck in the player collider
		if (ballTranslate.axis.x < 0) {
			pos.x -= 0.15f;
		} else {
			pos.x += 0.15f;
		}

		ballTransform.setPosition(pos);
	}
}

} // namespace tactics::component
