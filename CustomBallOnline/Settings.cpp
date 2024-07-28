#include "pch.h"
#include "CustomBallOnline.h"
#include "GuiUtils.hpp"


void CustomBallOnline::RenderSettings()
{
    // cvars
    auto enabledCvar = cvarManager->getCvar(CvarNames::enabled);
    auto clearUnusedOnLoadingCvar = cvarManager->getCvar(CvarNames::clearUnusedTexturesOnLoading);
    if (!enabledCvar || !clearUnusedOnLoadingCvar) return;


	// ---------------- calculate ImGui::BeginChild sizes ------------------

	ImVec2 availableSpace = ImGui::GetContentRegionAvail();
	availableSpace.y -= 4;		// act as if availableSpace height is 4px smaller, bc for some reason availableSpace height is cap (prevents scroll bars)
	float headerHeight = 80.0f;
	float footerHeight = 35.0f;
	float contentHeight = availableSpace.y - footerHeight;

	ImVec2 contentSize = ImVec2(0, contentHeight);
	ImVec2 footerSize = ImVec2(0, footerHeight);
	ImVec2 headerSize = ImVec2(0, headerHeight);

	// ----------------------------------------------------------------------


    if (ImGui::BeginChild("Content##cbo", contentSize))
    {
		GUI::SettingsHeader(headerSize, false);
    
		bool enabled = enabledCvar.getBoolValue();
		if (ImGui::Checkbox("Enabled", &enabled)) {
			enabledCvar.setValue(enabled);
		}

		if (enabled)
		{ 
			GUI::Spacing(2);

			bool clearUnusedOnLoading = clearUnusedOnLoadingCvar.getBoolValue();
			if (ImGui::Checkbox("Clear inactive textures on loading screen", &clearUnusedOnLoading)) {
				clearUnusedOnLoadingCvar.setValue(clearUnusedOnLoading);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("can save memory");
			}

			GUI::Spacing(8);

			if (ImGui::Button("Clear all saved textures")) {
				gameWrapper->Execute([this](GameWrapper* gw) {
					cvarManager->executeCommand(CvarNames::clearSavedTextures);
					});
			}

			GUI::Spacing(2);

			if (ImGui::Button("Clear unused saved textures")) {
				gameWrapper->Execute([this](GameWrapper* gw) {
					cvarManager->executeCommand(CvarNames::clearUnusedSavedTextures);
					});
			}
			if (!Textures.savedTextures.empty()) {
				// can do something here... like set a tooltip only if there are saved textures
			}

			GUI::Spacing(2);

			std::string numSaved = "Saved textures: " + std::to_string(Textures.savedTextures.size());
			ImGui::Text(numSaved.c_str());

			GUI::Spacing(4);

			// list of saved texture names
			if (!Textures.savedTextures.empty()) {
				if (ImGui::CollapsingHeader("saved", ImGuiTreeNodeFlags_None))
				{
					GUI::Spacing(2);

					for (const auto& [textureName, texMap] : Textures.savedTextures) {
						std::string label = "*\t" + textureName;
						ImGui::Text(label.c_str());

						ImGui::Spacing();
					}
				}
			}
		}
    }
	ImGui::EndChild();

    GUI::SettingsFooter(footerSize, availableSpace.x, false);
}
