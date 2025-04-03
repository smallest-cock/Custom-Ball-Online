#include "pch.h"
#include "CustomBallOnline.h"


void CustomBallOnline::RenderSettings()
{
    // cvars
    auto enabled_cvar =							GetCvar(Cvars::enabled);
    auto clearUnusedTexturesOnLoading_cvar =	GetCvar(Cvars::clearUnusedTexturesOnLoading);
    if (!enabled_cvar || !clearUnusedTexturesOnLoading_cvar) return;

	const float content_height = ImGui::GetContentRegionAvail().y - footer_height;	// available height after accounting for footer

	if (ImGui::BeginChild("PluginSettingsSection", ImVec2(0, content_height)))
    {
		GUI::alt_settings_header(h_label.c_str(), pretty_plugin_version);

		bool enabled = enabled_cvar.getBoolValue();
		if (ImGui::Checkbox("Enabled", &enabled))
		{
			enabled_cvar.setValue(enabled);
		}

		if (enabled)
		{ 
			GUI::Spacing(2);

			bool clearUnusedTexturesOnLoading = clearUnusedTexturesOnLoading_cvar.getBoolValue();
			if (ImGui::Checkbox("Clear inactive textures on loading screen", &clearUnusedTexturesOnLoading))
			{
				clearUnusedTexturesOnLoading_cvar.setValue(clearUnusedTexturesOnLoading);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Can save memory if you dont switch textures often. Otherwise, leave it unchecked to help reduce lag when switching between textures");
			}

			GUI::Spacing(8);

			if (ImGui::Button("Clear all saved textures"))
			{
				GAME_THREAD_EXECUTE(
					RunCommand(Cvars::clearSavedTextures);
				);
			}

			GUI::Spacing(2);

			if (ImGui::Button("Clear unused saved textures"))
			{
				GAME_THREAD_EXECUTE(
					RunCommand(Cvars::clearUnusedSavedTextures);
				);
			}

			GUI::Spacing(4);

			uint8_t num_saved_texture_sets = 0;

			// list of saved texture info
			if (!Textures.saved_texture_data.empty())
			{
				if (ImGui::CollapsingHeader("saved texture data"))
				{
					ImGui::Indent();
					GUI::Spacing(2);

					for (const auto& [textureName, tex_data] : Textures.saved_texture_data)
					{
						if (!tex_data.textures.empty()) num_saved_texture_sets++;

						if (ImGui::CollapsingHeader(textureName.c_str()))
						{
							ImGui::Indent(50);

							for (const auto& [param_name, img_path] : tex_data.img_paths)
							{
								std::string txt = param_name + ":\t" + img_path.string();
								ImGui::Text(txt.c_str());

								GUI::SameLineSpacing_relative(20);

								ImGui::PushID(img_path.c_str());

								if (ImGui::Button("Open"))
								{
									Files::OpenFolder(img_path.parent_path());
								}

								ImGui::PopID();
							}
							
							ImGui::Unindent(50);
						}

						ImGui::Spacing();
					}

					GUI::Spacing(2);

					ImGui::Text("Cached ball textures: %d", num_saved_texture_sets);

					ImGui::Unindent();
				}
			}

			GUI::Spacing(2);

		}
    }
	ImGui::EndChild();

	GUI::alt_settings_footer("Need help? Join the Discord", "https://discord.gg/d5ahhQmJbJ");
}
