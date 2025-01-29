#pragma once
#include "pch.h"
#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "Macros.hpp"
#include "Events.hpp"
#include "Cvars.hpp"
#include "GuiTools.hpp"
#include "Components/Includes.hpp"


constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
constexpr auto pretty_plugin_version = "v" stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);


class CustomBallOnline: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase
{
	//Boilerplate
	void onLoad() override;


	// cvar helpers
	CVarWrapper RegisterCvar_Bool(const Cvars::CvarData& cvar, bool startingValue);
	CVarWrapper RegisterCvar_String(const Cvars::CvarData& cvar, const std::string& startingValue);
	CVarWrapper RegisterCvar_Number(const Cvars::CvarData& cvar, float startingValue, bool hasMinMax = false, float min = 0, float max = 0);
	CVarWrapper RegisterCvar_Color(const Cvars::CvarData& cvar, const std::string& startingValue);
	void RegisterCommand(const Cvars::CvarData& cvar, std::function<void(std::vector<std::string>)> callback);
	CVarWrapper GetCvar(const Cvars::CvarData& cvar);


	// lil command helpers
	void RunCommand(const Cvars::CvarData& command, float delaySeconds = 0);
	void AutoRunCommand(const Cvars::CvarData& autoRunBool, const Cvars::CvarData& command, float delaySeconds = 0);

	void RunCommandInterval(const Cvars::CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand = false);
	void AutoRunCommandInterval(
		const Cvars::CvarData& autoRunBool,
		const Cvars::CvarData& command,
		int numIntervals,
		float delaySeconds,
		bool delayFirstCommand = false);

	bool PluginEnabled();


	// commands
	void cmd_applyTexture(std::vector<std::string> args);
	void cmd_clearSavedTextures(std::vector<std::string> args);
	void cmd_clearUnusedSavedTextures(std::vector<std::string> args);
	void cmd_test(std::vector<std::string> args);


	////cvar changed callbacks
	//void changed_acSelectedTexture(std::string cvarName, CVarWrapper newCvar);


	// hook callbacks
	void Event_BallAdded(std::string eventName);
	void Event_AllPlayersJoined(std::string eventName);
	void Event_ReplaySkipped(std::string eventName);
	void Event_ChangeTeam(std::string eventName);
	void Event_StartBallFadeIn(std::string eventName);
	void Event_ReplayBegin(std::string eventName);
	void Event_ReplayEnd(std::string eventName);
	void Event_ReplaceBot(std::string eventName);
	void Event_CountdownBegin(std::string eventName);
	void Event_EnterStartState(std::string eventName);
	void Event_LoadingScreenStart(std::string eventName);
	void Event_LoadingScreenEnd(std::string eventName);
	void Event_ReplaySkipToFrame(std::string eventName);
	void Event_ReplicatedGoalScored(std::string eventName);
	void Event_CasualIntermission(std::string eventName);
	void Event_SetTextureParamValue_MI(ActorWrapper caller, void* params, std::string eventName);
	void Event_SetTextureParamValue_MIC(ActorWrapper caller, void* params, std::string eventName);
	void Event_SetPausedForEndOfReplay(ActorWrapper caller, void* params, std::string eventName);

public:
	// GUI
	void RenderSettings() override;


	// header/footer stuff
	void gui_footer_init();
	bool assets_exist = false;
	std::shared_ptr<GUI::FooterLinks> footer_links;

	static constexpr float header_height =					80.0f;
	static constexpr float footer_height =					40.0f;
	static constexpr float footer_img_height =				25.0f;

	static constexpr const wchar_t* github_link =			L"https://github.com/smallest-cock/Custom-Ball-Online";
	static constexpr const char* github_link_tooltip =		"GitHub page";
};
