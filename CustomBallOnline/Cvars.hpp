#pragma once
#include <string>



namespace Cvars
{
	const std::string prefix = "cbo_";

	struct CvarData
	{
		std::string name;
		std::string description;

		// constructor to automatically add prefix to name
		CvarData(const std::string& name, const std::string& description)
			: name(prefix + name), description(description) {}
	};

	const std::string acSelectedTexture =						"acplugin_balltexture_selectedtexture";

	// ============================== cvars =============================

	// bools
	const CvarData enabled =						{ "enabled",						"plugin enabled"};
	const CvarData clearUnusedTexturesOnLoading =	{ "clear_unused_on_loading",		"clear unused textures on loading screen"};


	// ============================ commands ============================

	const CvarData applyTexture =					{ "apply_ball_texture",				"apply the currently selected ball texture"};
	const CvarData clearSavedTextures =				{ "clear_saved_textures",			"clear saved/stored ball textures"};
	const CvarData clearUnusedSavedTextures =		{ "clear_unused_saved_textures",	"clear unused saved/stored ball textures"};
	const CvarData test =							{ "test",							"test"};
}

