#include "pch.h"
#include "CustomBallOnline.h"



void CustomBallOnline::applyTexture(std::vector<std::string> args)
{
	auto selectedTextureCvar = cvarManager->getCvar(CvarNames::acSelectedTexture);
	if (!selectedTextureCvar) {
		LOG("[ERROR] '{}' Cvar doesnt exist!", CvarNames::acSelectedTexture);
		return;
	}

	Textures.LoadTexture(selectedTextureCvar.getStringValue());
}


void CustomBallOnline::clearSavedTextures(std::vector<std::string> args)
{
	auto enabledCvar = cvarManager->getCvar(CvarNames::enabled);
	if (!enabledCvar.getBoolValue()) return;

	Textures.ClearSavedTextures();
}


void CustomBallOnline::clearUnusedSavedTextures(std::vector<std::string> args)
{
	auto enabledCvar = cvarManager->getCvar(CvarNames::enabled);
	if (!enabledCvar.getBoolValue()) return;

	CVarWrapper selectedACTextureCvar = cvarManager->getCvar(CvarNames::acSelectedTexture);
	if (!selectedACTextureCvar) {
		LOG("[ERROR] Unable to access cvar: '{}'", CvarNames::acSelectedTexture);
		return;
	}

	Textures.ClearUnusedSavedTextures(selectedACTextureCvar.getStringValue());
}


void CustomBallOnline::test(std::vector<std::string> args)
{
	if (args.size() < 2) return;

	auto cars = Instances.GetAllInstancesOf<ACar_TA>();

	for (auto& car : cars)
	{
		if (!car) continue;

		float scale = std::stof(args[1]);

		car->SetCarScale(scale);
	}

	LOG("did the test thing...");
}