#pragma once

#include <Libs/Fsm/FsmStateWithServices.h>

namespace tactics {

class LoadState: public FsmStateWithServices {
public:
	using FsmStateWithServices::FsmStateWithServices;
	FsmAction enter() override;
	FsmAction update() override;
	void exit() override;

private:
	void _loadResources();
	void _createViewport();
	void _createCamera();
	void _setupRenderSteps();
};

}
