#include "pch.h"
#include "CustomBallOnline.h"


std::vector<std::string> CustomBallOnline::navigationSteps;
std::vector<ImGuiID> CustomBallOnline::widgetIDs;
int CustomBallOnline::frameCounter = 0;
int CustomBallOnline::stepCounter = 0;
bool CustomBallOnline::delay = false;
bool CustomBallOnline::idsAreStored = false;
int CustomBallOnline::delayCounter = 0;
int CustomBallOnline::delayStepAmount = 0;
int CustomBallOnline::activatedWidgetCount = 0;


// TODO: maybe eventually make a function to find the ID of a widget based on its label text or fixed position in the menu


// TODO: find out why tf widget IDs change (but not the last one?) every 2 games or whatever the fuck
//		 is it a memory address thing?
//		 is it a timeout thing?
//		 does it happen on a RL specific event/function?
//		 is there a way to prevent it? perhaps using ImGui::KeepAliveID() ?
//		 if not way to prevent it, is there a way to track it? maybe based on event/function hook?


// TODO: make start sequence nav steps editable, with dedicated text input in settings like regular nav steps


// TODO: only store 3 widget IDs and not the last one (bc it prevents user from changing their ball in AC and having it stay after auto nav.. need to clear IDs 1st which sucks)
//		 ... replace with just 'enter' nav step... bc that's all it should take to activate the last used texture once the dropdown is opened



std::vector<std::string> parseWords(const std::string& input) {
	std::vector<std::string> words;
	std::istringstream iss(input);
	std::string word;
	while (iss >> word) {
		words.push_back(word);
	}
	return words;
}


void CustomBallOnline::enableBallTexture() {
	std::string startCommand = cvarManager->getCvar("startCommand").getStringValue();

	// determine appropriate delay step amount based on avg game fps
	int navDelay = cvarManager->getCvar("autoNavDelay").getIntValue();
	float delayDuration = cvarManager->getCvar("delayDuration").getFloatValue();
	ImGuiIO& io = ImGui::GetIO();
	float gameFramerate = io.Framerate;
	LOG("<<< avg framerate: {} >>>", gameFramerate);

	// convert calculated value to int.. and add 1 to make sure there's at least 1 nav step worth of delay
	delayStepAmount = (int)((gameFramerate * delayDuration) / navDelay) + 1;		// let delay duration (seconds) be X .... X = delayAmount * (navDelay / gameFramerate)
																					//										  X * gameFramerate = delayAmount * navDelay
																					//									      (X * gameFramerate) / navDelay = delayAmount

	gameWrapper->Execute([startCommand, this](GameWrapper* gw) {
		cvarManager->executeCommand(startCommand);
		cvarManager->executeCommand("automateNav");
		});
}


void CustomBallOnline::startNav() {
	std::string navigationStepsStr = cvarManager->getCvar("navigationSteps").getStringValue();
	float startDelay = cvarManager->getCvar("startNavDelay").getFloatValue();

	// generate step arrays
	navigationSteps = parseWords(navigationStepsStr);

	LOG("it finna be {} navigation steps", navigationSteps.size());

	// reset counters
	resetNavVariables();

	gameWrapper->SetTimeout([this](...) {
		cvarManager->getCvar("autoNavActive").setValue(true);

		gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", [this](std::string eventName) {
			everyGameTick();
			});

		}, startDelay);
}


void CustomBallOnline::everyGameTick() {
	// check if should run callback
	bool navActive = cvarManager->getCvar("autoNavActive").getBoolValue();
	if (navActive) {
		frameRenderCallback();
	}
	else {
		gameWrapper->UnhookEvent("Function Engine.GameViewportClient.Tick");
		LOG("unhooked from game tick event...");

		// test
		for (int i = 0; i < widgetIDs.size(); i++) {
			LOG("widget id {}: {}", i + 1, widgetIDs[i]);
		}

		// determine if fast mode is possible
		if (widgetIDs.size() == 3) {
			idsAreStored = true;
		}

		// testing
		ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

		if (!onlineServer) {
			LOG("server is null -_-");
			return;
		}

		std::string matchType = onlineServer.GetMatchTypeName();
		GameSettingPlaylistWrapper playlist = onlineServer.GetPlaylist();
		if (!playlist) { return; }

		std::string playlistName = playlist.GetName();
		int playlistID = playlist.GetPlaylistId();
		LOG("++ playlistName: {}", playlistName);
		LOG("++ playlistID: {}", playlistID);
	}
}


// should make it so navigation steps start from the same position every time (no variance)
// should leave off with DSM button highlighted/focused (but not clicked) every time
void CustomBallOnline::startSequence() {

	if (stepCounter == 0) {
		ImGui::SetWindowFocus("AlphaConsole Plugin");
		navInput("down");
	}
	if (stepCounter == 1) {
		navInput("enter");
	}
	else if (stepCounter == 2) {
		navInput("up");
	}
	else if (stepCounter == 3 || stepCounter == 4) {
		navInput("down");
	}

	// DSM should be highlighted/focused at this point (but not pressed)
	else if (stepCounter == 5) {
		LOG("~~~ 'Disable Safe Mode' button should be highlighted rn... if it's not, startSequence() needs fixing ~~~");

		ImGuiID currentHighlightedID = ImGui::GetFocusID();

		if ((unsigned long)currentHighlightedID > 0) {
			if (idsAreStored) {
				// check if the current highlighted ID is the same as the 1st ID in the stored list (stored DSM ID)
				if (!(currentHighlightedID == widgetIDs[0])) {
					LOG("~~ stored DSM ID ({}) is invalid.. the valid ID is now {} ~~", widgetIDs[0], currentHighlightedID);
					clearWidgetIDs();
				}
			}
		}
	}
}


// to be ran on each UI frame during auto nav
void CustomBallOnline::frameRenderCallback() {
	frameCounter++;

	// on every x amount of frames
	int navDelay = cvarManager->getCvar("autoNavDelay").getIntValue();
	if (frameCounter % navDelay == 0) {

		if (!delay) {

			 startSequence();

			// after start sequence	(start sequence takes 5 steps)
			if (stepCounter >= 5) {

				// if a fastmode run
				bool fastModeEnabled = cvarManager->getCvar("enableFastMode").getBoolValue();
				if (fastModeEnabled && idsAreStored) {
					// make sure index is in range
					if ((stepCounter - 5) >= 0 && (stepCounter - 5) < widgetIDs.size()) {
						int widgetIndex = stepCounter - 5;
						ImGuiID currentSavedID = widgetIDs[widgetIndex];
					
						// dsm
						if (widgetIndex == 0) {
							activateBasedOnID(currentSavedID);
							delay = true;
						}
						// others
						else {
							activateBasedOnID(currentSavedID);
						}
					}
					// the next step after all the IDs in widgetIDs have been activated
					else if ((stepCounter - 5) == widgetIDs.size()) {
						navInput("enter");
					}
					// else... there must be no more steps/IDs
					else {
						LOG("... no more IDs to activate");
						gameWrapper->Execute([this](GameWrapper* gw) {
							cvarManager->getCvar("autoNavActive").setValue(false);
							cvarManager->executeCommand("sleep 200; navInput exit");
							LOG("........ t'was a fast mode run");
							});
						return;
					}
				}

				// if a regular run
				else {
					int navStepIndex = stepCounter - 5;

					// if there are more nav steps
					if (navStepIndex < navigationSteps.size()) {
						std::string currentStep = navigationSteps[navStepIndex];

						if (currentStep != "delay") {
							//gameWrapper->Execute([currentStep, this](GameWrapper* gw) {
							//	navInput(currentStep);
							//	});
							navInput(currentStep);
						}
						else {
							delay = true;
						}
					}
					// if nav steps are finished
					else {
						LOG("... no more nav steps");
						gameWrapper->Execute([this](GameWrapper* gw) {
							cvarManager->getCvar("autoNavActive").setValue(false);
							});
						return;
					}
				}
			}
			stepCounter++;
		}
		else {
			delayCounter++;
			LOG("~~~~ delay step count: {} ~~~~", delayCounter);

			if (delayCounter == delayStepAmount) {
				delay = false;
				delayCounter = 0;
			}
		}
	}
}


void CustomBallOnline::navInput(std::string keyName) {


	// TODO: maybe map the different possible keyNames to an enum, and use switch statement with enum


	ImGuiIO& io = ImGui::GetIO();
	io.NavActive = true;
	io.NavVisible = true;

	if (keyName == "up") {
		io.NavInputs[ImGuiNavInput_DpadUp] = 1.0f;
	}
	else if (keyName == "down") {
		io.NavInputs[ImGuiNavInput_DpadDown] = 1.0f;
	}
	else if (keyName == "left") {
		io.NavInputs[ImGuiNavInput_DpadLeft] = 1.0f;
	}
	else if (keyName == "right") {
		io.NavInputs[ImGuiNavInput_DpadRight] = 1.0f;
	}
	else if (keyName == "enter") {
		ImGuiID currentHighlightedID = ImGui::GetFocusID();
		ImGui::KeepAliveID(currentHighlightedID);

		if (((unsigned long)currentHighlightedID > 0) && stepCounter > 4) {
				if (widgetIDs.size() < 3) {
					widgetIDs.push_back(currentHighlightedID);
				}
		}
		io.NavInputs[ImGuiNavInput_Activate] = 1.0f;
		ImGui::KeepAliveID(currentHighlightedID);
	}
	else if (keyName == "back") {
		io.NavInputs[ImGuiNavInput_Cancel] = 1.0f;
	}
	else if (keyName == "exit") {
		gameWrapper->Execute([this](GameWrapper* gw) {
			std::string exitCommand = cvarManager->getCvar("exitCommand").getStringValue();
			cvarManager->executeCommand(exitCommand);
			});
	}

	//findWidgetID("test");
	LOG("[nav step {}] simulated *** {} ***", stepCounter, keyName);
}


void CustomBallOnline::clearWidgetIDs() {
	widgetIDs.clear();
	idsAreStored = false;
	LOG("~~~ cleared stored item IDs ~~~");
}


void CustomBallOnline::activateBasedOnID(ImGuiID id) {
	ImGui::KeepAliveID(id);
	ImGui::ActivateItem(id);
	ImGui::KeepAliveID(id);			// putting in work (i think)
	LOG("~~ attempted activation on ID {} ~~", id);
}


void CustomBallOnline::resetNavVariables() {
	frameCounter = 0;
	stepCounter = 0;
	delayCounter = 0;
	delay = false;
}