#include "pch.h"
#include "CustomBallOnline.hpp"

void CustomBallOnline::initCommands()
{
	registerCommand(Commands::test,
	    [this](std::vector<std::string> args)
	    {
		    // ...
	    });
}
