#include "pch.h"
#include "CustomBallOnline.h"


BAKKESMOD_PLUGIN(CustomBallOnline, "Custom Ball Online", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void CustomBallOnline::onLoad()
{
	_globalCvarManager = cvarManager;

	ImGuiIO& io = ImGui::GetIO();

	// Enable keyboard navigation
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	//// Enable mouse to follow navigation steps
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	

	// command cvars
	cvarManager->registerCvar("startCommand", "plugin reload acplugin; togglemenu ac_main", "command to run before auto menu navigation", true);
	cvarManager->registerCvar("exitCommand", "togglemenu ac_main", "commad to run after auto menu navigation", true);

	// step cvars
	cvarManager->registerCvar("navigationSteps", "enter delay up up up right enter down down down down enter enter exit", "menu navigation steps", true);
	
	// delay cvars
	cvarManager->registerCvar("startNavDelay", "1.2", "start delay", true, true, 0, true, 100);
	cvarManager->registerCvar("autoNavDelay", "7", "navigation delay", true, true, 1, true, 500);
	cvarManager->registerCvar("delayDuration", "0.7", "duration of a delay step", true, true, 0, true, 10);

	// bool cvars
	cvarManager->registerCvar("autoNavActive", "0", "flag for checking if automatic menu navigation is active", true, true, 0, true, 1);
	cvarManager->registerCvar("runOnMatchStart", "1", "automatically run enableBallTexture at the beginning of a match", true, true, 0, true, 1);
	cvarManager->registerCvar("enableFastMode", "1", "enable fast navigation mode", true, true, 0, true, 1);


	// register console commands
	cvarManager->registerNotifier("enableBallTexture", [this](std::vector<std::string> args) {
		enableBallTexture();
	}, "", PERMISSION_ALL);

	cvarManager->registerNotifier("automateNav", [this](std::vector<std::string> args) {
		startNav();
	}, "", PERMISSION_ALL);

	cvarManager->registerNotifier("navInput", [this](std::vector<std::string> args) {
		navInput(args[1]);
	}, "", PERMISSION_ALL);
	
	cvarManager->registerNotifier("activateID", [this](std::vector<std::string> args) {

		LOG("this is the content of args[1]: {}", args[1]);

		gameWrapper->SetTimeout([this, args](...){

			unsigned int idNum = std::stoll(args[1]);
			ImGuiID itemId = ImGuiID(idNum);

			ImGui::ActivateItem(itemId);

			LOG("~~ activation attempted on ID {} ~~", itemId);

			}, 3);
	}, "", PERMISSION_ALL);
	
	cvarManager->registerNotifier("getFocusID", [this](std::vector<std::string> args) {

		for (int i = 1; i < 30; i++) {

			gameWrapper->SetTimeout([this](...) {
				ImGuiID focus = ImGui::GetFocusID();
				LOG("focus ID: {}", focus);
				}, i);
		}

	}, "", PERMISSION_ALL);


	// hooks
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated", [this](std::string eventName) {
		bool runOnMatchStart = cvarManager->getCvar("runOnMatchStart").getBoolValue();

		if (runOnMatchStart) {
			bool inFreeplay = gameWrapper->IsInFreeplay();
			bool inReplay = gameWrapper->IsInReplay();

			if (!(inFreeplay || inReplay)) {

				// wait 1 second after all teams created... to perhaps give some extra time for things to load before starting?
				gameWrapper->SetTimeout([this](...) {
					cvarManager->executeCommand("enableBallTexture");
					}, 1); 

			}
		}
	});


	LOG("Custom Ball Online loaded :)");
}
