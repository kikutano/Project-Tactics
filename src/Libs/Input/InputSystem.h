#pragma once

#include "Click/Click.h"

#include <SDL.h>
#include <glm/vec2.hpp>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace tactics {
namespace resource {
class IniFile;
class InputMap;
class InputAction;
class ResourceProvider;
} // namespace resource

class InputSystem {
public:
	InputSystem(std::shared_ptr<resource::IniFile> configFile,
				resource::ResourceProvider& resourceProvider,
				const glm::vec2& screenSize);
	~InputSystem();

	unsigned int getNumPlayers() const;
	const click::Player& getPlayer(click::PlayerId playerId) const;

	void changeScreenSize(const glm::vec2& screenSize);
	void processEvents(SDL_Event& event);
	void update();

	void assignInputMap(std::shared_ptr<resource::InputMap> inputMap, click::PlayerId playerId);
	void assignDevice(click::DeviceType deviceType, unsigned int deviceIndex, click::PlayerId playerId);
	void assignKeyboard(click::PlayerId playerId);
	void assignMouse(click::PlayerId playerId);
	bool hasDeviceAssigned(click::PlayerId playerId, click::DeviceType deviceType) const;
	void assignGamepad(click::PlayerId playerId, unsigned int deviceIndex);
	click::DeviceId getDeviceId(click::DeviceType deviceType, unsigned int deviceIndex) const;
	const click::DeviceData& getDevice(click::DeviceId deviceId) const;
	bool hasDevice(click::DeviceType deviceType, unsigned int deviceIndex) const;
	const click::ActionState& getActionState(click::ActionId actionId, click::PlayerId playerId);
	const click::ActionValue& getInputCodeValue(click::InputCode inputCode, click::PlayerId playerId);

private:
	void _updateDeviceAssignment();
	static void _onDeviceChanged(click::DeviceId id, click::DeviceType type, click::DeviceEvent event, void* userData);

	resource::ResourceProvider& _resourceProvider;
	std::unordered_map<click::PlayerId, std::vector<std::tuple<click::DeviceType, unsigned int>>> _playerDevices;
};

} // namespace tactics