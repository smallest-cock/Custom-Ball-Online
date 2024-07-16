#include "pch.h"
#include "CustomBallOnline.h"


void CustomBallOnline::RenderSettings()
{
    // Cvars
    auto enabledCvar = cvarManager->getCvar(CvarNames::enabled);
    auto clearUnusedOnLoadingCvar = cvarManager->getCvar(CvarNames::clearUnusedTexturesOnLoading);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 0, 1, 1), "Plugin made by SSLow");

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text(pretty_plugin_version);
    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "If you have questions/issues, feel free to ask in the server ");
    ImGui::SameLine();
    ImGui::PushItemWidth(150);
    std::string txtToBeCopied = "discord.gg/tHZFsMsvDU";
    ImGui::InputText("", &txtToBeCopied, ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


    bool enabled = enabledCvar.getBoolValue();
    if (ImGui::Checkbox("Enabled", &enabled)) {
        enabledCvar.setValue(enabled);
    }

    if (!enabled) return;   // dont render code below if plugin disabled

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    bool clearUnusedOnLoading = clearUnusedOnLoadingCvar.getBoolValue();
    if (ImGui::Checkbox("clear inactive textures on loading screen", &clearUnusedOnLoading)) {
        clearUnusedOnLoadingCvar.setValue(clearUnusedOnLoading);
    }
    if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("can save memory, but may cause a stutter when switching textures in-game");
	}

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("clear all saved textures")) {
        gameWrapper->Execute([this](GameWrapper* gw) {
            cvarManager->executeCommand(CvarNames::clearSavedTextures);
        });
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("clear unused saved textures")) {
        gameWrapper->Execute([this](GameWrapper* gw) {
            cvarManager->executeCommand(CvarNames::clearUnusedSavedTextures);
        });
    }
	if (!Textures.savedTextures.empty()) {
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("can save memory if you're not using every single saved texture");
		}
	}

    ImGui::Spacing();
    ImGui::Spacing();

    std::string numSaved = "saved textures: " + std::to_string(Textures.savedTextures.size());
    ImGui::Text(numSaved.c_str());

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // list of saved texture names
    if (!Textures.savedTextures.empty()) {
        if (ImGui::CollapsingHeader("saved", ImGuiTreeNodeFlags_None))
        {
            ImGui::Spacing();
            ImGui::Spacing();

			for (const auto& [textureName, texMap] : Textures.savedTextures) {
				std::string label = "*\t" + textureName;
				ImGui::Text(label.c_str());

				ImGui::Spacing();
			}
        }
    }
}