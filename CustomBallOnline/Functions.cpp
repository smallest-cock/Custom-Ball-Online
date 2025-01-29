#include "pch.h"
#include "CustomBallOnline.h"



void CustomBallOnline::RunCommand(const Cvars::CvarData& command, float delaySeconds)
{
	if (delaySeconds == 0)
	{
		cvarManager->executeCommand(command.name);
	}
	else if (delaySeconds > 0)
	{
		gameWrapper->SetTimeout([this, command](GameWrapper* gw) {
			cvarManager->executeCommand(command.name);
		}, delaySeconds);
	}
}


void CustomBallOnline::RunCommandInterval(const Cvars::CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand)
{
	if (!delayFirstCommand)
	{
		RunCommand(command);
		numIntervals--;
	}

	for (int i = 1; i <= numIntervals; i++)
	{
		RunCommand(command, delaySeconds * i);
	}
}


void CustomBallOnline::AutoRunCommand(const Cvars::CvarData& autoRunBool, const Cvars::CvarData& command, float delaySeconds)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue()) return;

	RunCommand(command, delaySeconds);
}


void CustomBallOnline::AutoRunCommandInterval(const Cvars::CvarData& autoRunBool, const Cvars::CvarData& command,
	int numIntervals, float delaySeconds, bool delayFirstCommand)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue()) return;

	RunCommandInterval(command, numIntervals, delaySeconds, delayFirstCommand);
}


bool CustomBallOnline::PluginEnabled()
{
	auto enabled_cvar = GetCvar(Cvars::enabled);
	return enabled_cvar && enabled_cvar.getBoolValue();
}


void CustomBallOnline::gui_footer_init()
{
	fs::path plugin_assets_folder = gameWrapper->GetDataFolder() / "sslow_plugin_assets";
	if (!fs::exists(plugin_assets_folder))
	{
		LOG("[ERROR] Folder not found: {}", plugin_assets_folder.string());
		LOG("Will use old ugly settings footer :(");
		return;
	}

	GUI::FooterAssets assets = {
		plugin_assets_folder / "github.png",
		plugin_assets_folder / "discord.png",
		plugin_assets_folder / "youtube.png",
	};

	assets_exist = assets.all_assets_exist();

	if (assets_exist)
	{
		footer_links = std::make_shared<GUI::FooterLinks>(
			GUI::ImageLink(assets.github_img_path, github_link, github_link_tooltip, footer_img_height),
			GUI::ImageLink(assets.discord_img_path, GUI::discord_link, GUI::discord_desc, footer_img_height),
			GUI::ImageLink(assets.youtube_img_path, GUI::youtube_link, GUI::youtube_desc, footer_img_height)
		);
	}
	else
	{
		LOG("One or more plugin asset is missing... will use old ugly settings footer instead :(");
	}
}