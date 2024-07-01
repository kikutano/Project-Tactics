#include "OpenTacticaApplication.h"

#include "Component/BattleCamera.h"
#include "Component/CharacterFacing.h"
#include "Component/RotateItem.h"
#include "Component/RotateAroundPoint.h"
#include "Component/TranslateItem.h"

#include "States/DemoSimpleState.h"
#include "States/EmptyState.h"
#include "States/LoadState.h"
#include "States/DemoMapState.h"
#include "States/DemoSpriteState.h"
#include "States/UnloadState.h"
#include "States/PongDemoState.h"

#include <Libs/Utility/Reflection.h>
#include <Libs/Resource/ResourceSystem.h>
#include <Libs/Resource/IniFile/IniFile.h>

namespace tactics {

void OpenTacticaApplication::setupComponentReflections() {
	using namespace component;
	defineComponentsReflection<BattleCamera, CharacterFacing, RotateItem, RotateAroundPoint, TranslateItem>();
}

HashId OpenTacticaApplication::initialize(ServiceLocator& serviceLocator, FsmBuilder& fsmBuilder) {
	auto configFile = serviceLocator.getService<resource::ResourceSystem>().getResource<resource::IniFile>("devUserConfigFile"_id);
	auto state = configFile->getOrCreate("demo", "fsm", std::string("map"));

	return _initializePongDemo(serviceLocator, fsmBuilder);

	/*if (state == "sprite") {
		return _initializeSpriteDemo(serviceLocator, fsmBuilder);
	} else if (state == "map") {
		return _initializeMapDemo(serviceLocator, fsmBuilder);
	} else {
		return _initializeSimpleDemo(serviceLocator, fsmBuilder);
	}*/
}

HashId OpenTacticaApplication::_initializePongDemo(ServiceLocator& serviceLocator, FsmBuilder& fsmBuilder) {
	fsmBuilder
		.state<LoadState>("Load", serviceLocator, "_demoPong/resources.json", "pongDemo"_id, "spriteCamera"_id)
		.on("proceed").jumpTo("Sprites")
		.onAppExitRequest().jumpTo("Unload")

		.state<PongDemoState>("Sprites", serviceLocator)
		.on("exit").jumpTo("Unload")
		.onAppExitRequest().jumpTo("Unload")

		.state<UnloadState>("Unload", serviceLocator, "pongDemo"_id)
		.on("proceed").exitFsm()
		.onAppExitRequest().exitFsm();

	return "Load"_id;
}

HashId OpenTacticaApplication::_initializeSpriteDemo(ServiceLocator& serviceLocator, FsmBuilder& fsmBuilder) {
	fsmBuilder
		.state<LoadState>("Load", serviceLocator, "_demoSprites/resources.json", "demoSprites"_id, "spriteCamera"_id)
		.on("proceed").jumpTo("Sprites")
		.onAppExitRequest().jumpTo("Unload")

		.state<DemoSpriteState>("Sprites", serviceLocator)
		.on("exit").jumpTo("Unload")
		.onAppExitRequest().jumpTo("Unload")

		.state<UnloadState>("Unload", serviceLocator, "demoSprites"_id)
		.on("proceed").exitFsm()
		.onAppExitRequest().exitFsm();

	return "Load"_id;
}

HashId OpenTacticaApplication::_initializeMapDemo(ServiceLocator& serviceLocator, FsmBuilder& fsmBuilder) {
	fsmBuilder
		.state<LoadState>("Load", serviceLocator, "_demoMaps/resources.lua", "demoMaps"_id, "mapCamera"_id)
		.on("proceed").jumpTo("Map")
		.onAppExitRequest().jumpTo("Unload")

		.state<DemoMapState>("Map", serviceLocator)
		.on("exit").jumpTo("Unload")
		.onAppExitRequest().jumpTo("Unload")

		.state<UnloadState>("Unload", serviceLocator, "demoMaps"_id)
		.on("proceed").exitFsm()
		.onAppExitRequest().exitFsm();

	return "Load"_id;
}

HashId OpenTacticaApplication::_initializeSimpleDemo(ServiceLocator& serviceLocator, FsmBuilder& fsmBuilder) {
	fsmBuilder
		.state<LoadState>("Load", serviceLocator, "_demoSimple/resources.json", "demoSimple"_id, "rotateAroundCamera"_id)
		.on("proceed").jumpTo("DemoScene")

		.state<DemoSimpleState>("DemoScene", serviceLocator)
		.on("exit").jumpTo("Unload")
		.on("empty").jumpTo("Empty")
		.onAppExitRequest().jumpTo("Unload")

		.state<EmptyState>("Empty")
		.on("proceed").jumpTo("Unload")
		.onAppExitRequest().jumpTo("Unload")

		.state<UnloadState>("Unload", serviceLocator, "demoSimple"_id)
		.on("proceed").exitFsm()
		.onAppExitRequest().exitFsm();

	return "Load"_id;
}


}
