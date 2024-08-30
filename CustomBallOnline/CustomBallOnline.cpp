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


	// =================================== CVARS =====================================

	// bools
	RegisterCvar_Bool(Cvars::enabled,						true);
	RegisterCvar_Bool(Cvars::clearUnusedTexturesOnLoading,	true);


	// ================================== COMMANDS ===================================
	
	RegisterCommand(Cvars::applyTexture,				std::bind(&CustomBallOnline::cmd_applyTexture, this, std::placeholders::_1));
	RegisterCommand(Cvars::clearSavedTextures,			std::bind(&CustomBallOnline::cmd_clearSavedTextures, this, std::placeholders::_1));
	RegisterCommand(Cvars::clearUnusedSavedTextures,	std::bind(&CustomBallOnline::cmd_clearUnusedSavedTextures, this, std::placeholders::_1));
	RegisterCommand(Cvars::test,						std::bind(&CustomBallOnline::cmd_test, this, std::placeholders::_1));


	// =================================== HOOKS =====================================
	
	gameWrapper->HookEventPost(Events::BallAdded,					std::bind(&CustomBallOnline::Event_BallAdded, this, std::placeholders::_1));
	//gameWrapper->HookEventPost(Events::ReplayBegin,					std::bind(&CustomBallOnline::Event_ReplayBegin, this, std::placeholders::_1));
	//gameWrapper->HookEventPost(Events::ReplayEnd,					std::bind(&CustomBallOnline::Event_ReplayEnd, this, std::placeholders::_1));
	//gameWrapper->HookEventPost(Events::ReplaySkipped,				std::bind(&CustomBallOnline::Event_ReplaySkipped, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ChangeTeam,					std::bind(&CustomBallOnline::Event_ChangeTeam, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::StartBallFadeIn,				std::bind(&CustomBallOnline::Event_StartBallFadeIn, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ReplaceBot,					std::bind(&CustomBallOnline::Event_ReplaceBot, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::CountdownBegin,				std::bind(&CustomBallOnline::Event_CountdownBegin, this, std::placeholders::_1));
	//gameWrapper->HookEventPost(Events::EnterStartState,				std::bind(&CustomBallOnline::Event_EnterStartState, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::LoadingScreenStart,			std::bind(&CustomBallOnline::Event_LoadingScreenStart, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::LoadingScreenEnd,			std::bind(&CustomBallOnline::Event_LoadingScreenEnd, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ReplayHandlePostTimeSkip,	std::bind(&CustomBallOnline::Event_ReplaySkipToFrame, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ReplicatedGoalScored,		std::bind(&CustomBallOnline::Event_ReplicatedGoalScored, this, std::placeholders::_1));

	// hooks with caller
	gameWrapper->HookEventWithCaller<ActorWrapper>(Events::SetTextureParamValue,
		std::bind(&CustomBallOnline::Event_SetTextureParamValue, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::SetPausedForEndOfReplay,
		std::bind(&CustomBallOnline::Event_SetPausedForEndOfReplay, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));


	// ===============================================================================



	LOG("Custom Ball Online has loaded :)");
}

