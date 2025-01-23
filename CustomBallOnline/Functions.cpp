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

	fs::path assets[] =
	{
		plugin_assets_folder / "discord.png",
		plugin_assets_folder / "github.png",
		plugin_assets_folder / "youtube.png",
	};

	bool asset_missing = false;
	for (const auto& asset : assets)
	{
		if (!fs::exists(asset))
		{
			LOG("[ERROR] File not found: {}", asset.string());
			asset_missing = true;
		}
	}

	if (asset_missing)
	{
		LOG("One or more plugin asset is missing... will use old ugly settings footer instead :(");
		assets_exist = false;
	}
	else
	{
		assets_exist = true;

		footer_links =
		{
			GUI::ImageLink(std::make_shared<ImageWrapper>(assets[0]),	GUI::discord_link,	GUI::discord_desc),
			GUI::ImageLink(std::make_shared<ImageWrapper>(assets[1]),	github_link,		github_link_tooltip),
			GUI::ImageLink(std::make_shared<ImageWrapper>(assets[2]),	GUI::youtube_link,	GUI::youtube_desc)
		};

		footer_links.set_height(footer_img_height);
	}
}