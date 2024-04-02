#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include <windows.h>
#include <shellapi.h>
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

	// Auto nav stuff and such
	void onJoinedMatch();
	void enableBallTexture();
	void frameRenderCallback();
	void startNav();
	void startSequence();
	void activateBasedOnID(ImGuiID id);
	void everyGameTick();
	void clearWidgetIDs();
	void resetNavVariables();
	bool focusedItemHasChanged();
	bool checkIfACLoaded();
	bool checkPlaylist();
	bool playlistExists();
	void navInput(std::string);
	static int frameCounter;
	static int stepCounter;
	static int delayCounter;
	static int delayStepAmount;
	static int activatedWidgetCount;
	static int focusDidntChangeCount;
	static int stepRetries;
	static bool delay;
	static bool finnaEndDelay;
	static bool idsAreStored;
	static bool playlistFound;
	static std::vector<ImGuiID> widgetIDs;
	static std::vector<ImGuiID> highlightedWidgets;
	static std::vector<std::string> navigationSteps;
	static std::vector<std::string> startSequenceSteps;
	static std::vector<int> acceptablePlaylistIDs;

public:
	void RenderSettings() override;
};
