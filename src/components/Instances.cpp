#include "pch.h"
#include "Instances.hpp"

InstancesComponent::InstancesComponent() { OnCreate(); }

InstancesComponent::~InstancesComponent() { OnDestroy(); }

void InstancesComponent::OnCreate()
{
	I_UCanvas             = nullptr;
	I_AHUD                = nullptr;
	I_UGameViewportClient = nullptr;
	I_APlayerController   = nullptr;
}

void InstancesComponent::OnDestroy()
{
	m_staticClasses.clear();
	m_staticFunctions.clear();

	for (UObject* uObject : m_createdObjects)
	{
		if (uObject)
		{
			MarkForDestroy(uObject);
		}
	}

	m_createdObjects.clear();
}

// ========================================= to initialize globals ===========================================

uintptr_t InstancesComponent::FindPattern(HMODULE module, const unsigned char* pattern, const char* mask)
{
	MODULEINFO info{};
	GetModuleInformation(GetCurrentProcess(), module, &info, sizeof(MODULEINFO));

	uintptr_t start  = reinterpret_cast<uintptr_t>(module);
	size_t    length = info.SizeOfImage;

	size_t pos        = 0;
	size_t maskLength = std::strlen(mask) - 1;

	for (uintptr_t retAddress = start; retAddress < start + length; retAddress++)
	{
		if (*reinterpret_cast<unsigned char*>(retAddress) == pattern[pos] || mask[pos] == '?')
		{
			if (pos == maskLength)
			{
				return (retAddress - maskLength);
			}
			pos++;
		}
		else
		{
			retAddress -= pos;
			pos = 0;
		}
	}
	return NULL;
}

uintptr_t InstancesComponent::GetGNamesAddress()
{
	unsigned char GNamesPattern[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x35\x25\x02\x00";
	char          GNamesMask[]    = "??????xx??xxxxxx";

	auto GNamesAddress = FindPattern(GetModuleHandle(L"RocketLeague.exe"), GNamesPattern, GNamesMask);

	return GNamesAddress;
}

uintptr_t InstancesComponent::GetGObjectsAddress() { return GetGNamesAddress() + 0x48; }

void InstancesComponent::InitGlobals()
{
	uintptr_t gnamesAddr = GetGNamesAddress();
	GNames               = reinterpret_cast<TArray<FNameEntry*>*>(gnamesAddr);
	GObjects             = reinterpret_cast<TArray<UObject*>*>(gnamesAddr + 0x48);
}

bool InstancesComponent::AreGObjectsValid()
{
	if (UObject::GObjObjects()->size() > 0 && UObject::GObjObjects()->capacity() > UObject::GObjObjects()->size())
	{
		if (UObject::GObjObjects()->at(0)->GetFullName() == "Class Core.Config_ORS")
			return true;
	}
	return false;
}

bool InstancesComponent::AreGNamesValid()
{
	if (FName::Names()->size() > 0 && FName::Names()->capacity() > FName::Names()->size())
	{
		if (FName(0).ToString() == "None")
			return true;
	}
	return false;
}

bool InstancesComponent::CheckGlobals()
{
	bool gnamesValid   = GNames && AreGNamesValid();
	bool gobjectsValid = GObjects && AreGObjectsValid();
	if (!gnamesValid || !gobjectsValid)
	{
		LOG("(onLoad) Error: RLSDK classes are wrong... plugin needs an update :(");
		LOG(std::format("GNames valid: {} -- GObjects valid: {}", gnamesValid, gobjectsValid));
		return false;
	}

	LOG("Globals Initialized :)");
	return true;
}

// ===========================================================================================================

class UClass* InstancesComponent::FindStaticClass(const std::string& className)
{
	if (m_staticClasses.empty())
	{
		for (int32_t i = 0; i < (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i++)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);

			if (uObject)
			{
				if ((uObject->GetFullName().find("Class") == 0))
				{
					m_staticClasses[uObject->GetFullName()] = static_cast<UClass*>(uObject);
				}
			}
		}
	}

	if (m_staticClasses.contains(className))
	{
		return m_staticClasses[className];
	}

	return nullptr;
}

class UFunction* InstancesComponent::FindStaticFunction(const std::string& className)
{
	if (m_staticFunctions.empty())
	{
		for (int32_t i = 0; i < (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i++)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);

			if (uObject)
			{
				if (uObject && uObject->IsA<UFunction>())
				{
					m_staticFunctions[uObject->GetFullName()] = static_cast<UFunction*>(uObject);
				}
			}
		}
	}

	if (m_staticFunctions.contains(className))
	{
		return m_staticFunctions[className];
	}

	return nullptr;
}

void InstancesComponent::MarkInvincible(class UObject* object)
{
	if (object)
	{
		object->ObjectFlags &= ~EObjectFlags::RF_TagGarbage;
		object->ObjectFlags &= ~EObjectFlags::RF_PendingKill;
		object->ObjectFlags |= EObjectFlags::RF_DisregardForGC;
		object->ObjectFlags |= EObjectFlags::RF_RootSet;
	}
}

void InstancesComponent::MarkForDestroy(class UObject* object)
{
	if (object)
	{
		object->ObjectFlags |= EObjectFlags::RF_TagGarbage;
		object->ObjectFlags |= EObjectFlags::RF_PendingKill;
		object->ObjectFlags &= ~EObjectFlags::RF_DisregardForGC;
		object->ObjectFlags &= ~EObjectFlags::RF_RootSet;

		auto objectIt = std::find(m_createdObjects.begin(), m_createdObjects.end(), object);

		if (objectIt != m_createdObjects.end())
		{
			m_createdObjects.erase(objectIt);
		}
	}
}

void InstancesComponent::SimpleMarkForDestroy(class UObject* object)
{
	if (!object)
		return;

	object->ObjectFlags |= EObjectFlags::RF_TagGarbage;
	object->ObjectFlags |= EObjectFlags::RF_PendingKill;
	object->ObjectFlags &= ~EObjectFlags::RF_DisregardForGC;
	object->ObjectFlags &= ~EObjectFlags::RF_RootSet;
}

class UEngine* InstancesComponent::IUEngine() { return UEngine::GetEngine(); }

class UAudioDevice* InstancesComponent::IUAudioDevice() { return UEngine::GetAudioDevice(); }

class AWorldInfo* InstancesComponent::IAWorldInfo() { return UEngine::GetCurrentWorldInfo(); }

class UCanvas* InstancesComponent::IUCanvas() { return I_UCanvas; }

class AHUD* InstancesComponent::IAHUD() { return I_AHUD; }

class UFileSystem* InstancesComponent::IUFileSystem() { return reinterpret_cast<UFileSystem*>(UFileSystem::StaticClass()); }

class UGameViewportClient* InstancesComponent::IUGameViewportClient() { return I_UGameViewportClient; }

class ULocalPlayer* InstancesComponent::IULocalPlayer()
{
	UEngine* engine = IUEngine();

	if (engine && engine->GamePlayers[0])
	{
		return engine->GamePlayers[0];
	}

	return nullptr;
}

class APlayerController* InstancesComponent::IAPlayerController() { return I_APlayerController; }

struct FUniqueNetId InstancesComponent::GetUniqueID()
{
	ULocalPlayer* localPlayer = IULocalPlayer();
	if (!validUObject(localPlayer))
		return FUniqueNetId{};

	return localPlayer->eventGetUniqueNetId();
}

// ======================= get instance funcs =========================

AGameEvent_Team_TA* InstancesComponent::getTeamGameEvent()
{
	ULocalPlayer* localPlayer = IULocalPlayer();
	if (!validUObject(localPlayer))
		return nullptr;

	if (!validUObject(localPlayer->ViewportClient) || !localPlayer->ViewportClient->IsA<UGameViewportClient_TA>())
		return nullptr;

	auto vp = static_cast<UGameViewportClient_TA*>(localPlayer->ViewportClient);
	if (!validUObject(vp->GameEvent) || !vp->GameEvent->IsA<AGameEvent_Team_TA>())
		return nullptr;

	return static_cast<AGameEvent_Team_TA*>(vp->GameEvent);
}

USeqAct_SetStadiumTeamColors_TA* InstancesComponent::getSetStadiumTeamColorsSeqAct()
{
	static USeqAct_SetStadiumTeamColors_TA* instance = nullptr;
	if (!validUObject(instance))
		instance = reinterpret_cast<USeqAct_SetStadiumTeamColors_TA*>(USeqAct_SetStadiumTeamColors_TA::StaticClass());

	return instance;
}

APlayerController* InstancesComponent::getPlayerController()
{
	ULocalPlayer* lp = Instances.IULocalPlayer();
	if (!validUObject(lp) || !validUObject(lp->Actor) || !lp->Actor->IsA<APlayerController>())
		return nullptr;

	return static_cast<APlayerController*>(lp->Actor);
}

AGFxHUD_TA* InstancesComponent::getHUD()
{
	APlayerController* pc = getPlayerController();
	if (!pc)
		return nullptr;

	if (!validUObject(pc->myHUD) || !pc->myHUD->IsA<AGFxHUD_TA>())
		return nullptr;

	return static_cast<AGFxHUD_TA*>(pc->myHUD);
}

AGameEvent_TA* InstancesComponent::getGameEvent()
{
	AGFxHUD_TA* hud = getHUD();
	if (!hud)
		return nullptr;

	if (!validUObject(hud->GameEvent))
		return nullptr;

	return hud->GameEvent;
}

UMatchType_TA* InstancesComponent::getMatchType()
{
	AGameEvent_TA* event = getGameEvent();
	if (!event)
		return nullptr;

	if (!validUObject(event->MatchType))
		return nullptr;

	return event->MatchType;
}

UGFxDataStore_X* InstancesComponent::GetDataStore()
{
	if (!validUObject(dataStore))
		dataStore = GetInstanceOf<UGFxDataStore_X>();

	return dataStore;
}

UOnlinePlayer_X* InstancesComponent::GetOnlinePlayer()
{
	if (!validUObject(onlinePlayer))
		onlinePlayer = GetInstanceOf<UOnlinePlayer_X>();

	return onlinePlayer;
}

UCarColorSet_TA* InstancesComponent::GetColorPalette()
{
	if (!validUObject(colorPalette))
		colorPalette = GetInstanceOf<UCarColorSet_TA>();

	return colorPalette;
}

// misc
std::string InstancesComponent::fullNameWithoutClass(UObject* obj)
{
	if (!obj)
		return std::string();

	std::string fullName = obj->GetName();

	for (UObject* uOuter = obj->Outer; uOuter; uOuter = uOuter->Outer)
		fullName = (uOuter->GetName() + "." + fullName);

	return fullName;
}

void InstancesComponent::spawnNotification(const std::string& title, const std::string& content, float duration, bool log)
{
	auto notificationManager = Instances.GetInstanceOf<UNotificationManager_TA>();
	if (!validUObject(notificationManager))
		return;

	static UClass* notificationClass = nullptr;
	if (!validUObject(notificationClass))
		notificationClass = UGenericNotification_TA::StaticClass();

	UNotification_TA* notification = notificationManager->PopUpOnlyNotification(notificationClass);
	if (!validUObject(notification))
		return;

	notification->SetTitle(FString::create(title));
	notification->SetBody(FString::create(content));
	notification->PopUpDuration = duration;

	if (log)
		LOG("[{}] {}", title.c_str(), content.c_str());
}

class InstancesComponent Instances{};