#include "pch.h"
#include "CustomBallOnline.h"



enum States : uint8_t
{
	MainMenu = 0,
	Freeplay = 1,
	InReplay = 2,
	InMatch = 3
};

namespace GameState
{
	States GetState(std::shared_ptr<GameWrapper> gw)
	{
		if (gw->IsInFreeplay())
		{
			return States::Freeplay;
		}
		else if (gw->IsInReplay())
		{
			return States::InReplay;
		}
		else if (gw->IsInOnlineGame() || gw->IsInGame())
		{
			return States::InMatch;
		}
		else {
			return States::MainMenu;
		}
	}

	bool IsInMatch(std::shared_ptr<GameWrapper> gw)
	{
		return GetState(gw) == States::InMatch;
	}

	bool IsInFreeplay(std::shared_ptr<GameWrapper> gw)
	{
		return GetState(gw) == States::Freeplay;
	}

	bool IsInMainMenu(std::shared_ptr<GameWrapper> gw)
	{
		return GetState(gw) == States::MainMenu;
	}

	bool IsInReplay(std::shared_ptr<GameWrapper> gw)
	{
		return GetState(gw) == States::InReplay;
	}
}



void CustomBallOnline::Event_LoadingScreenEnd(std::string eventName)
{
	if (!PluginEnabled()) return;
	DEBUGLOG("[HOOK]: {}", eventName);
}


void CustomBallOnline::Event_AllPlayersJoined(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);
}


void CustomBallOnline::Event_CountdownBegin(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);
	
	if (gameWrapper->IsInReplay())
	{
		RunCommand(Cvars::applyTexture, 0.1f);
	}
}


void CustomBallOnline::Event_BallAdded(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;
	
	RunCommand(Cvars::applyTexture, 0.1f);
}


void CustomBallOnline::Event_ReplayBegin(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	RunCommand(Cvars::applyTexture, 0.1f);
}


void CustomBallOnline::Event_ReplayEnd(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	RunCommand(Cvars::applyTexture, 0.1f);
}


void CustomBallOnline::Event_ReplaySkipped(std::string eventName)
{
	if(!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	RunCommand(Cvars::applyTexture);
}


void CustomBallOnline::Event_ChangeTeam(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	RunCommand(Cvars::applyTexture);
}


void CustomBallOnline::Event_StartBallFadeIn(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	RunCommandInterval(Cvars::applyTexture, 4, 0.3f, true);
}


void CustomBallOnline::Event_ReplaceBot(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	RunCommand(Cvars::applyTexture);
}


void CustomBallOnline::Event_LoadingScreenStart(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	auto clearUnusedTexturesOnLoading_cvar = GetCvar(Cvars::clearUnusedTexturesOnLoading);
	if (!clearUnusedTexturesOnLoading_cvar || !clearUnusedTexturesOnLoading_cvar.getBoolValue()) return;

	RunCommand(Cvars::clearUnusedSavedTextures);
}


void CustomBallOnline::Event_EnterStartState(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	States gameState = GameState::GetState(gameWrapper);
	if (gameState == States::Freeplay || gameState == States::InReplay) return;

	RunCommandInterval(Cvars::applyTexture, 2, 0.1f);
}


void CustomBallOnline::Event_ReplaySkipToFrame(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	RunCommand(Cvars::applyTexture, 0.1f);
}


void CustomBallOnline::Event_ReplicatedGoalScored(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	if (GameState::IsInFreeplay(gameWrapper)) return;

	RunCommand(Cvars::applyTexture, 0.05f);
}


void CustomBallOnline::Event_CasualIntermission(std::string eventName)
{
	if (!PluginEnabled()) return;
	LOG("[HOOK]: {}", eventName);

	RunCommandInterval(Cvars::applyTexture, 5, 0.2f);
}


// ==================================== hooks with caller ====================================

void CustomBallOnline::Event_SetPausedForEndOfReplay(ActorWrapper caller, void* parameters, std::string eventName)
{
	LOG("[HOOK] {}", eventName);

	AGFxHUD_Replay_TA* replay = reinterpret_cast<AGFxHUD_Replay_TA*>(caller.memory_address);
	if (!replay || !replay->IsA<AGFxHUD_Replay_TA>()) return;

	if (replay->bUserPaused || replay->bPausedForEndOfReplay)
	{
		replay->ReplayPause();

		DELAY_CAPTURE(0.1f,

			RunCommand(Cvars::applyTexture);

			DELAY_CAPTURE(0.1f,

				if (replay && replay->IsA<AGFxHUD_Replay_TA>())
				{
					replay->ReplayPause();
				}
				else {
					LOG("[ERROR] AGFxHUD_Replay_TA* is null (from capture)");
				}

			, replay);
			
		, replay);
	}
}


void CustomBallOnline::Event_SetTextureParamValue_MI(ActorWrapper caller, void* parameters, std::string eventName)
{
	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	UMaterialInstance* mi = reinterpret_cast<UMaterialInstance*>(caller.memory_address);
	if (!mi || !mi->IsA<UMaterialInstance>()) return;

	UMaterialInterface* parent = mi->Parent;
	if (!parent || !parent->IsA<UMaterialInterface>()) return;

	UMaterialInstance_execSetTextureParameterValue_Params* params = reinterpret_cast<UMaterialInstance_execSetTextureParameterValue_Params*>(parameters);
	if (!params) return;

	Textures.HandleSetTexParamVal(mi, parent, params);
}


void CustomBallOnline::Event_SetTextureParamValue_MIC(ActorWrapper caller, void* parameters, std::string eventName)
{
	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	UMaterialInstanceConstant* mic = reinterpret_cast<UMaterialInstanceConstant*>(caller.memory_address);
	if (!mic || !mic->IsA<UMaterialInstanceConstant>()) return;

	UMaterialInterface* parent = mic->Parent;
	if (!parent || !parent->IsA<UMaterialInterface>()) return;

	// The params struct for MI and MIC SetTextureParamValue are the same, so we can just cast to the MI params struct here (to avoid making a
	// whole new Textures.HandleSetTexParamVal function to accept a MIC params struct ptr)
	UMaterialInstance_execSetTextureParameterValue_Params* params = reinterpret_cast<UMaterialInstance_execSetTextureParameterValue_Params*>(parameters);
	if (!params) return;

	Textures.HandleSetTexParamVal(mic, parent, params);
}

