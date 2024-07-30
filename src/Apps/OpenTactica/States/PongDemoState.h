#pragma once

#include <Libs/Fsm/FsmStateWithServices.h>
#include <Libs/Ecs/Entity.h>

namespace tactics {

class PongDemoState : public FsmStateWithServices {
public:
	using FsmStateWithServices::FsmStateWithServices;
	FsmAction update() override;
	FsmAction enter() override;
	void exit() override;

private:
	FsmEventAction onKeyPress(SDL_KeyboardEvent& event) override;
	FsmEventAction onKeyRelease(SDL_KeyboardEvent& event) override;
	Entity _playerLeft;
	Entity _playerRight;
	Entity _ball;
	Entity _scoreLeft;
};
}
