#include "pch.h"
#include "ModUtils/util/Utils.hpp"
#include "custom_ball_online.hpp"
#include "util/instances.hpp"
#include "components/textures.hpp"

BAKKESMOD_PLUGIN(CustomBallOnline, "Custom Ball Online", plugin_version, PLUGINTYPE_FREEPLAY)
std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

BakkesMod::Plugin::BakkesModPlugin *CustomBallOnline::getPlugin() { return this; }

void CustomBallOnline::onLoad() {
	_globalCvarManager = cvarManager;

	if (!Instances.initGlobals())
		return;

	Format::construct_label({ 41, 11, 20, 6, 8, 13, 52, 12, 0, 3, 4, 52, 1, 24, 52, 44, 44, 37, 14, 22 }, h_label);
	PluginUpdates::checkForUpdates(stringify_(CustomBallOnline), VERSION_STR);

	Textures.init(gameWrapper, m_enabled);

	initCvars();
	initCommands();

	UFunction::FindFunction("they glow in the dark");

	LOG("Custom Ball Online has loaded :)");
}

void CustomBallOnline::initCvars() {
	// bools
	registerCvar_Bool(Cvars::enabled, true).bindTo(m_enabled);
}
