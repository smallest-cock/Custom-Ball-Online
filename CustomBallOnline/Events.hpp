#pragma once
#include <string>


namespace Events
{
	constexpr const char* BallAdded =					"Function TAGame.GameObserver_TA.HandleBallAdded";
	constexpr const char* StartBallFadeIn =				"Function TAGame.FXActor_Ball_TA.StartBallFadeIn";
	constexpr const char* ReplayBegin =					"Function GameEvent_Soccar_TA.ReplayPlayback.BeginState";
	constexpr const char* ReplayEnd =					"Function GameEvent_Soccar_TA.ReplayPlayback.EndState";
	constexpr const char* ReplaySkipped =				"Function TAGame.GameInfo_Replay_TA.HandleReplayTimeSkip";
	constexpr const char* ChangeTeam =					"Function TAGame.GFxData_LocalPlayer_TA.ChangeTeam";
	constexpr const char* ReplaceBot =					"Function TAGame.GFxHUD_TA.HandleReplaceBot";
	constexpr const char* CountdownBegin =				"Function GameEvent_TA.Countdown.BeginState";
	constexpr const char* EnterStartState =				"Function Engine.PlayerController.EnterStartState";
	constexpr const char* LoadingScreenStart =			"Function ProjectX.EngineShare_X.EventPreLoadMap";
	constexpr const char* LoadingScreenEnd =			"Function TAGame.TeamColorScriptedTexture_TA.OnRender";
	constexpr const char* UpdateMipFromPNG =			"Function Engine.Texture2DDynamic.UpdateMipFromPNG";
	constexpr const char* TextureInit =					"Function Engine.Texture2DDynamic.Init";
	constexpr const char* SetTextureParamValue =		"Function Engine.MaterialInstance.SetTextureParameterValue";
	constexpr const char* ReplaySkipToFrame =			"Function TAGame.Replay_TA.SkipToFrame";
	constexpr const char* ReplayEventSpawned =			"Function TAGame.Replay_TA.EventSpawned";
	constexpr const char* ReplayHandlePostTimeSkip =	"Function TAGame.GFxHUD_Replay_TA.HandlePostTimeSkip";
	constexpr const char* SetPausedForEndOfReplay =		"Function TAGame.GFxHUD_Replay_TA.SetPausedForEndOfReplay";
	constexpr const char* ReplicatedGoalScored =		"Function TAGame.GameEvent_Soccar_TA.EventReplicatedGoalScored";
	constexpr const char* CasualIntermission =			"Function GameEvent_Lobby_TA.PlayWithBall.BeginState";
}