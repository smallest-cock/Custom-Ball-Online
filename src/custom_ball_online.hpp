#pragma once
#include "pch.h"
#include "boilerplate/gui_base.hpp"
#include "boilerplate/plugin_helper_base.hpp"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "version.hpp"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(
    VERSION_BUILD);

constexpr auto plugin_version_display = "v" VERSION_STR;

class CustomBallOnline : public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase, public PluginHelperBase {
	auto onLoad() -> void override;
	auto getPlugin() -> BakkesMod::Plugin::BakkesModPlugin * override;

private:
	void initCvars();
	void initCommands();
	// void initHooks();

private:
	std::string h_label;

	// cvar values
	std::shared_ptr<bool> m_enabled = std::make_shared<bool>(true);

	// gui
private:
	static constexpr float FOOTER_HEIGHT = 40.0f;

public:
	void RenderSettings() override;
};
