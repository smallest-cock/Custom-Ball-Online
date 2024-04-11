#include "pch.h"
#include "CustomBallOnline.h"


BAKKESMOD_PLUGIN(CustomBallOnline, "Custom Ball Online", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

// the playlist IDs which should use the default RL soccar ball ... from this list https://wiki.bakkesplugins.com/code_snippets/playlist_id/
std::vector<int> CustomBallOnline::acceptablePlaylistIDs = { 1, 2, 3, 4, 6, 10, 11, 13, 16, 18, 19, 22, 28, 34, 41, 50, 52, 55 };



void CustomBallOnline::onLoad()
{
	_globalCvarManager = cvarManager;

	ImGuiIO& io = ImGui::GetIO();

	// Enable keyboard navigation
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	//// Enable gamepad navigation
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	//// Enable mouse to follow navigation steps
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	

	// command cvars
	cvarManager->registerCvar("startCommand", "plugin reload acplugin; sleep 200; openmenu ac_main", "command to run before auto menu navigation", true);
	cvarManager->registerCvar("exitCommand", "closemenu ac_main", "command to run after auto menu navigation finished", true);

	// playlists
	cvarManager->registerCvar("automaticActivationPlaylists", "1 2 3 4 6 10 11 13 16 18 19 22 28 34 41 50 52 55", "playlists to automatically enable ball texture", true);

	// step cvars
	cvarManager->registerCvar("startSequenceSteps", "focus back resetNav down enter up down down", "start sequence steps", true);
	cvarManager->registerCvar("navigationSteps", "enter makeSureLoaded up up right activate down down down down enter enter exit", "menu navigation steps", true);
	
	// delay cvars
	cvarManager->registerCvar("startNavDelay", "1", "start delay", true, true, .2, true, 100);
	cvarManager->registerCvar("autoNavDelay", "2", "navigation delay", true, true, 1, true, 500);
	cvarManager->registerCvar("delayDuration", "0.2", "duration of a makeSureLoaded step", true, true, 0, true, 5);
	cvarManager->registerCvar("delayAfterJoinMatch", "0", "how long to wait after joining a match to run", true, true, 0, true, 5);

	// bool cvars
	cvarManager->registerCvar("autoNavActive", "0", "flag for checking if automatic menu navigation is active", true, true, 0, true, 1);
	cvarManager->registerCvar("runOnMatchStart", "1", "automatically run enableBallTexture at the beginning of a match", true, true, 0, true, 1);
	cvarManager->registerCvar("enableFastMode", "1", "enable fast navigation mode", true, true, 0, true, 1);

	// retries
	cvarManager->registerCvar("startSequenceRetryLimit", "4", "start sequence retry limit", true, true, 0, true, 50);
	cvarManager->registerCvar("startSequenceRetryThreshold", "4", "start sequence retry cutoff", true, true, 1, true, 50);
	cvarManager->registerCvar("navStepRetryLimit", "10", "navigation step retry limit", true, true, 0, true, 100);



	// register console commands
	cvarManager->registerNotifier("enableBallTexture", [this](std::vector<std::string> args) {
		enableBallTexture();
	}, "", PERMISSION_ALL);
	
	cvarManager->registerNotifier("currentPlaylistInfo", [this](std::vector<std::string> args) {
		bool playlistIsValid = checkPlaylist();
		LOG("++ current playlist should have default RL soccar ball: {}", playlistIsValid);

	}, "", PERMISSION_ALL);

	cvarManager->registerNotifier("startNav", [this](std::vector<std::string> args) {
		startNav();
	}, "", PERMISSION_ALL);

	cvarManager->registerNotifier("navInput", [this](std::vector<std::string> args) {
		navInput(args[1]);
	}, "", PERMISSION_ALL);
	

	// activate a specific widget based on its ImGuiID
	cvarManager->registerNotifier("activateID", [this](std::vector<std::string> args) {

		LOG("this is the content of args[1]: {}", args[1]);

		gameWrapper->SetTimeout([this, args](...){

			unsigned int idNum = std::stoll(args[1]);
			ImGuiID itemId = ImGuiID(idNum);

			ImGui::ActivateItem(itemId);

			LOG("~~ activation attempted on ID {} ~~", itemId);

			}, 3);
	}, "", PERMISSION_ALL);
	
	// get the ImGuiID for any highlighted widget, for 30s
	cvarManager->registerNotifier("getFocusID", [this](std::vector<std::string> args) {

		for (int i = 1; i < 30; i++) {

			gameWrapper->SetTimeout([this](...) {
				ImGuiID focus = ImGui::GetFocusID();

				LOG("focus ID: {}", focus);
				LOG("IsAnyItemFocused() : {}", ImGui::IsAnyItemFocused());
				}, i);
		}

	}, "", PERMISSION_ALL);
	
	// test run with a specific navigation delay value
	cvarManager->registerNotifier("customBallOnline_test", [this](std::vector<std::string> args) {

		// if nav delay and sleep time are specified
		if (args.size() == 3) {
			CVarWrapper navDelayCvar = cvarManager->getCvar("autoNavDelay");

			if (!navDelayCvar) { return; }

			int newDelay = std::stoi(args[1]);

			navDelayCvar.setValue(newDelay);

			float waitBeforeRunning = std::stof(args[2]);

			std::string command = std::to_string(waitBeforeRunning * 1000);

			cvarManager->executeCommand("plugin reload acplugin; sleep 1000; load_freeplay; sleep " + command + "; enableBallTexture");
		}
		else {
			cvarManager->executeCommand("plugin reload acplugin; sleep 1000; load_freeplay; sleep 5000; enableBallTexture");
		}


	}, "", PERMISSION_ALL);


	// hooks
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated", [this](std::string eventName) {
		onJoinedMatch();
	});


	LOG("Custom Ball Online loaded :)");
}
