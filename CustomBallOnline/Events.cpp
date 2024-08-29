#include "pch.h"
#include "CustomBallOnline.h"



void CustomBallOnline::Event_LoadingScreenEnd(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;
}


void CustomBallOnline::Event_AllPlayersJoined(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;
}


void CustomBallOnline::Event_CountdownBegin(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;
	
	if (gameWrapper->IsInReplay())
	{
		DELAY_RUN_COMMAND(Cvars::applyTexture, 0.1f);
	}
}


void CustomBallOnline::Event_BallAdded(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;
	
	DELAY_RUN_COMMAND(Cvars::applyTexture, 0.1f);
}


void CustomBallOnline::Event_ReplayBegin(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	DELAY_RUN_COMMAND(Cvars::applyTexture, 0.1f);
}


void CustomBallOnline::Event_ReplayEnd(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	DELAY_RUN_COMMAND(Cvars::applyTexture, 0.1f);
}


void CustomBallOnline::Event_ReplaySkipped(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	RUN_COMMAND(Cvars::applyTexture);
}


void CustomBallOnline::Event_ChangeTeam(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	RUN_COMMAND(Cvars::applyTexture);
}


void CustomBallOnline::Event_StartBallFadeIn(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	DELAY_RUN_COMMAND(Cvars::applyTexture, 1.0f);
}


void CustomBallOnline::Event_ReplaceBot(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	RUN_COMMAND(Cvars::applyTexture);
}


void CustomBallOnline::Event_LoadingScreenStart(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	auto clearUnusedTexturesOnLoading_cvar = GetCvar(Cvars::clearUnusedTexturesOnLoading);
	if (!clearUnusedTexturesOnLoading_cvar || !clearUnusedTexturesOnLoading_cvar.getBoolValue()) return;

	RUN_COMMAND(Cvars::clearUnusedSavedTextures);

	// ----------- hook 'acplugin_balltexture_selectedtexture' Cvar if not already hooked -----------
	if (!acHooked)
	{
		auto acSelectedTexture_cvar = cvarManager->getCvar(Cvars::acSelectedTexture);
		if (!acSelectedTexture_cvar) {
			LOG("[ERROR] Unable to access cvar: '{}'", Cvars::acSelectedTexture);
			return;
		}

		acSelectedTexture_cvar.addOnValueChanged(std::bind(&CustomBallOnline::changed_acSelectedTexture, this, std::placeholders::_1, std::placeholders::_2));
		LOG("[SUCCESS] Hooked '{}' Cvar", Cvars::acSelectedTexture);
		acHooked = true;
	}
}


void CustomBallOnline::Event_EnterStartState(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	States gameState = GetGameState();
	if (gameState == States::Freeplay || gameState == States::InReplay) return;

	DELAY_RUN_COMMAND(Cvars::applyTexture, 0.1f);
}


void CustomBallOnline::Event_ReplaySkipToFrame(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	DELAY_RUN_COMMAND(Cvars::applyTexture, 0.1f);
}


void CustomBallOnline::Event_ReplicatedGoalScored(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	DELAY_RUN_COMMAND(Cvars::applyTexture, 0.05f);
	//RUN_COMMAND(Cvars::applyTexture);
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

		DELAY_CAPTURE(0.1f, replay,

			RUN_COMMAND(Cvars::applyTexture);

			DELAY_CAPTURE(0.1f, replay,

				if (replay && replay->IsA<AGFxHUD_Replay_TA>())
				{
					replay->ReplayPause();
				}
				else {
					LOG("[ERROR] AGFxHUD_Replay_TA* is null (from capture)");
				}

			);
			
		);
	}
}


void CustomBallOnline::Event_SetTextureParamValue(ActorWrapper caller, void* parameters, std::string eventName)
{
	UMaterialInstance* mi = reinterpret_cast<UMaterialInstance*>(caller.memory_address);
	if (!mi || !mi->IsA<UMaterialInstance>()) return;

	UMaterialInstance_execSetTextureParameterValue_Params* params = reinterpret_cast<UMaterialInstance_execSetTextureParameterValue_Params*>(parameters);
	if (!params) return;

	std::string paramName = params->ParameterName.ToString();
	
	UTexture* tex = params->Value;
	if (!tex) {
		LOG("UTexture* param is null!");
		return;
	}

	if (mi->GetFullName().find("Ball_TA.StaticMeshComponent") != std::string::npos	// only save texture if it's being applied to a ball
		&& gameWrapper->IsInFreeplay() && Textures.freeToSaveTextures)
	{
		auto acSelectedTexture_cvar = cvarManager->getCvar(Cvars::acSelectedTexture);
		if (!acSelectedTexture_cvar) return;
		std::string texName = acSelectedTexture_cvar.getStringValue();

		// save texture
		UTexture2DDynamic* dynamicTex = reinterpret_cast<UTexture2DDynamic*>(tex);
		
		// cleanup old texture before saving new one
		if (Textures.savedTextures.find(texName) != Textures.savedTextures.end())
		{
			if (Textures.savedTextures[texName].find(paramName) != Textures.savedTextures[texName].end())
			{
				Instances.MarkForDestroy(Textures.savedTextures[texName][paramName]);
			}
		}

		Instances.MarkInvincible(dynamicTex);
		Textures.savedTextures[texName][paramName] = dynamicTex;


		LOG("[Events] Updated '{}' {} in savedTextures...", texName, paramName);
	}
}

