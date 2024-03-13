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

	// BallTexture stuff
	void enableBallTexture();
	void frameRenderCallback();
	void startNav();
	void everyGameTick();
	void resetNavVariables();
	void navInput(std::string);
	static int frameCounter;
	static int stepCounter;
	static int delayCounter;
	static int delayStepAmount;
	static bool delay;
	static std::vector<std::string> navigationSteps;

public:
	void RenderSettings() override;
};
