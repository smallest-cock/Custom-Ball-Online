#include "pch.h"
#include "CustomBallOnline.h"


//"Function TAGame.GameObserver_TA.HandleBallAdded"
//"Function TAGame.GameObserver_TA.GetBall"
//"Function TAGame.GameInfo_Replay_TA.HandleReplayTimeSkip"
//"Function TAGame.GFxData_LocalPlayer_TA.ChangeTeam"
//"Function TAGame.FXActor_Ball_TA.StartBallFadeIn"


//"Function TAGame.GFxHUD_TA.UpdatePendingAddMessagePlayers"
//"Function TAGame.GFxHUD_TA.OnAllTeamsCreated"
void CustomBallOnline::OnAllPlayersJoined(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);
	
	// ...
}


//"Function GameEvent_TA.Countdown.BeginState"
void CustomBallOnline::OnCountdownBegin(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	// ...
}


//"Function TAGame.GameObserver_TA.HandleBallAdded"
void CustomBallOnline::OnBallAdded(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, .1);
}


//"Function GameEvent_Soccar_TA.ReplayPlayback.BeginState"
void CustomBallOnline::OnReplayBegin(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, .1);
}


//"Function GameEvent_Soccar_TA.ReplayPlayback.EndState"
void CustomBallOnline::OnReplayEnd(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, .1);
}


//"Function TAGame.GameInfo_Replay_TA.HandleReplayTimeSkip"
void CustomBallOnline::OnHandleReplaySkip(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	cvarManager->executeCommand(CvarNames::applyTexture);
}


//"Function TAGame.GFxData_LocalPlayer_TA.ChangeTeam"
void CustomBallOnline::OnChangeTeam(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	cvarManager->executeCommand(CvarNames::applyTexture);
}


//"Function TAGame.FXActor_Ball_TA.StartBallFadeIn"
void CustomBallOnline::OnStartBallFadeIn(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, 1);
}


//"Function TAGame.GFxHUD_TA.HandleReplaceBot"
void CustomBallOnline::OnReplaceBot(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	cvarManager->executeCommand(CvarNames::applyTexture);
}


// "Function ProjectX.EngineShare_X.EventPreLoadMap"
void CustomBallOnline::OnLoadingScreen(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	auto clearUnusedOnLoadingCvar = cvarManager->getCvar(CvarNames::clearUnusedTexturesOnLoading);
	if (!clearUnusedOnLoadingCvar.getBoolValue()) return;

	cvarManager->executeCommand(CvarNames::clearUnusedSavedTextures);
}


//"Function Engine.PlayerController.EnterStartState"
void CustomBallOnline::OnEnterStartState(std::string eventName)
{
	LOG("[HOOK]: {}", eventName);

	if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) return;

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		cvarManager->executeCommand(CvarNames::applyTexture);
	}, .1);
}


//"Function Engine.Texture2DDynamic.UpdateMipFromPNG"
void CustomBallOnline::OnUpdateMipFromPNG(ActorWrapper caller, void* params, std::string eventName)
{
	return;

	UTexture2DDynamic* tex = reinterpret_cast<UTexture2DDynamic*>(caller.memory_address);
	if (!tex) return;

	UTexture2DDynamic_execUpdateMipFromPNG_Params* parameters = reinterpret_cast<UTexture2DDynamic_execUpdateMipFromPNG_Params*>(params);
	if (!parameters) return;
}


//"Function Engine.Texture2DDynamic.Init"
void CustomBallOnline::OnTextureInit(ActorWrapper caller, void* params, std::string eventName)
{
	//LOG("[HOOK]: {}", eventName); 

	UTexture2DDynamic* tex = reinterpret_cast<UTexture2DDynamic*>(caller.memory_address);
	if (!tex) return;

	UTexture2DDynamic_execInit_Params* parameters = reinterpret_cast<UTexture2DDynamic_execInit_Params*>(params);
	if (!params) return;

	//LOG("=============================================");
	//LOG("pixel format : {}", parameters->InFormat);
	//LOG("size: X: {} - Y: {}", parameters->InSizeX, parameters->InSizeY);
}


//"Function Engine.MaterialInstance.SetTextureParameterValue"
void CustomBallOnline::OnSetTexParamValue(ActorWrapper caller, void* parameters, std::string eventName)
{
	//LOG("[HOOK]: {}", eventName);

	UMaterialInstance* mi = reinterpret_cast<UMaterialInstance*>(caller.memory_address);
	if (!mi) return;

	UMaterialInstance_execSetTextureParameterValue_Params* params = reinterpret_cast<UMaterialInstance_execSetTextureParameterValue_Params*>(parameters);
	if (!params) return;

	std::string paramName = params->ParameterName.ToString();

	//LOG("=============================================");
	//LOG("caller full name: {}", mi->GetFullName());
	//LOG("ParameterName: {}", paramName);
	
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

