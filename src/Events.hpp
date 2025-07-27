#pragma once

namespace Events
{
constexpr auto EngineShare_X_EventPreLoadMap                 = "Function ProjectX.EngineShare_X.EventPreLoadMap"; // loading screen start
constexpr auto TeamColorScriptedTexture_TA_OnRender          = "Function TAGame.TeamColorScriptedTexture_TA.OnRender"; // loading screen end

constexpr auto GFxHUD_TA_HandlePRICameraChanged              = "Function TAGame.GFxHUD_TA.HandlePRICameraChanged";
constexpr auto GFxHUD_TA_HandleReservationsUpdated           = "Function TAGame.GFxHUD_TA.HandleReservationsUpdated";
constexpr auto GFxHUD_TA_HandleReplaceBot                    = "Function TAGame.GFxHUD_TA.HandleReplaceBot";
constexpr auto GameEvent_Soccar_TA_StartNewRound             = "Function TAGame.GameEvent_Soccar_TA.StartNewRound";
constexpr auto GameEvent_Soccar_TA_SpawnNewBall              = "Function TAGame.GameEvent_Soccar_TA.SpawnNewBall";
constexpr auto ReplayPlayback_BeginState                     = "Function GameEvent_Soccar_TA.ReplayPlayback.BeginState";
constexpr auto ReplayPlayback_EndState                       = "Function GameEvent_Soccar_TA.ReplayPlayback.EndState";
constexpr auto Active_SetBallEventListeners                  = "Function GameEvent_Soccar_TA.Active.SetBallEventListeners";
constexpr auto Ball_TA_OverrideBallTexture                   = "Function TAGame.Ball_TA.OverrideBallTexture";
constexpr auto Ball_TA_EventGameEventSet                     = "Function TAGame.Ball_TA.EventGameEventSet";
constexpr auto Ball_TA_ReplicatedEvent                       = "Function TAGame.Ball_TA.ReplicatedEvent";
constexpr auto Ball_TA_PreBeginPlay                          = "Function TAGame.Ball_TA.PreBeginPlay";
constexpr auto Ball_TA_PostBeginPlay                         = "Function TAGame.Ball_TA.PostBeginPlay";
constexpr auto Ball_TA_SetGameEvent                          = "Function TAGame.Ball_TA.SetGameEvent";
constexpr auto GameEvent_Soccar_TA_ResetBalls                = "Function TAGame.GameEvent_Soccar_TA.ResetBalls";
constexpr auto FXActor_Ball_TA_HandleBallGameEventSet        = "Function TAGame.FXActor_Ball_TA.HandleBallGameEventSet";
constexpr auto FXActor_Ball_TA_StartBallFadeIn               = "Function TAGame.FXActor_Ball_TA.StartBallFadeIn";
constexpr auto FXActor_Ball_TA_HandleBallExploded            = "Function TAGame.FXActor_Ball_TA.HandleBallExploded";


constexpr auto Replay_TA_SkipToFrame                         = "Function TAGame.Replay_TA.SkipToFrame";
constexpr auto Replay_TA_EventSpawned                        = "Function TAGame.Replay_TA.EventSpawned";
constexpr auto Texture2DDynamic_Init                         = "Function Engine.Texture2DDynamic.Init";
constexpr auto Texture2DDynamic_UpdateMipFromPNG             = "Function Engine.Texture2DDynamic.UpdateMipFromPNG";
constexpr auto GFxHUD_Replay_TA_HandlePostTimeSkip           = "Function TAGame.GFxHUD_Replay_TA.HandlePostTimeSkip";
constexpr auto GFxHUD_Replay_TA_SetPausedForEndOfReplay      = "Function TAGame.GFxHUD_Replay_TA.SetPausedForEndOfReplay";
constexpr auto GameObserver_TA_HandleBallAdded               = "Function TAGame.GameObserver_TA.HandleBallAdded";
constexpr auto GFxData_LocalPlayer_TA_ChangeTeam             = "Function TAGame.GFxData_LocalPlayer_TA.ChangeTeam";
constexpr auto GameInfo_Replay_TA_HandleReplayTimeSkip       = "Function TAGame.GameInfo_Replay_TA.HandleReplayTimeSkip";
constexpr auto Countdown_BeginState                          = "Function GameEvent_TA.Countdown.BeginState";
constexpr auto PlayerController_EnterStartState              = "Function Engine.PlayerController.EnterStartState";
constexpr auto MI_SetTextureParameterValue                   = "Function Engine.MaterialInstance.SetTextureParameterValue";
constexpr auto MIC_SetTextureParameterValue                  = "Function Engine.MaterialInstanceConstant.SetTextureParameterValue";
constexpr auto GameEvent_Soccar_TA_EventReplicatedGoalScored = "Function TAGame.GameEvent_Soccar_TA.EventReplicatedGoalScored";
constexpr auto PlayWithBall_BeginState                       = "Function GameEvent_Lobby_TA.PlayWithBall.BeginState";
}