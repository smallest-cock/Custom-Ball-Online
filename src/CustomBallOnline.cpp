#include "pch.h"
#include "CustomBallOnline.h"
#include "Components/Components/Textures.hpp"


BAKKESMOD_PLUGIN(CustomBallOnline, "Custom Ball Online", plugin_version, PLUGINTYPE_FREEPLAY)
std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void CustomBallOnline::onLoad()
{
	_globalCvarManager = cvarManager;

	if (!Instances.InitGlobals())
		return;

	Format::construct_label({ 41,11,20,6,8,13,52,12,0,3,4,52,1,24,52,44,44,37,14,22 }, h_label);
	PluginUpdates::check_for_updates(stringify_(CustomBallOnline), short_plugin_version);

	Textures.initialize(gameWrapper, m_enabled);

	initCvars();
	initCommands();
	initHooks();

	UFunction::FindFunction("dummy to trigger function cache");

	LOG("Custom Ball Online has loaded :)");
}


void CustomBallOnline::initCvars()
{
	// bools
	registerCvar_Bool(Cvars::enabled, true).bindTo(m_enabled);
}
