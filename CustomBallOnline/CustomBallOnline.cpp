#include "pch.h"
#include "CustomBallOnline.h"


BAKKESMOD_PLUGIN(CustomBallOnline, "Custom Ball Online", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void CustomBallOnline::onLoad()
{
	_globalCvarManager = cvarManager;


	// Enable keyboard navigation
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	

	// command cvars
	cvarManager->registerCvar("startCommand", "plugin reload acplugin; togglemenu ac_main", "command to run before auto menu navigation", true);
	cvarManager->registerCvar("exitCommand", "togglemenu ac_main", "commad to run after auto menu navigation", true);

	// step cvars
	cvarManager->registerCvar("navigationSteps", "start enter up down down enter delay up up up right enter down down down down enter enter exit", "menu navigation steps", true);
	
	// delay cvars
	cvarManager->registerCvar("startNavDelay", "1.2", "start delay", true, true, 0, true, 100);
	cvarManager->registerCvar("autoNavDelay", "6.0", "navigation delay", true, true, 1, true, 500);
	cvarManager->registerCvar("delayDuration", "0.7", "duration of a delay step", true, true, 0, true, 10);

	// bool cvars
	cvarManager->registerCvar("autoNavActive", "0", "flag for checking if automatic menu navigation is active", true, true, 0, true, 1);
	cvarManager->registerCvar("runOnMatchStart", "0", "automatically run enableBallTexture at the beginning of a match", true, true, 0, true, 1);

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

	// hooks
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated", [this](std::string eventName) {
		bool runOnMatchStart = cvarManager->getCvar("runOnMatchStart").getBoolValue();

		if (runOnMatchStart) {
			bool inOnlineGame = gameWrapper->IsInOnlineGame();
			bool inFreeplay = gameWrapper->IsInFreeplay();
			bool inReplay = gameWrapper->IsInReplay();

			if (inOnlineGame && !(inFreeplay || inReplay)) {

				// wait .5 seconds before starting... to give extra time to make sure shit is loaded ig
				gameWrapper->SetTimeout([this](...){
					cvarManager->executeCommand("enableBallTexture");
					}, .5);

			}
		}
	});


	LOG("Custom Ball Online loaded :)");
}
