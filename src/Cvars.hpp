#pragma once
#include <string>


#define CVAR(name, desc) CvarData("cbo_" name, desc) // automatically apply the prefix to cvar names

struct CvarData
{
	const char* name;
	const char* description;

	constexpr CvarData(const char* name, const char* description) : name(name), description(description) {}
};


namespace Cvars
{
	const std::string acSelectedTexture =				"acplugin_balltexture_selectedtexture";

	// bools
	constexpr CvarData enabled =						CVAR("enabled",							"plugin enabled");
	constexpr CvarData clearUnusedTexturesOnLoading =	CVAR("clear_unused_on_loading",			"clear unused textures on loading screen");
}

namespace Commands
{
	constexpr CvarData applyTexture =					CVAR("apply_ball_texture",				"apply the currently selected ball texture");
	constexpr CvarData clearSavedTextures =				CVAR("clear_saved_textures",			"clear saved/stored ball textures");
	constexpr CvarData clearUnusedSavedTextures =		CVAR("clear_unused_saved_textures",		"clear unused saved/stored ball textures");
	constexpr CvarData test =							CVAR("test",							"test");
}

#undef CVAR