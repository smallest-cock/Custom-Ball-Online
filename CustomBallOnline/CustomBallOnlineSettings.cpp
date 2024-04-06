#include "pch.h"
#include "CustomBallOnline.h"


void ClickableLink(const char* label, const char* url) {
    for (int i = 0; i < 12; i++) {
		ImGui::Indent();
    }
    if (ImGui::Selectable(label, false, ImGuiSelectableFlags_NoHoldingActiveID, ImVec2(250,15))) {
        ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
    }
    for (int i = 0; i < 12; i++) {
		ImGui::Unindent();
    }
}


void CustomBallOnline::RenderSettings() {
    // steps
    CVarWrapper navigationStepsCvar = cvarManager->getCvar("navigationSteps");
    CVarWrapper startSequenceStepsCvar = cvarManager->getCvar("startSequenceSteps");

    // playlists
    CVarWrapper automaticActivationPlaylistsCvar = cvarManager->getCvar("automaticActivationPlaylists");

    // delays
    CVarWrapper navDelayCvar = cvarManager->getCvar("autoNavDelay");
    CVarWrapper startNavDelayCvar = cvarManager->getCvar("startNavDelay");
    CVarWrapper delayDurationCvar = cvarManager->getCvar("delayDuration");
    CVarWrapper delayAfterJoinMatchCvar = cvarManager->getCvar("delayAfterJoinMatch");

    // commands
    CVarWrapper startCommandCvar = cvarManager->getCvar("startCommand");
    CVarWrapper exitCommandCvar = cvarManager->getCvar("exitCommand");

    // retry CVars
    CVarWrapper startSequenceRetryLimitCvar = cvarManager->getCvar("startSequenceRetryLimit");
    CVarWrapper startSequenceRetryThresholdCvar = cvarManager->getCvar("startSequenceRetryThreshold");
    CVarWrapper navStepRetryLimitCvar = cvarManager->getCvar("navStepRetryLimit");

    // bools
    CVarWrapper runOnMatchStartCvar = cvarManager->getCvar("runOnMatchStart");
    CVarWrapper enableFastModeCvar = cvarManager->getCvar("enableFastMode");


    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    

    // automatically run at the start of each match
    bool runOnMatchStart = runOnMatchStartCvar.getBoolValue();
    if (ImGui::Checkbox("Automatically run after joining a match", &runOnMatchStart)) {
        runOnMatchStartCvar.setValue(runOnMatchStart);
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (runOnMatchStart) {
		// delay after OnAllTeamsCreated event .... aka when to automatically run enableBallTexture command
		float delayAfterJoinMatch = delayAfterJoinMatchCvar.getFloatValue();
		ImGui::SliderFloat("automatic start delay\t(after joining a match)", &delayAfterJoinMatch, 0.0f, 5.0f, "%.1f seconds");
		delayAfterJoinMatchCvar.setValue(delayAfterJoinMatch);

        ImGui::Spacing();
        ImGui::Spacing();

        // playlist IDs to automatically run at start of match
        std::string automaticActivationPlaylists = automaticActivationPlaylistsCvar.getStringValue();
        ImGui::InputTextWithHint("playlist IDs\t(game modes to use custom ball texture)", "1 2 6 13 34 ...", &automaticActivationPlaylists);
        automaticActivationPlaylistsCvar.setValue(automaticActivationPlaylists);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("these playlists should use the default RL soccar ball\t\t**text field must only contain numbers and spaces**");
        }
        ClickableLink("\t(click to see list of available playlist IDs)", "https://wiki.bakkesplugins.com/code_snippets/playlist_id/");
    }


    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


    ImGui::Text("\t\t\t\t\t\t\t\tor");

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("bind the  ");
    ImGui::SameLine();
    //ImGui::TextColored(ImVec4(1, 1, 0, 1), "enableBallTexture");
    ImGui::PushItemWidth(110);
    std::string commandToBeCopied = "enableBallTexture";
    ImGui::InputText("", &commandToBeCopied, ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Text("  command");


    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


    // enable fast mode
    bool enableFastMode = enableFastModeCvar.getBoolValue();
    if (ImGui::Checkbox("Attempt fast navigation when possible", &enableFastMode)) {
        enableFastModeCvar.setValue(enableFastMode);
        if (!enableFastMode) {
			clearWidgetIDs();
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("searches for menu items directly instead of using 'blind' navigation steps (can be more reliable & faster)");
    }


    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    

    // Delay durations
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Delay durations");

    ImGui::Spacing();
    ImGui::Spacing();

    // start delay
    float startDelay = startNavDelayCvar.getFloatValue();
    ImGui::SliderFloat("start navigation delay", &startDelay, 0.0f, 5.0f, "%.1f seconds");
    startNavDelayCvar.setValue(startDelay);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("gives bakkesmod time to reload AlphaConsole before attempting navigation steps");
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // navigation step delay
    int delay = navDelayCvar.getIntValue();
    ImGui::SliderInt("navigation delay\t(lower is faster)", &delay, 2, 100, "%.0f frames");      // ImGuiSliderFlags_Logarithmic exists in v1.75 :(
    navDelayCvar.setValue(delay);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("menu navigation speed ... depends on your game fps (higher fps = faster navigation)");
    }

    ImGui::Spacing();
    ImGui::Spacing();
    
    // delay duration
    float delayDuration = delayDurationCvar.getFloatValue();
    ImGui::SliderFloat("additional delay after a \"makeSureLoaded\" step", &delayDuration, 0.0f, 3.0f, "%.1f seconds");
    delayDurationCvar.setValue(delayDuration);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("gives some extra time to make sure everything is loaded before resuming navigation");
    }


    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


    // Menu navigation steps
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Menu navigation steps");

    ImGui::Spacing();
    ImGui::Spacing();

    // start sequence steps
    std::string startSequenceSteps = startSequenceStepsCvar.getStringValue();
    ImGui::InputTextWithHint("start sequence\t(steps to highlight 'Disable Safe Mode')", "navigation words: enter, back, up, down, left, right, focus, makeSureLoaded, exit", &startSequenceSteps);
    startSequenceStepsCvar.setValue(startSequenceSteps);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("should end with the 'Disable Safe Mode' button highlighted (but not pressed)");
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // remaining navigation steps
    std::string remainingSteps = navigationStepsCvar.getStringValue();
    ImGui::InputTextWithHint("remaining steps", "navigation words: enter, back, up, down, left, right, focus, makeSureLoaded, exit", &remainingSteps);
    navigationStepsCvar.setValue(remainingSteps);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("navigation steps to perform after the 'Disable Safe Mode' button has been highlighted");
    }

    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


    // Commands
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Commands");

    ImGui::Spacing();
    ImGui::Spacing();

    // start command
    std::string startCommand = startCommandCvar.getStringValue();
    ImGui::InputTextWithHint("start command", "start command", &startCommand);
    startCommandCvar.setValue(startCommand);

    ImGui::Spacing();
    ImGui::Spacing();

    // exit command
    std::string exitCommand = exitCommandCvar.getStringValue();
    ImGui::InputTextWithHint("exit command", "exit command", &exitCommand);
    exitCommandCvar.setValue(exitCommand);


    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


    // Commands
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Retries");
    if (ImGui::IsItemHovered()) {
        std::string retryLimitTooltip = "if menu navigation isn't working properly";
        ImGui::SetTooltip(retryLimitTooltip.c_str());
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // start sequence retry cutoff step
    int startSequenceRetryThreshold = startSequenceRetryThresholdCvar.getIntValue();
    ImGui::SliderInt("retry start sequence after this step\t(if nothing is highlighted)", &startSequenceRetryThreshold, 1, 10, "step %.0f");
    startSequenceRetryThresholdCvar.setValue(startSequenceRetryThreshold);

    ImGui::Spacing();
    ImGui::Spacing();

    // start sequence retry limit
    int startSequenceRetryLimit = startSequenceRetryLimitCvar.getIntValue();
    ImGui::SliderInt("start sequence retry limit", &startSequenceRetryLimit, 0, 10, "%.0f retries");
    startSequenceRetryLimitCvar.setValue(startSequenceRetryLimit);
    if (ImGui::IsItemHovered()) {
        std::string retryLimitTooltip = "max # of times to retry the start sequence";
        ImGui::SetTooltip(retryLimitTooltip.c_str());
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // navigation step retry limit
    int navStepRetryLimit = navStepRetryLimitCvar.getIntValue();
    ImGui::SliderInt("navigation step retry limit", &navStepRetryLimit, 0, 50, "%.0f retries");
    navStepRetryLimitCvar.setValue(navStepRetryLimit);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("max # of times to retry a navigation step (in remaining steps), if no item is highlighted after it's performed");
    }


    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


    if (ImGui::Button("clear saved menu item IDs")) {
        clearWidgetIDs();
    }
}