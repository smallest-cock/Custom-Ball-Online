#pragma once
#include <string>


namespace CvarNames
{
	const std::string prefix = "cbo_";
	const std::string acSelectedTexture =						"acplugin_balltexture_selectedtexture";

	// vars 
	const std::string enabled = prefix +						"enabled";
	const std::string clearUnusedTexturesOnLoading = prefix +	"clear_unused_on_loading";

	// commands
	const std::string applyTexture = prefix +					"apply_ball_texture";
	const std::string clearSavedTextures = prefix +				"clear_saved_textures";
	const std::string clearUnusedSavedTextures = prefix +		"clear_unused_saved_textures";
	const std::string test = prefix +							"test";
}