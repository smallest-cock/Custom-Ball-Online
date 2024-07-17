#pragma once

#include "pch.h"
#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "Components/Includes.hpp"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
constexpr auto pretty_plugin_version = "v" stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);


namespace CvarNames {
	const std::string acSelectedTexture =						"acplugin_balltexture_selectedtexture";
	const std::string prefix =									"cbo_";
	const std::string enabled = prefix +						"enabled";
	const std::string applyTexture = prefix +					"applyballtexture";
	const std::string clearSavedTextures = prefix +				"clearsavedtextures";
	const std::string clearUnusedSavedTextures = prefix +		"clearunusedsavedtextures";
	const std::string clearUnusedTexturesOnLoading = prefix +	"clearunusedonloading";
	const std::string test = prefix +							"test";
}

namespace Events {
	const std::string ballAdded =				"Function TAGame.GameObserver_TA.HandleBallAdded";
	const std::string ballFadeIn =				"Function TAGame.FXActor_Ball_TA.StartBallFadeIn";
	const std::string replayBegin =				"Function GameEvent_Soccar_TA.ReplayPlayback.BeginState";
	const std::string replayEnd =				"Function GameEvent_Soccar_TA.ReplayPlayback.EndState";
	const std::string replaySkipped =			"Function TAGame.GameInfo_Replay_TA.HandleReplayTimeSkip";
	const std::string teamChanged =				"Function TAGame.GFxData_LocalPlayer_TA.ChangeTeam";
	const std::string botReplaced =				"Function TAGame.GFxHUD_TA.HandleReplaceBot";
	const std::string countdownBegin =			"Function GameEvent_TA.Countdown.BeginState";
	const std::string enterGameplayView =		"Function Engine.PlayerController.EnterStartState";
	const std::string loadingScreen =			"Function ProjectX.EngineShare_X.EventPreLoadMap";
	const std::string mipUpdatedFromPNG =		"Function Engine.Texture2DDynamic.UpdateMipFromPNG";
	const std::string textureInitialized =		"Function Engine.Texture2DDynamic.Init";
	const std::string textureParamValueSet =	"Function Engine.MaterialInstance.SetTextureParameterValue";
}


class CustomBallOnline: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase
{
	//Boilerplate
	void onLoad() override;

	// bools
	bool acHooked = false;

	// hook callbacks
	void OnBallAdded(std::string eventName);
	void OnAllPlayersJoined(std::string eventName);
	void OnHandleReplaySkip(std::string eventName);
	void OnChangeTeam(std::string eventName);
	void OnStartBallFadeIn(std::string eventName);
	void OnReplayBegin(std::string eventName);
	void OnReplayEnd(std::string eventName);
	void OnReplaceBot(std::string eventName);
	void OnCountdownBegin(std::string eventName);
	void OnEnterStartState(std::string eventName);
	void OnLoadingScreen(std::string eventName);
	void OnUpdateMipFromPNG(ActorWrapper caller, void* params, std::string eventName);
	void OnTextureInit(ActorWrapper caller, void* params, std::string eventName);
	void OnSetTexParamValue(ActorWrapper caller, void* params, std::string eventName);

	// hook for 'acplugin_balltexture_selectedtexture' Cvar
	void OnACTexChanged(std::string cvarName, CVarWrapper newCvar);

public:
	void RenderSettings() override;
};
