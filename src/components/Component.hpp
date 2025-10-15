#pragma once
#include "Instances.hpp"
#include <ModUtils/wrappers/GFxWrapper.hpp>
#include "Cvars.hpp"

enum class EGameStates : uint8_t
{
	OnlineMatch,
	Replay,
	Freeplay,
	Training,
	LocalMatch,
	Menu
};

enum class EMatchType : uint8_t
{
	Freeplay,
	PhysicsLogPlayback,
	Tutorial,
	FTE,
	Offline,

	AdHoc,
	OnlineFreeplay,
	Private,

	Lan,
	Custom,

	AutoTournament,
	FaceIt,
	Tournament,
	PublicRanked,
	Public,

	Base,
	Unknown
};

template <typename Derived> class Component
{
private:
public:
	std::shared_ptr<GameWrapper> gameWrapper;

	template <typename... Args>
	static void LOG(std::string_view format_str, Args&&... args) // overload LOG function to add component name prefix
	{
		std::string strWithComponentName = std::format("[{}] {}", Derived::componentName, format_str);
		::LOG(strWithComponentName, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void LOGERROR(std::string_view format_str, Args&&... args) // overload LOG function to add component name prefix
	{
		std::string strWithComponentName = std::format("[{}] ERROR: {}", Derived::componentName, format_str);
		::LOG(strWithComponentName, std::forward<Args>(args)...);
	}

	// hooks
	void hookEvent(const char* funcName, std::function<void(std::string eventName)> callback)
	{
		gameWrapper->HookEvent(funcName, callback);
		LOG("Hooked function pre: \"{}\"", funcName);
	}

	void hookEventPost(const char* funcName, std::function<void(std::string eventName)> callback)
	{
		gameWrapper->HookEventPost(funcName, callback);
		LOG("Hooked function post: \"{}\"", funcName);
	}

	void hookWithCaller(const char* function_name, std::function<void(ActorWrapper Caller, void* Params, std::string eventName)> callback)
	{
		gameWrapper->HookEventWithCaller<ActorWrapper>(function_name, callback);
		LOG("Hooked function pre: \"{}\"", function_name);
	}

	void hookWithCallerPost(
	    const char* function_name, std::function<void(ActorWrapper Caller, void* Params, std::string eventName)> callback)
	{
		gameWrapper->HookEventWithCallerPost<ActorWrapper>(function_name, callback);
		LOG("Hooked function post: \"{}\"", function_name);
	}

	// cvars
	CVarWrapper getCvar(const CvarData& cvar) { return _globalCvarManager->getCvar(cvar.name); }

	CVarWrapper registerCvar_bool(const CvarData& cvar, bool startingValue, bool log = true)
	{
		std::string value = startingValue ? "1" : "0";

		if (log)
			LOG("Registered CVar: {}", cvar.name);
		return _globalCvarManager->registerCvar(cvar.name, value, cvar.description, true, true, 0, true, 1);
	}

	CVarWrapper registerCvar_string(const CvarData& cvar, const std::string& startingValue, bool log = true)
	{
		if (log)
			LOG("Registered CVar: {}", cvar.name);
		return _globalCvarManager->registerCvar(cvar.name, startingValue, cvar.description);
	}

	CVarWrapper registerCvar_number(
	    const CvarData& cvar, float startingValue, bool hasMinMax = false, float min = 0.0f, float max = 0.0f, bool log = true)
	{
		std::string numberStr = std::to_string(startingValue);

		if (log)
			LOG("Registered CVar: {}", cvar.name);
		if (hasMinMax)
		{
			return _globalCvarManager->registerCvar(cvar.name, numberStr, cvar.description, true, true, min, true, max);
		}
		else
		{
			return _globalCvarManager->registerCvar(cvar.name, numberStr, cvar.description);
		}
	}

	CVarWrapper registerCvar_color(const CvarData& cvar, const std::string& startingValue, bool log = true)
	{
		if (log)
			LOG("Registered CVar: {}", cvar.name);
		return _globalCvarManager->registerCvar(cvar.name, startingValue, cvar.description);
	}

	// commands
	void runCommand(const CvarData& command, float delaySeconds = 0.0f)
	{
		if (delaySeconds == 0)
		{
			_globalCvarManager->executeCommand(command.name);
		}
		else if (delaySeconds > 0)
		{
			gameWrapper->SetTimeout([this, command](GameWrapper* gw) { _globalCvarManager->executeCommand(command.name); }, delaySeconds);
		}
	}

	void autoRunCommand(const CvarData& autoRunBool, const CvarData& command, float delaySeconds = 0.0f)
	{
		auto autoRunBool_cvar = getCvar(autoRunBool);
		if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue())
			return;

		runCommand(command, delaySeconds);
	}

	void runCommandInterval(const CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand = false)
	{
		if (!delayFirstCommand)
		{
			runCommand(command);
			numIntervals--;
		}

		for (int i = 1; i <= numIntervals; ++i)
			runCommand(command, delaySeconds * i);
	}

	void autoRunCommandInterval(
	    const CvarData& autoRunBool, const CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand = false)
	{
		auto autoRunBool_cvar = getCvar(autoRunBool);
		if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue())
			return;

		runCommandInterval(command, numIntervals, delaySeconds, delayFirstCommand);
	}

	void registerCommand(const CvarData& cvar, std::function<void(std::vector<std::string>)> callback)
	{
		_globalCvarManager->registerNotifier(cvar.name, callback, cvar.description, PERMISSION_ALL);
	}

	// game state
	// thx segalll
	// https://github.com/segalll/DiscordRPCPlugin/blob/ad3b93b04d0b7867d55554846ae91ba467123673/source/DiscordRPCPlugin/DiscordRPCPlugin.cpp#L227-L245
	EGameStates getGameState()
	{
		if (gameWrapper->IsInReplay()) // must come before online game
			return EGameStates::Replay;
		else if (gameWrapper->IsInOnlineGame())
			return EGameStates::OnlineMatch;
		else if (gameWrapper->IsInFreeplay())
			return EGameStates::Freeplay;
		else if (gameWrapper->IsInCustomTraining())
			return EGameStates::Training;
		else if (gameWrapper->IsInGame())
			return EGameStates::LocalMatch;
		else
			return EGameStates::Menu;
	}

	bool isInMatchOrReplay() { return gameWrapper->IsInReplay() || gameWrapper->IsInOnlineGame() || gameWrapper->IsInGame(); }

	bool isInMatch() { return !gameWrapper->IsInReplay() && (gameWrapper->IsInOnlineGame() || gameWrapper->IsInGame()); }

	EMatchType getMatchType(UMatchType_TA* matchType)
	{
		if (!validUObject(matchType))
			return EMatchType::Unknown;

		static std::unordered_map<UClass*, EMatchType> resultCache;

		UClass* uClass = matchType->Class;
		if (auto it = resultCache.find(uClass); it != resultCache.end())
			return it->second;

		EMatchType result = EMatchType::Unknown;

		if (matchType->IsA<UMatchType_Offline_TA>())
		{
			if (matchType->IsA<UMatchType_Tutorial_TA>())
			{
				if (matchType->IsA<UMatchType_Freeplay_TA>())
					result = EMatchType::Freeplay;
				else if (matchType->IsA<UMatchType_PhysicsLogPlayback_TA>())
					result = EMatchType::PhysicsLogPlayback;
				else
					result = EMatchType::Tutorial;
			}
			else if (matchType->IsA<UMatchType_FTE_TA>())
				result = EMatchType::FTE;
			else
				result = EMatchType::Offline;
		}
		else if (matchType->IsA<UMatchType_Private_TA>())
		{
			if (matchType->IsA<UMatchType_AdHoc_TA>())
				result = EMatchType::AdHoc;
			else if (matchType->IsA<UMatchType_OnlineFreeplay_TA>())
				result = EMatchType::OnlineFreeplay;
			else
				result = EMatchType::Private;
		}
		else if (matchType->IsA<UMatchType_Lan_TA>())
			result = EMatchType::Lan;
		else if (matchType->IsA<UMatchType_Custom_TA>())
			result = EMatchType::Custom;
		else if (matchType->IsA<UMatchType_Tournament_TA>())
		{
			if (matchType->IsA<UMatchType_AutoTournament_TA>())
				result = EMatchType::AutoTournament;
			else if (matchType->IsA<UMatchType_FaceIt_TA>())
				result = EMatchType::FaceIt;
			else
				result = EMatchType::Tournament;
		}
		else if (matchType->IsA<UMatchType_PublicRanked_TA>())
			result = EMatchType::PublicRanked;
		else if (matchType->IsA<UMatchType_Public_TA>())
			result = EMatchType::Public;
		else if (matchType->IsA<UMatchType_TA>())
			result = EMatchType::Base;

		resultCache[uClass] = result; // cache result for future calls
		return result;
	}

	bool isInMainMenu()
	{
		APlayerController* pc = Instances.getPlayerController();
		if (!pc)
		{
			LOGERROR("APlayerController* is null");
			return false;
		}

		if (!validUObject(pc->myHUD) || !pc->myHUD->IsA<AHUDBase_TA>())
		{
			LOGERROR("pc->myHUD is invalid or not a AHUDBase_TA");
			return false;
		}

		auto* hud = static_cast<AHUDBase_TA*>(pc->myHUD);
		if (!validUObject(hud->Shell) || !validUObject(hud->Shell->SystemData))
		{
			LOGERROR("hud->Shell or hud->Shell->SystemData is invalid");
			return false;
		}

		return hud->Shell->SystemData->UIState == L"MainMenu";
	}
};