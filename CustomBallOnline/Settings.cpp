#include "pch.h"
#include "CustomBallOnline.h"
#include "GuiTools.hpp"


void CustomBallOnline::RenderSettings()
{
    // cvars
    auto enabled_cvar =							GetCvar(Cvars::enabled);
    auto clearUnusedTexturesOnLoading_cvar =	GetCvar(Cvars::clearUnusedTexturesOnLoading);
    if (!enabled_cvar || !clearUnusedTexturesOnLoading_cvar) return;


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
		GUI::SettingsHeader("Header##cbo", pretty_plugin_version, headerSize, false);
    
		bool enabled = enabled_cvar.getBoolValue();
		if (ImGui::Checkbox("Enabled", &enabled))
		{
			enabled_cvar.setValue(enabled);
		}

		if (enabled)
		{ 
			GUI::Spacing(2);

			bool clearUnusedTexturesOnLoading = clearUnusedTexturesOnLoading_cvar.getBoolValue();
			if (ImGui::Checkbox("Clear inactive textures on loading screen", &clearUnusedTexturesOnLoading)) {
				clearUnusedTexturesOnLoading_cvar.setValue(clearUnusedTexturesOnLoading);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Can save memory if you dont switch textures often. Otherwise, leave it unchecked to help reduce lag when switching between textures");
			}

			GUI::Spacing(8);

			if (ImGui::Button("Clear all saved textures"))
			{
				GAME_THREAD_EXECUTE(RUN_COMMAND(Cvars::clearSavedTextures));
			}

			GUI::Spacing(2);

			if (ImGui::Button("Clear unused saved textures"))
			{
				GAME_THREAD_EXECUTE(RUN_COMMAND(Cvars::clearUnusedSavedTextures));
			}

			if (!Textures.savedTextures.empty())
			{
				// can do something here... like set a tooltip only if there are saved textures
			}

			GUI::Spacing(2);

			std::string numSaved = "Saved textures: " + std::to_string(Textures.savedTextures.size());
			ImGui::Text(numSaved.c_str());

			GUI::Spacing(4);

			// list of saved texture names
			if (!Textures.savedTextures.empty())
			{
				if (ImGui::CollapsingHeader("saved", ImGuiTreeNodeFlags_None))
				{
					GUI::Spacing(2);

					for (const auto& [textureName, texMap] : Textures.savedTextures)
					{
						std::string label = "*\t" + textureName;
						ImGui::Text(label.c_str());

						GUI::Spacing();
					}
				}
			}
		}
    }
	ImGui::EndChild();

    GUI::SettingsFooter("Footer##cbo", footerSize, availableSpace.x, false);
}
