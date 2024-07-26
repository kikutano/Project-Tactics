#include "BallMovement.h"

#include "Rectangle2DCollider.h"

#include <Libs/Utility/Time/EngineTime.h>

namespace tactics::component {

void BallMovement::defineReflection() {
	componentReflection<BallMovement>("ballMovement")
		.data<&BallMovement::speed>("speed"_id)
		.data<&BallMovement::axis>("axis"_id);
}

void BallMovementSystem::update(const ecs_view<Transform, TranslateItem, BallMovement>& view,
								const ecs_view<Transform, Rectangle2DCollider>& viewCollider) {
	view.each([](auto& transform, auto& translateItem, auto& /* ballMovement*/) {
		_updateCollisionWithWall(transform, translateItem);
	});

	viewCollider.each([](auto& transform, auto& collider) {
		_updateCollisionWithPlayer(transform, collider);
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

void BallMovementSystem::_updateCollisionWithPlayer(Transform& ballTransform, Rectangle2DCollider& collider) {
	/*auto ballYPos = _ball.getComponent<component::Transform>().getPosition().y;
	auto ballXPos = _ball.getComponent<component::Transform>().getPosition().x;
	auto ballDir = _ball.getComponent<component::TranslateItem>().axis;

	// left player bouncing
	auto leftStickCollUpRightXPos = _playerLeft.getComponent<component::Transform>().getPosition().x + 0.2f;
	auto leftStickCollUpRightYPos = _playerLeft.getComponent<component::Transform>().getPosition().y + 0.5f;
	auto leftStickCollDownRightYPos = _playerLeft.getComponent<component::Transform>().getPosition().y - 0.5f;

	if (ballDir.x < 0) {
		if (ballYPos < leftStickCollUpRightYPos && ballYPos > leftStickCollDownRightYPos) {
			if (ballXPos < leftStickCollUpRightXPos + 0.01f && ballXPos > leftStickCollUpRightXPos - 0.01f) {
				_ball.getComponent<component::TranslateItem>().axis.x = 1.0f;
			}
		}
	}

	// right player bouncing
	auto rightStickCollUpRightXPos = _playerRight.getComponent<component::Transform>().getPosition().x - 0.2f;
	auto rightStickCollUpRightYPos = _playerRight.getComponent<component::Transform>().getPosition().y + 0.5f;
	auto rightStickCollDownRightYPos = _playerRight.getComponent<component::Transform>().getPosition().y - 0.5f;

	if (ballDir.x > 0) {
		if (ballYPos < rightStickCollUpRightYPos && ballYPos > rightStickCollDownRightYPos) {
			if (ballXPos > rightStickCollUpRightXPos - 0.01f && ballXPos < rightStickCollUpRightXPos + 0.01f) {
				_ball.getComponent<component::TranslateItem>().axis.x = -1.0f;
			}
		}
	}*/
}

} // namespace tactics::component
