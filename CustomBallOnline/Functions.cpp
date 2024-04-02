#include "pch.h"
#include "CustomBallOnline.h"



std::vector<std::string> CustomBallOnline::navigationSteps;
std::vector<std::string> CustomBallOnline::startSequenceSteps;
std::vector<ImGuiID> CustomBallOnline::widgetIDs;
std::vector<ImGuiID> CustomBallOnline::highlightedWidgets;
int CustomBallOnline::frameCounter = 0;
int CustomBallOnline::stepCounter = 0;
bool CustomBallOnline::delay = false;
bool CustomBallOnline::finnaEndDelay = false;
bool CustomBallOnline::playlistFound = false;
int CustomBallOnline::delayCounter = 0;
int CustomBallOnline::delayStepAmount = 0;
int CustomBallOnline::focusDidntChangeCount = 0;
bool CustomBallOnline::idsAreStored = false;
int CustomBallOnline::activatedWidgetCount = 0;
int CustomBallOnline::stepRetries = 0;



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

	// convert calculated value to int.. and add 1 to make sure there's at least 1 nav step worth of delay
	delayStepAmount = (int)((gameFramerate * delayDuration) / navDelay) + 1;		// let delay duration (seconds) be X .... X = delayAmount * (navDelay / gameFramerate)
																					//										  X * gameFramerate = delayAmount * navDelay
																					//									      (X * gameFramerate) / navDelay = delayAmount

	gameWrapper->Execute([startCommand, this](GameWrapper* gw) {
		cvarManager->executeCommand(startCommand);
		cvarManager->executeCommand("startNav");
	});
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

		gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", [this](std::string eventName) {
			everyGameTick();
		});

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
				gameWrapper->UnhookEvent("Function Engine.GameViewportClient.Tick");
				LOG("unhooked from game tick event...");
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
		gameWrapper->UnhookEvent("Function Engine.GameViewportClient.Tick");
		LOG("unhooked from game tick event...");

		// determine if fast mode is possible
		if (widgetIDs.size() == 3) {
			idsAreStored = true;
		}

		// logging
		for (int i = 0; i < widgetIDs.size(); i++) {
			LOG("widget id {}: {}", i + 1, widgetIDs[i]);
		}

		checkPlaylist();
		LOG("+++\tnumber of nav steps that didnt change the highlighted item: {}", focusDidntChangeCount);
	}
}


// goal: make it so remaining navigation steps start from the same position every time (no variance)
// should leave off with DSM button highlighted/focused (but not clicked) every time

// TODO: check if any item has focus with ImGui::IsAnyItemFocused() at some place to ensure things have been highlighted with nav
//		 ... if by the end no item has focus, then the start sequence definitely didnt take effect and will fail (no need to wait and probe AC CVars)
void CustomBallOnline::startSequence() {

	// start sequence steps
	if (stepCounter < startSequenceSteps.size()) {
		navInput(startSequenceSteps[stepCounter]);
	}
	// after start sequence nav steps have completed... DSM should be highlighted/focused at this point (but not pressed)
	else if (stepCounter == startSequenceSteps.size()) {
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

						gameWrapper->SetTimeout([this](...) {
							LOG("****\trepeated activation by ID\t****");
						}, 3);
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

									gameWrapper->SetTimeout([prevStep, this](...) {
										LOG("****\trepeated [{}] step\t****", prevStep);
									}, 7);
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
					gameWrapper->Execute([this](GameWrapper* gw) {
						cvarManager->getCvar("autoNavActive").setValue(false);
					});
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
	else if (keyName == "back") {
		io.NavInputs[ImGuiNavInput_Cancel] = 1.0f;
	}
	else if (keyName == "alt") {
		io.NavInputs[ImGuiNavInput_KeyMenu_] = 1.0f;
	}
	else if (keyName == "focus") {
		ImGui::SetWindowFocus("AlphaConsole Plugin");
	}
	else if (keyName == "makeSureLoaded") {
		delay = true;
	}
	else if (keyName == "exit") {
		gameWrapper->Execute([this](GameWrapper* gw) {
			std::string exitCommand = cvarManager->getCvar("exitCommand").getStringValue();
			cvarManager->executeCommand(exitCommand);
		});
	}

	//LOG("IsAnyItemFocused() : {}", ImGui::IsAnyItemFocused());
	LOG("[nav step {}] simulated *** {} ***", stepCounter, keyName);
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
	ImGui::KeepAliveID(id);			// putting in work (i think)
	LOG("~~ attempted activation on ID {} ~~", id);
}


void CustomBallOnline::resetNavVariables() {
	frameCounter = 0;
	stepCounter = 0;
	delayCounter = 0;
	delayStepAmount = 0;
	delay = false;
	finnaEndDelay = false;
	highlightedWidgets.clear();
	focusDidntChangeCount = 0;

	int savedIDsAmount = widgetIDs.size();
	if (savedIDsAmount != 0 && savedIDsAmount != 3) {
		clearWidgetIDs();
	}
}