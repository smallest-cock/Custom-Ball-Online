#include "pch.h"
#include "CustomBallOnline.hpp"
#include "components/Textures.hpp"

void CustomBallOnline::RenderSettings()
{
	auto enabled_cvar = getCvar(Cvars::enabled);
	if (!enabled_cvar)
		return;

	const float contentHeight = ImGui::GetContentRegionAvail().y - FOOTER_HEIGHT; // available height after accounting for footer
	{
		GUI::ScopedChild c{"PluginSettingsSection", ImVec2(0, contentHeight)};

		GUI::alt_settings_header(h_label.c_str(), plugin_version_display, gameWrapper);

		GUI::Spacing(2);

		bool enabled = enabled_cvar.getBoolValue();
		if (ImGui::Checkbox("Enabled", &enabled))
			enabled_cvar.setValue(enabled);

		if (enabled)
			Textures.display();
	}

	GUI::alt_settings_footer("Need help? Join the Discord", "https://discord.gg/d5ahhQmJbJ");
}
