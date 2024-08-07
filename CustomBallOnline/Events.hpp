#pragma once
#include <string>


namespace Events
{
	const std::string BallAdded =					"Function TAGame.GameObserver_TA.HandleBallAdded";
	const std::string StartBallFadeIn =				"Function TAGame.FXActor_Ball_TA.StartBallFadeIn";
	const std::string ReplayBegin =					"Function GameEvent_Soccar_TA.ReplayPlayback.BeginState";
	const std::string ReplayEnd =					"Function GameEvent_Soccar_TA.ReplayPlayback.EndState";
	const std::string ReplaySkipped =				"Function TAGame.GameInfo_Replay_TA.HandleReplayTimeSkip";
	const std::string ChangeTeam =					"Function TAGame.GFxData_LocalPlayer_TA.ChangeTeam";
	const std::string ReplaceBot =					"Function TAGame.GFxHUD_TA.HandleReplaceBot";
	const std::string CountdownBegin =				"Function GameEvent_TA.Countdown.BeginState";
	const std::string EnterStartState =				"Function Engine.PlayerController.EnterStartState";
	const std::string LoadingScreenStart =			"Function ProjectX.EngineShare_X.EventPreLoadMap";
	const std::string LoadingScreenEnd =			"Function TAGame.TeamColorScriptedTexture_TA.OnRender";
	const std::string UpdateMipFromPNG =			"Function Engine.Texture2DDynamic.UpdateMipFromPNG";
	const std::string TextureInit =					"Function Engine.Texture2DDynamic.Init";
	const std::string SetTextureParamValue =		"Function Engine.MaterialInstance.SetTextureParameterValue";
}