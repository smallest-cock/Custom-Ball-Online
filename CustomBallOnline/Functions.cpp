#include "pch.h"
#include "CustomBallOnline.h"


std::vector<std::string> CustomBallOnline::navigationSteps;
int CustomBallOnline::frameCounter = 0;
int CustomBallOnline::stepCounter = 0;
bool CustomBallOnline::delay = false;
int CustomBallOnline::delayCounter = 0;
int CustomBallOnline::delayStepAmount = 0;


std::vector<std::string> parseWords(const std::string& input) {
	std::vector<std::string> words;
	std::istringstream iss(input);
	std::string word;
	while (iss >> word) {
		words.push_back(word);
	}
	return words;
}



	// TODO: find convenient fix for when auto nav fucks up causing alphaconsole menu widgets to be misaligned
	//		 perhaps see if there's a way to activate imgui widgets by name/id/etc instead of using blind "up up up down" nav steps
	//		 which are very error prone (one fucked run can fuck up subsequent runs)



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
	}
}


// to be ran on each UI frame during auto nav
void CustomBallOnline::frameRenderCallback() {
	frameCounter++;

	// on every x amount of frames
	int navDelay = cvarManager->getCvar("autoNavDelay").getIntValue();

	if (frameCounter % navDelay == 0) {

		if (!delay) {
			// if there are more nav steps
			if (stepCounter < navigationSteps.size()) {
				std::string currentStep = navigationSteps[stepCounter];


				if (currentStep != "delay") {
					gameWrapper->Execute([currentStep, this](GameWrapper* gw) {
						navInput(currentStep);
						});
				}
				else {
					delay = true;
				}
				
				stepCounter++;
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
		io.NavInputs[ImGuiNavInput_Activate] = 1.0f;
	}
	else if (keyName == "back") {
		io.NavInputs[ImGuiNavInput_Cancel] = 1.0f;
	}
	else if (keyName == "start") {
		ImGui::SetWindowFocus("AlphaConsole Plugin");
		io.NavInputs[ImGuiNavInput_DpadDown] = 1.0f;
	}
	else if (keyName == "exit") {
		gameWrapper->Execute([this](GameWrapper* gw) {
			std::string exitCommand = cvarManager->getCvar("exitCommand").getStringValue();
			cvarManager->executeCommand(exitCommand);
			});
	}

	LOG("[nav step {}] simulated *** {} ***", stepCounter, keyName);
}


void CustomBallOnline::resetNavVariables() {
	frameCounter = 0;
	stepCounter = 0;
	delayCounter = 0;
	delay = false;
}