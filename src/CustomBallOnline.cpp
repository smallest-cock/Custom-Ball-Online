#include "pch.h"
#include "ModUtils/util/Utils.hpp"
#include "CustomBallOnline.hpp"
#include "components/Instances.hpp"
#include "components/Textures.hpp"

BAKKESMOD_PLUGIN(CustomBallOnline, "Custom Ball Online", plugin_version, PLUGINTYPE_FREEPLAY)
std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void CustomBallOnline::onLoad()
{
	_globalCvarManager = cvarManager;

	if (!Instances.initGlobals())
		return;

	Format::construct_label({41, 11, 20, 6, 8, 13, 52, 12, 0, 3, 4, 52, 1, 24, 52, 44, 44, 37, 14, 22}, h_label);
	PluginUpdates::checkForUpdates(stringify_(CustomBallOnline), VERSION_STR);

	Textures.init(gameWrapper, m_enabled);

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
