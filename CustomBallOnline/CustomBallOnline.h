#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include <windows.h>
#include <chrono>
#include <format>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class CustomBallOnline: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase
{

	//Boilerplate
	void onLoad() override;

	// Auto nav stuff
	void enableBallTexture();
	void frameRenderCallback();
	void startNav();
	void startSequence();
	void activateBasedOnID(ImGuiID id);
	void everyGameTick();
	void clearWidgetIDs();
	void resetNavVariables();
	void navInput(std::string);
	static int frameCounter;
	static int stepCounter;
	static int delayCounter;
	static int delayStepAmount;
	static int activatedWidgetCount;
	static bool delay;
	static bool idsAreStored;
	static std::vector<ImGuiID> widgetIDs;
	static std::vector<std::string> navigationSteps;

public:
	void RenderSettings() override;
};
