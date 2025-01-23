#include "pch.h"
#include "CustomBallOnline.h"



void CustomBallOnline::cmd_applyTexture(std::vector<std::string> args)
{
	if (!PluginEnabled()) return;

	auto acSelectedTexture_cvar = cvarManager->getCvar(Cvars::acSelectedTexture);
	if (!acSelectedTexture_cvar) {
		LOG("[ERROR] Unable to access cvar: '{}'", Cvars::acSelectedTexture);
		return;
	}

	Textures.apply_selected_tex_to_ball(acSelectedTexture_cvar.getStringValue());
}


void CustomBallOnline::cmd_clearSavedTextures(std::vector<std::string> args)
{
	if (!PluginEnabled()) return;

	Textures.ClearSavedTextures();
}


void CustomBallOnline::cmd_clearUnusedSavedTextures(std::vector<std::string> args)
{
	if (!PluginEnabled()) return;

	auto acSelectedTexture_cvar = cvarManager->getCvar(Cvars::acSelectedTexture);
	if (!acSelectedTexture_cvar) {
		LOG("[ERROR] Unable to access cvar: '{}'", Cvars::acSelectedTexture);
		return;
	}

	Textures.ClearUnusedSavedTextures(acSelectedTexture_cvar.getStringValue());
}


void CustomBallOnline::cmd_test(std::vector<std::string> args)
{
	auto ac_tex_cvar = cvarManager->getCvar(Cvars::acSelectedTexture);
	if (!ac_tex_cvar) return;

	auto ball_image = Textures.get_img_path(ac_tex_cvar.getStringValue(), "Diffuse");
	LOG("ball_image: {}", ball_image.string());
}