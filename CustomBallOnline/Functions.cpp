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



std::vector<int> parseInts(const std::string& input) {
	std::vector<int> playlistIDs = {};

	std::vector<std::string> parsedStingInts = CustomBallOnline::parseWords(input);
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
	DEBUGLOG("{} playlists will automatically enable custom ball texture once joined the match", playlistIDs.size());
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
			gameTickCallback();
		});
		LOG("<<<\thooked into game tick event\t>>>");

	}, startDelay);
}


// ran on every game tick (when game tick event is hooked)
void CustomBallOnline::gameTickCallback() {
	bool navActive = cvarManager->getCvar("autoNavActive").getBoolValue();

	// if there are more navigation steps to perform
	if (navActive) {

		// if delay is active
		if (delay) {
			if (finnaEndDelay) { return; }

			if (delayCounter < 1000) {
				if (checkIfACLoaded()) {
					DEBUGLOG("AC has been loaded and we finna set delay = false after the delay duration...");
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
		// if no delay
		else {
			automateNav();
		}
	}

	// if navigation is finished
	else {
		unhookFromTickEvent();
	}
}


// to be ran when menu navigation is active (no delay)
void CustomBallOnline::automateNav() {
	frameCounter++;

	// on every x amount of frames
	int navDelay = cvarManager->getCvar("autoNavDelay").getIntValue();
	if (frameCounter % navDelay == 0) {

		// debug logging
		DEBUGLOG("------------------------------------------------");		// visual aid to distinguish different steps in console
		ImGuiID currentFocusID = ImGui::GetFocusID();
		DEBUGLOG("[nav stepCounter {}] current focus ID: {}", stepCounter, currentFocusID);
		DEBUGLOG("[nav stepCounter {}] IsAnyItemFocused() : {}", stepCounter, ImGui::IsAnyItemFocused());


		// determine if in start sequence or remaining steps
		if (stepCounter < startSequenceSteps.size()) {
			startSequence();
		}
		else {
			// on the next step after start sequence has completed
			if (stepCounter == startSequenceSteps.size()) {

				// retry start sequence if it was "stuck" the entire time (causing navigation steps to have no effect)
				if (!startSequenceFocusChanged) {
					LOG("*** start sequence steps are done and item focus ID hasnt changed :( ***");
					retryStartSequence();
					return;
				}

				// clear widget IDs if necessary
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
			remainingSteps();
		}
		stepCounter++;
	}
}


// -------  goal: remaining navigation steps should start from the same position every time (no variance)  ---------
// start sequence should leave off with 'Disable Safe Mode' button highlighted/focused (but not clicked) every time
void CustomBallOnline::startSequence() {

	// retry start sequence if no item is highlighted by x step ...
	CVarWrapper startSequenceRetryThresholdCvar = cvarManager->getCvar("startSequenceRetryThreshold");
	if (!startSequenceRetryThresholdCvar) {
		LOG("*** startSequenceRetryThresholdCvar is null!! ***");
		return;
	}
	int startSequenceRetryThreshold = startSequenceRetryThresholdCvar.getIntValue();
	
	if ((stepCounter > (startSequenceRetryThreshold - 1)) && !ImGui::IsAnyItemFocused()) {
		LOG("*** we're past step {} in start sequence and no item is currently focused ***", startSequenceRetryThreshold);
		retryStartSequence();
	}


	// detect if focus has changed (from the 1st non-zero item ID found)
	if (!startSequenceFocusChanged) {
		// ignore the 1st step 
		if (stepCounter > 0) {
			ImGuiID currentFocusedItem = ImGui::GetFocusID();
			if (currentFocusedItem > 0) {

				// save the first highlighted item with a non-zero ID
				if (firstHighlightedItem == 420) {
					firstHighlightedItem = currentFocusedItem;
					LOG("<<< saved the first non-zero ID: {} >>>", currentFocusedItem);
				}

				if (currentFocusedItem != firstHighlightedItem) {
					startSequenceFocusChanged = true;
					LOG("<<< start sequence navigation ID has changed :) >>>");
					DEBUGLOG("<<<\t{}\t!=\t{}\t>>>", currentFocusedItem, firstHighlightedItem);
				}
			}
		}
	}

	// perform step
	if (stepCounter < startSequenceSteps.size()) {
		navInput(startSequenceSteps[stepCounter]);
	}
}


// for remaining steps ...
void CustomBallOnline::remainingSteps() {

	// repeat step if necessary, if no item is highlighted
	if (!ImGui::IsAnyItemFocused()) {

		LOG("*** we're in remaining steps and no item is currently focused ***");

		// get retry limit
		CVarWrapper navStepRetryLimitCvar = cvarManager->getCvar("navStepRetryLimit");
		if (!navStepRetryLimitCvar) {
			LOG("*** navStepRetryLimitCvar is null! ***");
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
			DEBUGLOG("... decremented step counter in order to retry last step");
			LOG("retrying nav step........");
			navStepRetries++;
			return;
		}
		else {
			// maybe do something here if the nav step retries reached the limit ... like exit
			// ... or do nothing, which will just move on to the next step 
		}
	}

	// determine whether to do a fast mode or a regular "blind" mode step
	bool fastModeEnabled = cvarManager->getCvar("enableFastMode").getBoolValue();
	if (fastModeEnabled && idsAreStored) {
		fastMode();
	}
	else {
		blindMode();
	}
}


// regular navigation using "blind" steps
void CustomBallOnline::blindMode() {
	int navStepIndex = stepCounter - startSequenceSteps.size();

	// repeat step if necessary
	if (!focusedItemHasChanged()) {
		if ((navStepIndex - 1) >= 0) {
			std::string prevStep = navigationSteps[navStepIndex - 1];

			if (navStepIndex < navigationSteps.size()) {
				std::string currentStep = navigationSteps[navStepIndex];

				if ((prevStep == "up" || prevStep == "down" || prevStep == "right" || prevStep == "enter") && currentStep != "makeSureLoaded") {
					if (stepRetries < 3) {
						stepRetries++;
						stepCounter--;
						// update navStepIndex based on new stepCount
						navStepIndex--;

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
	}
}


// "smart" menu navigation using IDs to activate items
void CustomBallOnline::fastMode() {
	int widgetIndex = stepCounter - startSequenceSteps.size();

	// repeat last step if necessary (except if last step was 1st widget ID ... bc focus after DSM doesnt change immediately, resulting in false positive)
	if (!focusedItemHasChanged() && widgetIndex > 3) {
		if (stepRetries < 3) {		// 3 retries is currently hardcoded... maybe eventually turn into CVar and add to settings
			stepCounter--;
			widgetIndex--;
			stepRetries++;

			LOG("****\trepeating activation by ID\t****");
		}
		else {
			stepRetries = 0;
		}
	}
	else {
		stepRetries = 0;
	}


	// make sure index is in range
	if (widgetIndex >= 0 && widgetIndex < widgetIDs.size()) {
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
		LOG("<<<\tt'was a fast mode run\t>>>");
	}
}


void CustomBallOnline::navInput(const std::string& keyName) {
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
		// get all windows
		ImVector<ImGuiWindow*> windows = GImGui->Windows;

		// find the active AC window containing the DSM button ... and bring focus to it
		for (ImGuiWindow* window : windows) {
			if (window) {
				std::string winName(window->Name);
				DEBUGLOG("window name is: {}", winName);

				bool isNavFocusable = ImGui::IsWindowNavFocusable(window);
				DEBUGLOG("window nav is focusable: {}", isNavFocusable);

				bool windowActive = window->Active;
				DEBUGLOG("window is active: {}", windowActive);

				// find the AC window that's a child of the main window ... and active
				if ((winName.find("AlphaConsole Plugin/") != std::string::npos) && window->Active) {

					// lil extra tidbit, just because
					ImVec2 winSize = window->Size;
					LOG("AC (child) window size: x-{}, y-{}", winSize[0], winSize[1]);

					// focus the window (and by window im 83% sure its referring to the big rectangular sub-section of the main window containing the "contents")
					ImGui::FocusWindow(window);
					ImGui::BringWindowToFocusFront(window);
				}
			}
			else {
				DEBUGLOG("window ptr is null :(");
			}
		}
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


	DEBUGLOG("[nav stepCounter {}] simulated *** {} ***", stepCounter, keyName);
	LOG("[Step {}] simulated *** {} ***", (stepCounter + 1), keyName);
}



// ---------------------------------------- lil helper functions --------------------------------------------------------------


bool CustomBallOnline::checkIfACLoaded() {
	CVarWrapper acCvar = cvarManager->getCvar("acplugin_wheeltexture_selectedtexture_orange");

	if (!acCvar) {
		return false;
	}

	std::string wheelTexture = acCvar.getStringValue();

	DEBUGLOG("successfully accessed acplugin_wheeltexture_selectedtexture_orange CVar :)");
	DEBUGLOG("... its value is: {}", wheelTexture);

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
		firstHighlightedItem = 420;
		startSequenceFocusChanged = false;
		LOG("retrying start sequence........");
	}
	else {
		// exit run ... bc something fucked up
		LOG("reached the start sequence retry limit of {}", startSequenceRetryLimit);
		LOG(".... exiting because something in the start sequence is fucked and needs fixing");
		cvarManager->getCvar("autoNavActive").setValue(false);
		cvarManager->executeCommand("sleep 200; navInput exit");
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

	bool playlistIsValid = std::find(acceptablePlaylistIDs.begin(), acceptablePlaylistIDs.end(), playlistID) != acceptablePlaylistIDs.end();
	LOG("++ should activate custom ball in this playlist: {}", playlistIsValid);
	return playlistIsValid;
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

	// determine if fast mode is possible
	if (widgetIDs.size() == 3) {
		idsAreStored = true;
	}

	// logging
	LOG(".....................");
	for (int i = 0; i < widgetIDs.size(); i++) {
		LOG("Saved widget ID {}: {}", i + 1, widgetIDs[i]);
	}
	//checkPlaylist();
	DEBUGLOG("+++\tnumber of nav steps that didnt change the highlighted item: {}", focusDidntChangeCount);
}


std::vector<std::string> CustomBallOnline::parseWords(const std::string& input) {
	std::vector<std::string> words;
	std::istringstream iss(input);
	std::string word;
	while (iss >> word) {
		words.push_back(word);
	}
	return words;
}


void CustomBallOnline::resetNavVariables() {
	frameCounter = 0;
	stepCounter = 0;
	delayCounter = 0;
	delayStepAmount = 0;
	delay = false;
	finnaEndDelay = false;
	startSequenceFocusChanged = false;
	firstHighlightedItem = 420;
	highlightedWidgets.clear();
	focusDidntChangeCount = 0;
	startSequenceRetries = 0;
	navStepRetries = 0;

	int savedIDsAmount = widgetIDs.size();
	if (savedIDsAmount != 0 && savedIDsAmount != 3) {
		clearWidgetIDs();
	}
}