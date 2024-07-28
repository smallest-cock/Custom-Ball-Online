#pragma once

#include "pch.h"
#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "Events.hpp"
#include "CvarNames.hpp"
#include "Components/Includes.hpp"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
constexpr auto pretty_plugin_version = "v" stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);


class CustomBallOnline: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase
{
	//Boilerplate
	void onLoad() override;

	// bools
	bool acHooked = false;

	
	// commands
	void applyTexture(std::vector<std::string> args);
	void clearSavedTextures(std::vector<std::string> args);
	void clearUnusedSavedTextures(std::vector<std::string> args);
	void test(std::vector<std::string> args);


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
	void Event_UpdateMipFromPNG(ActorWrapper caller, void* params, std::string eventName);
	void Event_TextureInit(ActorWrapper caller, void* params, std::string eventName);
	void Event_SetTextureParamValue(ActorWrapper caller, void* params, std::string eventName);

	// callback for 'acplugin_balltexture_selectedtexture' cvar change
	void OnACBallTextureChanged(std::string cvarName, CVarWrapper newCvar);

public:
	// GUI
	void RenderSettings() override;
};
