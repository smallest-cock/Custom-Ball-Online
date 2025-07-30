#include "pch.h"
#include "CustomBallOnline.h"


void CustomBallOnline::initCommands()
{
	registerCommand(Commands::test, [this](std::vector<std::string> args)
	{
		// ...
	});
}
