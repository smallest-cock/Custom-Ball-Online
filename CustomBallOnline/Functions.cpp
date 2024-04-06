#include "pch.h"
#include "CustomBallOnline.h"


std::vector<std::string> CustomBallOnline::navigationSteps;
std::vector<std::string> CustomBallOnline::startSequenceSteps;
std::vector<ImGuiID> CustomBallOnline::widgetIDs;
std::vector<ImGuiID> CustomBallOnline::highlightedWidgets;
bool CustomBallOnline::delay = false;
bool CustomBallOnline::finnaEndDelay = false;
bool CustomBallOnline::playlistFound = false;
bool CustomBallOnline::idsAreStored = false;
bool CustomBallOnline::startSequenceFocusChanged = false;
int CustomBallOnline::frameCounter = 0;
int CustomBallOnline::stepCounter = 0;
int CustomBallOnline::delayCounter = 0;
int CustomBallOnline::delayStepAmount = 0;
int CustomBallOnline::focusDidntChangeCount = 0;
int CustomBallOnline::activatedWidgetCount = 0;
int CustomBallOnline::stepRetries = 0;
int CustomBallOnline::startSequenceRetries = 0;
int CustomBallOnline::navStepRetries = 0;
int CustomBallOnline::lastRetriedStep = 0;
ImGuiID CustomBallOnline::firstHighlightedItem = 420;



std::vector<std::string> parseWords(const std::string& input) {
	std::vector<std::string> words;
	std::istringstream iss(input);
	std::string word;
	while (iss >> word) {
		words.push_back(word);
	}
	return words;
}


std::vector<int> parseInts(const std::string& input) {
	std::vector<int> playlistIDs = {};

	std::vector<std::string> parsedStingInts = parseWords(input);
	for ( std::string intStr : parsedStingInts) {
		try
		{
			int intID = std::stoi(intStr);
			playlistIDs.push_back(intID);
		}
		catch (...)
		{
			LOG("***\tError reading playlist ID numbers!\t***");
			LOG("***\t... make sure the text only contains numbers and spaces\t***");
		}
	}
	LOG("{} playlists will automatically enable custom ball texture once joined the match", playlistIDs.size());
	return playlistIDs;
}


// determine if/when to automatically start navigation
void CustomBallOnline::onJoinedMatch() {
	playlistFound = false;
	bool runOnMatchStart = cvarManager->getCvar("runOnMatchStart").getBoolValue();

	if (runOnMatchStart) {
		bool inFreeplay = gameWrapper->IsInFreeplay();
		bool inReplay = gameWrapper->IsInReplay();

		if (!(inFreeplay || inReplay)) {

			// update list of playlist IDs
			CVarWrapper playlistIDsCvar = cvarManager->getCvar("automaticActivationPlaylists");
			if (!playlistIDsCvar) { return; }
			std::string playlistIDsStr = playlistIDsCvar.getStringValue();
			acceptablePlaylistIDs.clear();
			acceptablePlaylistIDs = parseInts(playlistIDsStr);

			// probe to see if match server and/or playlist is null ... for 2 seconds (0.2s * 10)
			for (int i = 0; i < 10; i++) {

				gameWrapper->SetTimeout([this](...) {

					if (!playlistFound) {

						if (playlistExists()) {

							playlistFound = true;

							// wait some time after all teams created... to perhaps give more room for things to load before starting?
							float delayAfterJoinMatch = cvarManager->getCvar("delayAfterJoinMatch").getFloatValue();
							gameWrapper->SetTimeout([this](...) {

								// check if current playlist ID is found in acceptablePlaylistIDs
								if (checkPlaylist()) {
									cvarManager->executeCommand("enableBallTexture");
								}

							}, delayAfterJoinMatch);
						}
					}
				}, .2 * i);
			}
		}
	}
}


void CustomBallOnline::enableBallTexture() {
	std::string startCommand = cvarManager->getCvar("startCommand").getStringValue();

	// determine appropriate delay step amount based on avg game fps
	int navDelay = cvarManager->getCvar("autoNavDelay").getIntValue();
	float delayDuration = cvarManager->getCvar("delayDuration").getFloatValue();
	ImGuiIO& io = ImGui::GetIO();
	float gameFramerate = io.Framerate;
	LOG("<<< avg framerate: {} >>>", gameFramerate);

	//// Disable mouse inputs entirely
	//io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
	
	//// Enable mouse to follow navigation steps
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	//LOG("~~ enabled mouse move with nav ~~");

	// convert calculated value to int.. and add 1 to make sure there's at least 1 nav step worth of delay
	delayStepAmount = (int)((gameFramerate * delayDuration) / navDelay) + 1;		// let delay duration (seconds) be X .... X = delayAmount * (navDelay / gameFramerate)
																					//										  X * gameFramerate = delayAmount * navDelay
																					//									      (X * gameFramerate) / navDelay = delayAmount

	cvarManager->executeCommand(startCommand);
	cvarManager->executeCommand("startNav");
}


void CustomBallOnline::startNav() {
	// first and foremost
	cvarManager->getCvar("autoNavActive").setValue(true);

	ImGuiContext *currentContext = ImGui::GetCurrentContext();
	currentContext->NavDisableHighlight = false;
	
	std::string navigationStepsStr = cvarManager->getCvar("navigationSteps").getStringValue();
	std::string startSequenceStepsStr = cvarManager->getCvar("startSequenceSteps").getStringValue();
	float startDelay = cvarManager->getCvar("startNavDelay").getFloatValue();

	// generate step arrays
	startSequenceSteps = parseWords(startSequenceStepsStr);
	navigationSteps = parseWords(navigationStepsStr);
	
	// reset counters
	resetNavVariables();

	LOG("start sequence is {} steps", startSequenceSteps.size());
	LOG("... then {} remaining navigation steps", navigationSteps.size());

	gameWrapper->SetTimeout([this](...) {
		// make sure nothing is active that would prevent auto nav (i.e. a text input)
		ImGui::ClearActiveID();
		LOG("cleared active ID (to make sure nothing else 'eats' the nav steps)");

		gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", [this](std::string eventName) {
			everyGameTick();
		});
		LOG("<<<\thooked into game tick event\t>>>");

	}, startDelay);
}


bool CustomBallOnline::checkIfACLoaded() {
	CVarWrapper acCvar = cvarManager->getCvar("acplugin_wheeltexture_selectedtexture_orange");

	if (!acCvar) {
		return false;
	}

	std::string wheelTexture = acCvar.getStringValue();

	LOG("successfully accessed acplugin_wheeltexture_selectedtexture_orange CVar :)");
	LOG("... its value is: {}", wheelTexture);

	return true;
}


bool CustomBallOnline::focusedItemHasChanged() {
	ImGuiID currentlyHighlightedItem = ImGui::GetFocusID();
	if (!highlightedWidgets.empty()) {
		ImGuiID lastHighlightedItem = highlightedWidgets.back();
		if (currentlyHighlightedItem != lastHighlightedItem) {
			highlightedWidgets.push_back(currentlyHighlightedItem);
			return true;
		}
		else {
			focusDidntChangeCount++;
			return false;
		}
	}
	else {
		highlightedWidgets.push_back(currentlyHighlightedItem);
		return true;
	}
}


void CustomBallOnline::everyGameTick() {
	// check if should run callback
	bool navActive = cvarManager->getCvar("autoNavActive").getBoolValue();
	if (navActive) {
		if (delay) {
			if (finnaEndDelay) { return; }

			if (delayCounter < 1000) {
				if (checkIfACLoaded()) {
					LOG("AC has been loaded and we finna set delay = false after the delay duration...");
					finnaEndDelay = true;
					float delayDuration = cvarManager->getCvar("delayDuration").getFloatValue();
					gameWrapper->SetTimeout([this](...) {
						finnaEndDelay = false;
						delay = false;
						delayCounter = 0;
					}, delayDuration);
				}
			}
			// unhook from tick event if AlphaConsole Cvar isn't found after 1000 frames (something probably went wrong)
			else {
				unhookFromTickEvent();

				LOG("... something went wrong :(");
				LOG("... make sure the start sequence is good");

				// abort on error exit command ... maybe make editable?
				std::string exitCommand = cvarManager->getCvar("exitCommand").getStringValue();
				cvarManager->executeCommand(exitCommand);
			}
			delayCounter++;
		}
		else {
			frameRenderCallback();
		}
	}
	else {
		unhookFromTickEvent();

		// determine if fast mode is possible
		if (widgetIDs.size() == 3) {
			idsAreStored = true;
		}

		// logging
		for (int i = 0; i < widgetIDs.size(); i++) {
			LOG("widget id {}: {}", i + 1, widgetIDs[i]);
		}
		//checkPlaylist();
		LOG("+++\tnumber of nav steps that didnt change the highlighted item: {}", focusDidntChangeCount);
	}
}


// -------  goal: remaining navigation steps should start from the same position every time (no variance)  ---------
// start sequence should leave off with 'Disable Safe Mode' button highlighted/focused (but not clicked) every time
void CustomBallOnline::startSequence() {
	LOG("------------------------------------------------");		// visual aid to distinguish different steps in console


	// detect if focus has changed (from the 1st non-zero item ID found)
	if (!startSequenceFocusChanged) {
		// ignore the 1st & 2nd steps 
		if (stepCounter > 1) {
			ImGuiID currentFocusedItem = ImGui::GetFocusID();
			if (currentFocusedItem > 0) {

				// save the first highlighted item with a non-zero ID
				if (firstHighlightedItem == 420) {
					firstHighlightedItem = currentFocusedItem;
					LOG("<<< saved the first non-zero focus ID: {} >>>", firstHighlightedItem);
				}

				if (currentFocusedItem != firstHighlightedItem) {
					startSequenceFocusChanged = true;
					LOG("<<< start sequence item focus has changed :) >>>");
				}
			}
		}
	}


	// after x nav step, check if any item is highlighted... to determine if start sequence (or a remaining nav step) needs to be retried
	CVarWrapper startSequenceRetryThresholdCvar = cvarManager->getCvar("startSequenceRetryThreshold");
	if (!startSequenceRetryThresholdCvar) {
		LOG("*** startSequenceRetryThresholdCvar is null!! ***");
		return;
	}
	int startSequenceRetryThreshold = startSequenceRetryThresholdCvar.getIntValue();

	// if past x start sequence step
	if (stepCounter > (startSequenceRetryThreshold - 1)) {

		// if no item is highlighted
		if (!ImGui::IsAnyItemFocused()) {

			// if still in start sequence
			if (stepCounter <= startSequenceSteps.size()) {
				LOG("*** we're past step {} in start sequence and no item is currently focused ***", startSequenceRetryThreshold);
				retryStartSequence();
			}
			// if in remaining steps
			else {
				LOG("*** we're done with start sequence and no item is currently focused ***");
				
				// get retry limit
				CVarWrapper navStepRetryLimitCvar = cvarManager->getCvar("navStepRetryLimit");
				if (!navStepRetryLimitCvar) {
					LOG("*** navStepRetryLimitCvar is null!! ***");
					return;
				}
				int navStepRetryLimit = navStepRetryLimitCvar.getIntValue();

				// determine whether to reset nav step retry counter (if current step is different than lastRetriedStep)
				if (stepCounter > lastRetriedStep) {
					navStepRetries = 0;
					lastRetriedStep = stepCounter;
				}

				if (navStepRetries < navStepRetryLimit) {
					// retry step that didnt get executed because nothing was focused
					stepCounter--;
					LOG("... decremented step counter in order to retry last step");
					LOG("retrying nav step........");
					navStepRetries++;
					return;
				}
				else {
					// maybe do something here if the nav step retries reached the limit ... like exit
					// ... or do nothing, which will just move on to the next step 
				}
			}
		}
	}


	// start sequence steps
	if (stepCounter < startSequenceSteps.size()) {
		navInput(startSequenceSteps[stepCounter]);
	}

	// after start sequence nav steps have completed... DSM should be highlighted/focused at this point (but not pressed)
	else if (stepCounter == startSequenceSteps.size()) {

		// make sure start sequence wasnt stuck on one item the entire time (causing navigation steps to not take effect)
		if (!startSequenceFocusChanged) {
			LOG("*** start sequence steps are done and the focus ID hasnt changed :( ***");
			retryStartSequence();
		}

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

		startSequence();

		// after start sequence
		if (stepCounter >= startSequenceSteps.size()) {

			// if a fastmode run
			bool fastModeEnabled = cvarManager->getCvar("enableFastMode").getBoolValue();
			if (fastModeEnabled && idsAreStored) {

				// repeat step if necessary
				if (!focusedItemHasChanged()) {
					if (stepRetries < 3) {
						stepCounter--;
						stepRetries++;

						LOG("****\trepeated activation by ID\t****");
					}
					else {
						stepRetries = 0;
					}
				}
				else {
					stepRetries = 0;
				}


				// make sure index is in range
				if ((stepCounter - startSequenceSteps.size()) >= 0 && (stepCounter - startSequenceSteps.size()) < widgetIDs.size()) {
					int widgetIndex = stepCounter - startSequenceSteps.size();
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
				else if ((stepCounter - startSequenceSteps.size()) == widgetIDs.size()) {
					navInput("enter");
				}
				// else... there must be no more steps/IDs
				else {
					LOG("... no more IDs to activate");
					cvarManager->getCvar("autoNavActive").setValue(false);
					cvarManager->executeCommand("sleep 200; navInput exit");
					LOG("........ t'was a fast mode run");
					return;
				}
			}

			// if a regular run
			else {
				int navStepIndex = stepCounter - startSequenceSteps.size();
				
				// repeat step if necessary
				if (!focusedItemHasChanged()) {
					if ((navStepIndex - 1) >= 0) {
						std::string prevStep = navigationSteps[navStepIndex - 1];

						if (navStepIndex < navigationSteps.size()) {
							std::string currentStep = navigationSteps[navStepIndex];

							if ((prevStep == "up" || prevStep == "down" || prevStep == "right" || prevStep == "enter") && currentStep != "makeSureLoaded") {
								if (stepRetries < 3) {
									stepCounter--;
									stepRetries++;

									LOG("****\trepeated [{}] step\t****", prevStep);
								}
								else {
									stepRetries = 0;
								}
							}
						}
					}
				}
				else {
					stepRetries = 0;
				}

				navStepIndex = stepCounter - startSequenceSteps.size();

				// execute next nav step
				if (navStepIndex < navigationSteps.size()) {
					std::string currentStep = navigationSteps[navStepIndex];
					navInput(currentStep);
				}
				// if all nav steps are finished
				else {
					LOG("... no more nav steps");
					cvarManager->getCvar("autoNavActive").setValue(false);
					unhookFromTickEvent();
					return;
				}
			}
		}
		stepCounter++;
	}
}


void CustomBallOnline::navInput(std::string keyName) {


	// TODO: maybe map the different possible keyNames to an enum, and use switch statement with enum for sexier code


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
	else if (keyName == "enter" || keyName == "activate") {
		ImGuiID currentHighlightedID = ImGui::GetFocusID();
		ImGui::KeepAliveID(currentHighlightedID);

		if (((unsigned long)currentHighlightedID > 0) && stepCounter >= startSequenceSteps.size()) {
				if (widgetIDs.size() < 3) {
					widgetIDs.push_back(currentHighlightedID);
				}
		}
		io.NavInputs[ImGuiNavInput_Activate] = 1.0f;
		ImGui::KeepAliveID(currentHighlightedID);
	}
	else if (keyName == "idInfo") {
		ImGuiWindow *acWin = ImGui::FindWindowByName("AlphaConsole Plugin");
		if (acWin) {
			LOG("[resetNav] seems like acWin aint null ....");
			ImGuiID lastItem = acWin->DC.LastItemId;
			LOG("this should be the ID of the last item in the AC window: {}", lastItem);
			ImGuiID lastNavID = acWin->NavLastIds[0];
			LOG("this should be the last Nav ID for the AC window: {}", lastNavID);
			
			if (lastNavID == 0) {
				ImVector<ImGuiID> acWinIDStack = acWin->IDStack;
				if (!acWinIDStack.empty()) {
					LOG("these are the IDs in the AC window ID stack ....");

					for (int i = 0; i < acWinIDStack.size(); i++) {
						LOG("[{}] -- {}", i, acWinIDStack[i]);
					}
				}
			}

		}
		else {
			LOG("acWin is null -_-");
		}
	}
	else if (keyName == "back") {
		io.NavInputs[ImGuiNavInput_Cancel] = 1.0f;
	}
	else if (keyName == "alt") {
		io.NavInputs[ImGuiNavInput_KeyMenu_] = 1.0f;
	}
	else if (keyName == "resetNav") {
		GImGui->NavId = 0;
	}
	else if (keyName == "focus") {
		// idk if this is working
		ImGui::SetWindowFocus("AlphaConsole Plugin");

		ImGuiWindow* acWin = ImGui::FindWindowByName("AlphaConsole Plugin");
		if (acWin) {
			LOG("[focus] seems like acWin aint null ....");
			ImGui::FocusWindow(acWin);
		}
		else {
			LOG("acWin is null -_-");
		}

		// idk if this has any effect (ID would already be 0)
		GImGui->NavId = 0;
	}
	else if (keyName == "makeSureLoaded") {
		delay = true;
	}
	else if (keyName == "exit") {
		std::string exitCommand = cvarManager->getCvar("exitCommand").getStringValue();
		cvarManager->executeCommand(exitCommand);
	}
	else {
		LOG("*** Error: {} is not a valid navigation step ***");
	}

	// debug logging
	//LOG("------------------------------------------------");
	ImGuiID currentFocusID = ImGui::GetFocusID();
	LOG("[nav stepCounter {}] current focus ID: {}", stepCounter, currentFocusID);
	LOG("[nav stepCounter {}] IsAnyItemFocused() : {}", stepCounter, ImGui::IsAnyItemFocused());


	LOG("[nav stepCounter {}] simulated *** {} ***", stepCounter, keyName);
}


void CustomBallOnline::retryStartSequence() {
	// get retry limit
	CVarWrapper startSequenceRetryLimitCvar = cvarManager->getCvar("startSequenceRetryLimit");
	if (!startSequenceRetryLimitCvar) {
		LOG("*** startSequenceRetryLimitCvar is null!! ***");
		return;
	}
	int startSequenceRetryLimit = startSequenceRetryLimitCvar.getIntValue();

	if (startSequenceRetries < startSequenceRetryLimit) {
		// retry start sequence and hope something gets focused
		stepCounter = 0;
		firstHighlightedItem == 420;
		startSequenceFocusChanged = false;
		LOG("retrying start sequence........");
		return;
	}
	else {
		// exit run ... bc something fucked up
		LOG("reached the start sequence retry limit of {}", startSequenceRetryLimit);
		LOG(".... exiting because something in the start sequence is fucked and needs fixing");
		cvarManager->getCvar("autoNavActive").setValue(false);
		cvarManager->executeCommand("sleep 200; navInput exit");
		return;
	}
	startSequenceRetries++;
}


bool CustomBallOnline::playlistExists() {
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (!onlineServer) {
		LOG("server is null -_-");
		return false;
	}

	GameSettingPlaylistWrapper playlist = onlineServer.GetPlaylist();

	if (!playlist) { 
		LOG("playlist is null -_-");
		return false; 
	}

	return true;
}

bool CustomBallOnline::checkPlaylist() {
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (!onlineServer) {
		LOG("server is null -_-");
		return false;
	}

	GameSettingPlaylistWrapper playlist = onlineServer.GetPlaylist();

	if (!playlist) { return false; }

	std::string playlistName = playlist.GetName();
	int playlistID = playlist.GetPlaylistId();
	LOG("++ playlistName: {}", playlistName);
	LOG("++ playlistID: {}", playlistID);

	return std::find(acceptablePlaylistIDs.begin(), acceptablePlaylistIDs.end(), playlistID) != acceptablePlaylistIDs.end();
}


void CustomBallOnline::clearWidgetIDs() {
	widgetIDs.clear();
	idsAreStored = false;
	LOG("~~~ cleared stored item IDs ~~~");
}


void CustomBallOnline::activateBasedOnID(ImGuiID id) {
	ImGui::KeepAliveID(id);
	ImGui::ActivateItem(id);
	ImGui::KeepAliveID(id);			// have no clue if this does anything
	LOG("~~ attempted activation on ID {} ~~", id);
}


void CustomBallOnline::unhookFromTickEvent() {
	gameWrapper->UnhookEvent("Function Engine.GameViewportClient.Tick");
	LOG("<<<\tunhooked from game tick event...\t>>>");
}


void CustomBallOnline::resetNavVariables() {
	frameCounter = 0;
	stepCounter = 0;
	delayCounter = 0;
	delayStepAmount = 0;
	delay = false;
	finnaEndDelay = false;
	startSequenceFocusChanged = false;
	firstHighlightedItem == 420;
	highlightedWidgets.clear();
	focusDidntChangeCount = 0;
	startSequenceRetries = 0;
	navStepRetries = 0;

	int savedIDsAmount = widgetIDs.size();
	if (savedIDsAmount != 0 && savedIDsAmount != 3) {
		clearWidgetIDs();
	}
}