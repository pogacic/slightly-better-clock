#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class SlightlyBetterClock: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase {

	void onLoad() override;
	void Render(CanvasWrapper canvas);

public:
	void RenderSettings() override;
};
