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

	// oink oink oink
	void automateNav();
	void activateBasedOnID(ImGuiID id);
	void gameTickCallback();
	void clearWidgetIDs();
	void resetNavVariables();
	void retryStartSequence();
	void unhookFromTickEvent();
	bool focusedItemHasChanged();
	bool checkIfACLoaded();
	bool checkPlaylist();
	bool playlistExists();

	// ok pal
	void onJoinedMatch();
	void enableBallTexture();
	void startNav();
	void startSequence();
	void remainingSteps();
	void blindMode();
	void fastMode();
	void navInput(const std::string& step);


	static int frameCounter;
	static int stepCounter;
	static int delayCounter;
	static int delayStepAmount;
	static int activatedWidgetCount;
	static int focusDidntChangeCount;
	static int stepRetries;
	static int startSequenceRetries;
	static int navStepRetries;
	static int lastRetriedStep;
	static bool delay;
	static bool finnaEndDelay;
	static bool idsAreStored;
	static bool playlistFound;
	static bool startSequenceFocusChanged;
	static ImGuiID firstHighlightedItem;
	static std::vector<ImGuiID> widgetIDs;
	static std::vector<ImGuiID> highlightedWidgets;
	static std::vector<std::string> navigationSteps;
	static std::vector<std::string> startSequenceSteps;
	static std::vector<int> acceptablePlaylistIDs;

public:
	static std::vector<std::string> parseWords(const std::string& input);
	void RenderSettings() override;
};
