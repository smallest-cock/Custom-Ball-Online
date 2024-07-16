#include "pch.h"
#include "CustomBallOnline.h"


BAKKESMOD_PLUGIN(CustomBallOnline, "Custom Ball Online", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void CustomBallOnline::onLoad()
{
	_globalCvarManager = cvarManager;

	// initialize globals
	Instances.InitGlobals();
	if (!Instances.CheckGlobals()) return;

	// other init
	Textures.Initialize(gameWrapper->GetDataFolder());


	// This code snippet needs to exist.. or else the game will crash on a goal replay due to an invalid UMaterialInstance ptr
	// The code does literally nothing, yet it prevents the whole game from crashing??
	// Maybe some kind of weird c++/compiler quirk, or the creator of the universe is fucking with me ...
	// -----------------------------------------------
	UStaticMeshComponent* idk = nullptr;
	if (idk) {
		auto fml = idk->GetMaterial(0);
	}
	// -----------------------------------------------



	// ====================================== Cvars ===========================================

	// bools
	cvarManager->registerCvar(CvarNames::enabled, "1", "enable plugin", true, true, 0, true, 1);
	cvarManager->registerCvar(CvarNames::clearUnusedTexturesOnLoading, "1", "clear unused saved textures on loading screens", true, true, 0, true, 1);


	// ================================== console commands ====================================
	
	cvarManager->registerNotifier(CvarNames::applyTexture, [this](std::vector<std::string> args) {

		auto selectedTextureCvar = cvarManager->getCvar(CvarNames::acSelectedTexture);
		if (!selectedTextureCvar) {
			LOG("[ERROR] '{}' Cvar doesnt exist!", CvarNames::acSelectedTexture);
			return;
		}

		Textures.LoadTexture(selectedTextureCvar.getStringValue());

	}, "", PERMISSION_ALL);
	
	
	cvarManager->registerNotifier(CvarNames::clearSavedTextures, [this](std::vector<std::string> args) {

		auto enabledCvar = cvarManager->getCvar(CvarNames::enabled);
		if (!enabledCvar.getBoolValue()) return;

		Textures.ClearSavedTextures();

	}, "", PERMISSION_ALL);
	
	
	cvarManager->registerNotifier(CvarNames::clearUnusedSavedTextures, [this](std::vector<std::string> args) {

		auto enabledCvar = cvarManager->getCvar(CvarNames::enabled);
		if (!enabledCvar.getBoolValue()) return;

		CVarWrapper selectedACTextureCvar = cvarManager->getCvar(CvarNames::acSelectedTexture);
		if (!selectedACTextureCvar) {
			LOG("[ERROR] Unable to access cvar: '{}'", CvarNames::acSelectedTexture);
			return;
		}

		Textures.ClearUnusedSavedTextures(selectedACTextureCvar.getStringValue());

	}, "", PERMISSION_ALL);


	// ======================================= hooks ==========================================
	
	gameWrapper->HookEventPost(Events::ballAdded, std::bind(&CustomBallOnline::OnBallAdded, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::replayBegin, std::bind(&CustomBallOnline::OnReplayBegin, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::replayEnd, std::bind(&CustomBallOnline::OnReplayEnd, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::replaySkipped, std::bind(&CustomBallOnline::OnHandleReplaySkip, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::teamChanged, std::bind(&CustomBallOnline::OnChangeTeam, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ballFadeIn, std::bind(&CustomBallOnline::OnStartBallFadeIn, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::botReplaced, std::bind(&CustomBallOnline::OnReplaceBot, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::countdownBegin, std::bind(&CustomBallOnline::OnCountdownBegin, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::enterGameplayView, std::bind(&CustomBallOnline::OnEnterStartState, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::loadingScreen, std::bind(&CustomBallOnline::OnLoadingScreen, this, std::placeholders::_1));

	//gameWrapper->HookEventPost("Function OnlineGameJoinGame_X.WaitForAllPlayers.EndState", std::bind(&CustomBallOnline::OnAllPlayersJoined, this, std::placeholders::_1));
	//gameWrapper->HookEventPost("Function TAGame.GFxHUD_TA.UpdatePendingAddMessagePlayers", std::bind(&CustomBallOnline::OnAllPlayersJoined, this, std::placeholders::_1));
	//gameWrapper->HookEventPost("Function TAGame.GFxHUD_TA.OnAllTeamsCreated", std::bind(&CustomBallOnline::OnAllPlayersJoined, this, std::placeholders::_1));


	// ----------------- hooks with caller ----------------- 

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::mipUpdatedFromPNG,
		std::bind(&CustomBallOnline::OnUpdateMipFromPNG, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::textureInitialized,
		std::bind(&CustomBallOnline::OnTextureInit, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::textureParamValueSet,
		std::bind(&CustomBallOnline::OnSetTexParamValue, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	

	// -------------- "hook" for AC selectedtexture cvar --------------

	CVarWrapper selectedACTextureCvar = cvarManager->getCvar(CvarNames::acSelectedTexture);
	if (!selectedACTextureCvar) {
		LOG("[ERROR] Unable to access cvar: '{}'", CvarNames::acSelectedTexture);
		return;
	}

	selectedACTextureCvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	
		if (gameWrapper->IsInOnlineGame() || gameWrapper->IsInGame() || gameWrapper->IsInReplay())
		{
			std::string newTexName = newCvar.getStringValue();

			gameWrapper->SetTimeout([this, newTexName](GameWrapper* gw) {
				
				Textures.LoadTexture(newTexName);

			}, .1);	// wait 0.1s before applying custom texture, so AC can finish applying default ball texture first
		}		
	});

	// ========================================================================================


	LOG("Custom Ball Online loaded :)");
}
