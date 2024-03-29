#include "pch.h"
#include "SlightlyBetterClock.h"
#include "PersistentStorage.h"
#include <chrono>
#include <ctime>
#include <sstream>

BAKKESMOD_PLUGIN(SlightlyBetterClock, "SBClock Plugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
std::shared_ptr<PersistentStorage> _persistent_storage;

void SlightlyBetterClock::onLoad()
{
	// Define cvar manager
	_globalCvarManager = cvarManager;

	// I think this is how you use PersistentStorage.. it works? so bleh.
	_persistent_storage = std::make_shared<PersistentStorage>(this, "sbclock", true, true);

	// Clock: show
	cvarManager->registerCvar("sbc_show", "1", "Clock show", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		showClock = cvar.getBoolValue();
			});
	_persistent_storage->RegisterPersistentCvar("sbc_show", "1", "Clock show", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		showClock = cvar.getBoolValue();
			});

	// Clock: format
	cvarManager->registerCvar("sbc_format", "time_date", "Clock time format");
	_persistent_storage->RegisterPersistentCvar("sbc_format", "time_date", "Clock time format");

	// Clock: location
	cvarManager->registerCvar("sbc_location", "bottom_right", "Clock location");
	_persistent_storage->RegisterPersistentCvar("sbc_location", "bottom_right", "Clock location");

	// Text color
	cvarManager->registerCvar("sbc_color", "#FFFF00", "Color of text");
	_persistent_storage->RegisterPersistentCvar("sbc_color", "#FFFF00", "Color of text");

	// Fillbox: alpha
	cvarManager->registerCvar("sbc_fb_alpha", "150.0", "Fillbox alpha value");
	_persistent_storage->RegisterPersistentCvar("sbc_fb_alpha", "150.0", "Fillbox alpha value");

	// Clock: scale
	cvarManager->registerCvar("sbc_scale", "1.0", "Clock scale value");
	_persistent_storage->RegisterPersistentCvar("sbc_scale", "1.0", "Clock scale value");

	// Draw text
	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		Render(canvas);
	});

	// Nice
	LOG("SBClock Plugin loaded!");
}
