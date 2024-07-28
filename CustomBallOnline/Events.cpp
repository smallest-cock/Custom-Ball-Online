#include "pch.h"
#include "CustomBallOnline.h"


//"Function TAGame.GameObserver_TA.HandleBallAdded"
//"Function TAGame.GameObserver_TA.GetBall"
//"Function TAGame.GameInfo_Replay_TA.HandleReplayTimeSkip"
//"Function TAGame.GFxData_LocalPlayer_TA.ChangeTeam"
//"Function TAGame.FXActor_Ball_TA.StartBallFadeIn"


void CustomBallOnline::Event_LoadingScreenEnd(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;

	LOG("[HOOK]: {}", eventName);
}


void CustomBallOnline::Event_AllPlayersJoined(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;

	LOG("[HOOK]: {}", eventName);
}


void CustomBallOnline::Event_CountdownBegin(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;

	LOG("[HOOK]: {}", eventName);
}


void CustomBallOnline::Event_BallAdded(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;
	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;
	
	LOG("[HOOK]: {}", eventName);


	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, .1);
}


void CustomBallOnline::Event_ReplayBegin(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;
	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	LOG("[HOOK]: {}", eventName);


	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, .1);
}


void CustomBallOnline::Event_ReplayEnd(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;
	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	LOG("[HOOK]: {}", eventName);

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, .1);
}


void CustomBallOnline::Event_ReplaySkipped(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;
	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	LOG("[HOOK]: {}", eventName);

	cvarManager->executeCommand(CvarNames::applyTexture);
}


void CustomBallOnline::Event_ChangeTeam(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;
	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	LOG("[HOOK]: {}", eventName);

	cvarManager->executeCommand(CvarNames::applyTexture);
}


void CustomBallOnline::Event_StartBallFadeIn(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;
	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	LOG("[HOOK]: {}", eventName);

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, 1);
}


void CustomBallOnline::Event_ReplaceBot(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;
	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	LOG("[HOOK]: {}", eventName);

	cvarManager->executeCommand(CvarNames::applyTexture);
}


void CustomBallOnline::Event_LoadingScreenStart(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;

	LOG("[HOOK]: {}", eventName);

	auto clearUnusedOnLoadingCvar = cvarManager->getCvar(CvarNames::clearUnusedTexturesOnLoading);
	if (!clearUnusedOnLoadingCvar.getBoolValue()) return;

	cvarManager->executeCommand(CvarNames::clearUnusedSavedTextures);


	// ----------- hook 'acplugin_balltexture_selectedtexture' Cvar if not already hooked -----------
	if (!acHooked) {
		CVarWrapper selectedACTextureCvar = cvarManager->getCvar(CvarNames::acSelectedTexture);
		if (!selectedACTextureCvar) {
			LOG("[ERROR] Unable to access cvar: '{}'", CvarNames::acSelectedTexture);
			return;
		}

		selectedACTextureCvar.addOnValueChanged(std::bind(&CustomBallOnline::OnACBallTextureChanged, this, std::placeholders::_1, std::placeholders::_2));
		LOG("[SUCCESS] Hooked '{}' Cvar", CvarNames::acSelectedTexture);
		acHooked = true;
	}
}


void CustomBallOnline::Event_EnterStartState(std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;

	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, .1);
}


void CustomBallOnline::Event_UpdateMipFromPNG(ActorWrapper caller, void* params, std::string eventName)
{
	return;

	UTexture2DDynamic* tex = reinterpret_cast<UTexture2DDynamic*>(caller.memory_address);
	if (!tex) return;

	UTexture2DDynamic_execUpdateMipFromPNG_Params* parameters = reinterpret_cast<UTexture2DDynamic_execUpdateMipFromPNG_Params*>(params);
	if (!parameters) return;
}


void CustomBallOnline::Event_TextureInit(ActorWrapper caller, void* params, std::string eventName)
{
	return;

	//UTexture2DDynamic* tex = reinterpret_cast<UTexture2DDynamic*>(caller.memory_address);
	//if (!tex) return;

	//UTexture2DDynamic_execInit_Params* parameters = reinterpret_cast<UTexture2DDynamic_execInit_Params*>(params);
	//if (!params) return;
}


void CustomBallOnline::Event_SetTextureParamValue(ActorWrapper caller, void* parameters, std::string eventName)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;

	UMaterialInstance* mi = reinterpret_cast<UMaterialInstance*>(caller.memory_address);
	if (!mi) return;

	UMaterialInstance_execSetTextureParameterValue_Params* params = reinterpret_cast<UMaterialInstance_execSetTextureParameterValue_Params*>(parameters);
	if (!params) return;

	std::string paramName = params->ParameterName.ToString();

	UTexture* tex = params->Value;
	if (!tex) {
		LOG("UTexture* param is null!");
		return;
	}


	if (mi->GetFullName().find("Ball_TA.StaticMeshComponent") != std::string::npos	// only save texture if it's being applied to a ball
		&& (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay())	// <--- prolly remove IsInReplay() if textures not showing up in replays
		&& Textures.freeToSaveTextures) {

		auto currentSelectedTexCvar = cvarManager->getCvar(CvarNames::acSelectedTexture);
		if (!currentSelectedTexCvar) return;
		std::string texName = currentSelectedTexCvar.getStringValue();

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
		Textures.savedTextures[texName][paramName] = dynamicTex; // savedACTextures["Razer"]["Diffuse"] = texture


		LOG("[Events] Updated '{}' {} in savedTextures...", texName, paramName);
	}
}


void CustomBallOnline::OnACBallTextureChanged(std::string cvarName, CVarWrapper newCvar)
{
	if (!cvarManager->getCvar(CvarNames::enabled).getBoolValue()) return;

	if (!gameWrapper->IsInFreeplay() && (gameWrapper->IsInOnlineGame() || gameWrapper->IsInReplay()))
	{
		std::string newTexName = newCvar.getStringValue();

		gameWrapper->SetTimeout([this, newTexName](GameWrapper* gw) {

			LOG("Applying new '{}' texture selected in AlphaConsole...", newTexName);
			Textures.LoadTexture(newTexName);

		}, .1);	//	wait 0.1s before applying custom texture, so AC can finish applying default ball texture first
	}
	else {
		LOG("Didn't apply current AlphaConsole ball texture bc not in an online game");
	}
}