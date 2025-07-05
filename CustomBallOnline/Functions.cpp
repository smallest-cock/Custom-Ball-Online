#include "pch.h"
#include "CustomBallOnline.h"



void CustomBallOnline::RunCommand(const Cvars::CvarData& command, float delaySeconds)
{
	if (delaySeconds == 0)
	{
		cvarManager->executeCommand(command.name);
	}
	else if (delaySeconds > 0)
	{
		gameWrapper->SetTimeout([this, command](GameWrapper* gw) {
			cvarManager->executeCommand(command.name);
		}, delaySeconds);
	}
}


void CustomBallOnline::RunCommandInterval(const Cvars::CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand)
{
	if (!delayFirstCommand)
	{
		RunCommand(command);
		numIntervals--;
	}

	for (int i = 1; i <= numIntervals; i++)
	{
		RunCommand(command, delaySeconds * i);
	}
}


void CustomBallOnline::AutoRunCommand(const Cvars::CvarData& autoRunBool, const Cvars::CvarData& command, float delaySeconds)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue())
		return;

	RunCommand(command, delaySeconds);
}


void CustomBallOnline::AutoRunCommandInterval(const Cvars::CvarData& autoRunBool, const Cvars::CvarData& command,
	int numIntervals, float delaySeconds, bool delayFirstCommand)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue())
		return;

	RunCommandInterval(command, numIntervals, delaySeconds, delayFirstCommand);
}


bool CustomBallOnline::PluginEnabled()
{
	auto enabled_cvar = GetCvar(Cvars::enabled);
	return enabled_cvar && enabled_cvar.getBoolValue();
}
