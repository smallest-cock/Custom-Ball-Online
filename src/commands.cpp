#include "pch.h"
#include "custom_ball_online.hpp"

void CustomBallOnline::initCommands() {
	registerCommand(Commands::test, [this](std::vector<std::string> args) {
		// ...
	});
}
