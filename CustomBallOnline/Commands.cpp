#include "pch.h"
#include "CustomBallOnline.h"



void CustomBallOnline::cmd_applyTexture(std::vector<std::string> args)
{
	auto acSelectedTexture_cvar = cvarManager->getCvar(Cvars::acSelectedTexture);
	if (!acSelectedTexture_cvar) {
		LOG("[ERROR] Unable to access cvar: '{}'", Cvars::acSelectedTexture);
		return;
	}

	Textures.LoadTexture(acSelectedTexture_cvar.getStringValue());
}


void CustomBallOnline::cmd_clearSavedTextures(std::vector<std::string> args)
{
	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	Textures.ClearSavedTextures();
}


void CustomBallOnline::cmd_clearUnusedSavedTextures(std::vector<std::string> args)
{
	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	auto acSelectedTexture_cvar = cvarManager->getCvar(Cvars::acSelectedTexture);
	if (!acSelectedTexture_cvar) {
		LOG("[ERROR] Unable to access cvar: '{}'", Cvars::acSelectedTexture);
		return;
	}

	Textures.ClearUnusedSavedTextures(acSelectedTexture_cvar.getStringValue());
}


void CustomBallOnline::cmd_test(std::vector<std::string> args)
{
	// ...
}