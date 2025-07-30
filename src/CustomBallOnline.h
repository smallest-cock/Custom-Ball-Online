#pragma once
#include "pch.h"
#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

#include "version.h"

#include "Cvars.hpp"
#include <ModUtils/includes.hpp>


constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
constexpr auto short_plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);
constexpr auto pretty_plugin_version = "v" stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "\t(AlphaConsole version)";


class CustomBallOnline: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase
{
private:
	void onLoad() override;

	// init
	void initCvars();
	void initCommands();
	void initHooks();

private:
	// plugin boilerplate helper stuff
	CVarWrapper registerCvar_Bool(const CvarData& cvar, bool startingValue);
	CVarWrapper registerCvar_String(const CvarData& cvar, const std::string& startingValue);
	CVarWrapper registerCvar_Number(const CvarData& cvar, float startingValue, bool hasMinMax = false, float min = 0, float max = 0);
	CVarWrapper registerCvar_Color(const CvarData& cvar, const std::string& startingValue);
	void registerCommand(const CvarData& cvar, std::function<void(std::vector<std::string>)> callback);
	CVarWrapper getCvar(const CvarData& cvar);

	void hookEvent(const char* funcName, std::function<void(std::string)> callback);
	void hookEventPost(const char* funcName, std::function<void(std::string)> callback);
	void hookWithCaller(const char* funcName, std::function<void(ActorWrapper, void*, std::string)> callback);
	void hookWithCallerPost(const char* funcName, std::function<void(ActorWrapper, void*, std::string)> callback);

	void runCommand(const CvarData& command, float delaySeconds = 0.0f);
	void autoRunCommand(const CvarData& autoRunBool, const CvarData& command, float delaySeconds = 0.0f);
	void runCommandInterval(const CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand = false);
	void autoRunCommandInterval(
		const CvarData& autoRunBool,
		const CvarData& command,
		int numIntervals,
		float delaySeconds,
		bool delayFirstCommand = false);

private:
	// cvar values
	std::shared_ptr<bool> m_enabled = std::make_shared<bool>(true);

	// values
	std::string h_label;

public:
	// gui
	void RenderSettings() override;

	// header/footer stuff
	static constexpr float HEADER_HEIGHT = 80.0f;
	static constexpr float FOOTER_HEIGHT = 40.0f;
};
