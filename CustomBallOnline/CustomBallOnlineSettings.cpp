#include "pch.h"
#include "CustomBallOnline.h"

void CustomBallOnline::RenderSettings() {
    // steps
    CVarWrapper navigationStepsCvar = cvarManager->getCvar("navigationSteps");

    // delays
    CVarWrapper navDelayCvar = cvarManager->getCvar("autoNavDelay");
    CVarWrapper startNavDelayCvar = cvarManager->getCvar("startNavDelay");
    CVarWrapper delayDurationCvar = cvarManager->getCvar("delayDuration");

    // commands
    CVarWrapper startCommandCvar = cvarManager->getCvar("startCommand");
    CVarWrapper exitCommandCvar = cvarManager->getCvar("exitCommand");

    // bools
    CVarWrapper runOnMatchStartCvar = cvarManager->getCvar("runOnMatchStart");
    CVarWrapper enableFastModeCvar = cvarManager->getCvar("enableFastMode");



    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Automated menu navigation");

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // automatically run at the start of each match
    bool runOnMatchStart = runOnMatchStartCvar.getBoolValue();
    if (ImGui::Checkbox("Automatically run at the start of each match", &runOnMatchStart)) {
        runOnMatchStartCvar.setValue(runOnMatchStart);
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
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "enableBallTexture");
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

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // navigation steps
    std::string beforeSteps = navigationStepsCvar.getStringValue();
    ImGui::InputTextWithHint("menu navigation steps", "navigation words: enter, back, up, down, left, right, delay, exit", &beforeSteps);
    navigationStepsCvar.setValue(beforeSteps);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("starts after the 'Disable Safe Mode' button is highlighted");
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // start delay
    float startDelay = startNavDelayCvar.getFloatValue();
    ImGui::SliderFloat("start delay", &startDelay, 0.0f, 5.0f, "%.1f seconds");
    startNavDelayCvar.setValue(startDelay);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("gives bakkesmod time to reload AlphaConsole");
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // navigation step delay
    int delay = navDelayCvar.getIntValue();
    ImGui::SliderInt("navigation delay (lower is faster)", &delay, 2, 100, "%.0f frames");      // maybe make slider logarithmic .... dont think ImGuiSliderFlags_Logarithmic exists in v1.75 :(
    navDelayCvar.setValue(delay);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("menu navigation speed depends on your game fps (higher fps = faster navigation) ... super low delay may cause errors");
    }

    ImGui::Spacing();
    ImGui::Spacing();
    
    // delay duration
    float delayDuration = delayDurationCvar.getFloatValue();
    ImGui::SliderFloat("duration of a \"delay\" step", &delayDuration, 0.0f, 5.0f, "%.1f seconds");
    delayDurationCvar.setValue(delayDuration);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("gives AlphaConsole time to load all its modules");
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
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
    ImGui::Spacing();
    ImGui::Spacing();


    if (ImGui::Button("clear saved IDs")) {
        clearWidgetIDs();
    }
}