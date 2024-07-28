#include "pch.h"
#include "CustomBallOnline.h"


BAKKESMOD_PLUGIN(CustomBallOnline, "Custom Ball Online", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void CustomBallOnline::onLoad()
{
	_globalCvarManager = cvarManager;


	// init globals
	Instances.InitGlobals();
	if (!Instances.CheckGlobals()) return;


	// other init
	Textures.Initialize(gameWrapper);


	/*	
		This code snippet needs to exist.. or else the game will crash on a goal replay due to an invalid UMaterialInstance ptr
		The code does literally nothing, yet it prevents the whole game from crashing??
		Maybe some kind of weird c++/compiler quirk, or the creator of the universe is fucking with me ... 
	*/
	UStaticMeshComponent* idk = nullptr;
	if (idk) {
		auto fml = idk->GetMaterial(0);
	}


	// ====================================== cvars ===========================================

	// bools
	cvarManager->registerCvar(CvarNames::enabled,						"1", "enable plugin", true, true, 0, true, 1);
	cvarManager->registerCvar(CvarNames::clearUnusedTexturesOnLoading,	"1", "clear unused saved textures on loading screens", true, true, 0, true, 1);


	// ================================== console commands ====================================
	
	cvarManager->registerNotifier(CvarNames::applyTexture,				std::bind(&CustomBallOnline::applyTexture, this, std::placeholders::_1), "", PERMISSION_ALL);
	cvarManager->registerNotifier(CvarNames::clearSavedTextures,		std::bind(&CustomBallOnline::clearSavedTextures, this, std::placeholders::_1), "", PERMISSION_ALL);
	cvarManager->registerNotifier(CvarNames::clearUnusedSavedTextures,	std::bind(&CustomBallOnline::clearUnusedSavedTextures, this, std::placeholders::_1), "", PERMISSION_ALL);
	cvarManager->registerNotifier(CvarNames::test,						std::bind(&CustomBallOnline::test, this, std::placeholders::_1), "", PERMISSION_ALL);


	// ======================================= hooks ==========================================
	
	gameWrapper->HookEventPost(Events::BallAdded,				std::bind(&CustomBallOnline::Event_BallAdded, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ReplayBegin,				std::bind(&CustomBallOnline::Event_ReplayBegin, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ReplayEnd,				std::bind(&CustomBallOnline::Event_ReplayEnd, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ReplaySkipped,			std::bind(&CustomBallOnline::Event_ReplaySkipped, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ChangeTeam,				std::bind(&CustomBallOnline::Event_ChangeTeam, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::StartBallFadeIn,			std::bind(&CustomBallOnline::Event_StartBallFadeIn, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ReplaceBot,				std::bind(&CustomBallOnline::Event_ReplaceBot, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::CountdownBegin,			std::bind(&CustomBallOnline::Event_CountdownBegin, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::EnterStartState,			std::bind(&CustomBallOnline::Event_EnterStartState, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::LoadingScreenStart,		std::bind(&CustomBallOnline::Event_LoadingScreenStart, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::LoadingScreenEnd,		std::bind(&CustomBallOnline::Event_LoadingScreenEnd, this, std::placeholders::_1));


	// hooks with caller
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::UpdateMipFromPNG,
		std::bind(&CustomBallOnline::Event_UpdateMipFromPNG, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::TextureInit,
		std::bind(&CustomBallOnline::Event_TextureInit, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::SetTextureParamValue,
		std::bind(&CustomBallOnline::Event_SetTextureParamValue, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));


	// "hook" for 'acplugin_balltexture_selectedtexture' cvar
	gameWrapper->SetTimeout([this](GameWrapper* gw) {

		CVarWrapper selectedACTextureCvar = cvarManager->getCvar(CvarNames::acSelectedTexture);
		if (!selectedACTextureCvar) {
			LOG("[ERROR] Unable to access cvar: '{}'", CvarNames::acSelectedTexture);
			return;
		}

		selectedACTextureCvar.addOnValueChanged(std::bind(&CustomBallOnline::OnACBallTextureChanged, this, std::placeholders::_1, std::placeholders::_2));
		LOG("[SUCCESS] Hooked '{}' cvar", CvarNames::acSelectedTexture);
		acHooked = true;

	}, 3);	// wait 3s after onLoad before attempting to hook 'acplugin_balltexture_selectedtexture' (to make sure it exists)


	// ========================================================================================



	LOG("Custom Ball Online has loaded :)");
}
