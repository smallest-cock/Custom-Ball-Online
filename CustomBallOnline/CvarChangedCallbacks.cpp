#include "pch.h"
#include "CustomBallOnline.h"



void CustomBallOnline::changed_acSelectedTexture(std::string cvarName, CVarWrapper newCvar)
{
	auto enabled_cvar = GetCvar(Cvars::enabled);
	if (!enabled_cvar || !enabled_cvar.getBoolValue()) return;

	States gameState = GetGameState();
	if (gameState == States::InMatch || gameState == States::InReplay)
	{
		std::string newTexName = newCvar.getStringValue();

		DELAY_CAPTURE(0.1f, newTexName,

			LOG("Applying new '{}' texture selected in AlphaConsole...", newTexName);
			Textures.LoadTexture(newTexName);
		);
	}
	else {
		LOG("Didn't apply current AlphaConsole ball texture bc not in an online game");
	}
}