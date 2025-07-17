#pragma once
#include "pch.h"
#include <ModUtils/util/Utils.hpp>


static constexpr int32_t INSTANCES_INTERATE_OFFSET = 10;

template <typename T>
concept UObjectOrDerived = std::is_base_of_v<UObject, T>;

class InstancesComponent
{
public:
	InstancesComponent();
	~InstancesComponent();

public:
	void OnCreate();
	void OnDestroy();

	// initialize globals for RLSDK
	uintptr_t FindPattern(HMODULE module, const unsigned char* pattern, const char* mask);
	uintptr_t GetGNamesAddress();
	uintptr_t GetGObjectsAddress();
	void      InitGlobals();
	bool      AreGObjectsValid();
	bool      AreGNamesValid();
	bool      CheckGlobals();

private:
	std::map<std::string, class UClass*>    m_staticClasses;
	std::map<std::string, class UFunction*> m_staticFunctions;
	std::vector<class UObject*>             m_createdObjects;

	bool CheckNotInName(UObject* obj, const std::string& str) { return obj->GetFullName().find(str) == std::string::npos; }

public:
	// Get the default constructor of a class type. Example: UGameData_TA* gameData = GetDefaultInstanceOf<UGameData_TA>();
	template <UObjectOrDerived T>
	T* GetDefaultInstanceOf()
	{
		for (int32_t i = 0; i < (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); ++i)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);
			if (!validUObject(uObject) || !uObject->IsA<T>())
				continue;

			if (uObject->ObjectFlags & RF_ClassDefaultObject)
				return static_cast<T*>(uObject);
		}

		return nullptr;
	}

	// Get the most current/active instance of a class. Example: UEngine* engine = GetInstanceOf<UEngine>();
	template <UObjectOrDerived T>
	T* GetInstanceOf(bool omitDefaultsAndArchetypes = true)
	{
		for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; --i)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);
			if (!validUObject(uObject) || !uObject->IsA<T>())
				continue;

			if (omitDefaultsAndArchetypes && uObject->ObjectFlags & RF_DefaultOrArchetypeFlags)
				continue;

			return static_cast<T*>(uObject);
		}

		return nullptr;
	}

	// Get the most current/active instance of a class, if one isn't found it creates a new instance. Example: UEngine* engine =
	// GetInstanceOf<UEngine>();
	template <UObjectOrDerived T>
	T* GetOrCreateInstance()
	{
		for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; --i)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);
			if (uObject && uObject->IsA<T>())
			{
				// if (uObject->GetFullName().find("Default__") == std::string::npos)
				if (CheckNotInName(uObject, "Default") && CheckNotInName(uObject, "Archetype") &&
					CheckNotInName(uObject, "PostGameLobby") && CheckNotInName(uObject, "Test"))
				{
					return static_cast<T*>(uObject);
				}
			}
		}

		return CreateInstance<T>();
	}

	template <UObjectOrDerived T>
	std::vector<T*> GetAllArchetypeInstancesOf()
	{
		std::vector<T*> objectInstances;

		for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; --i)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);
			if (!validUObject(uObject) || !uObject->IsA<T>())
				continue;

			if (!(uObject->ObjectFlags & RF_ArchetypeObject))
				continue;

			objectInstances.push_back(static_cast<T*>(uObject));
		}

		return objectInstances;
	}

	// Get all active instances of a class type. Example: std::vector<APawn*> pawns = GetAllInstancesOf<APawn>();
	template <UObjectOrDerived T>
	std::vector<T*> GetAllInstancesOf(bool omitDefaultsAndArchetypes = true)
	{
		std::vector<T*> objectInstances;

		for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; --i)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);
			if (!validUObject(uObject) || !uObject->IsA<T>())
				continue;

			if (omitDefaultsAndArchetypes && uObject->ObjectFlags & RF_DefaultOrArchetypeFlags)
				continue;

			objectInstances.push_back(static_cast<T*>(uObject));
		}

		return objectInstances;
	}

	// Get all default instances of a class type.
	template <UObjectOrDerived T>
	std::vector<T*> GetAllDefaultInstancesOf()
	{
		std::vector<T*> objectInstances;

		for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; i--)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);

			if (uObject && uObject->IsA<T>())
			{
				// if (uObject->GetFullName().find("Default__") != std::string::npos)
				if (uObject->ObjectFlags & RF_ClassDefaultObject)
					objectInstances.push_back(static_cast<T*>(uObject));
			}
		}

		return objectInstances;
	}

	// Get an object instance by it's name and class type. Example: UTexture2D* texture = FindObject<UTexture2D>("WhiteSquare");
	template <UObjectOrDerived T>
	T* FindObject(const std::string& objectName, bool bStrictFind = false)
	{
		for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; i--)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);

			if (uObject && uObject->IsA<T>())
			{
				std::string objectFullName = uObject->GetFullName();

				if (bStrictFind)
				{
					if (objectFullName == objectName)
					{
						return static_cast<T*>(uObject);
					}
				}
				else if (objectFullName.find(objectName) != std::string::npos)
				{
					return static_cast<T*>(uObject);
				}
			}
		}

		return nullptr;
	}

	// Get all object instances by it's name and class type. Example: std::vector<UTexture2D*> textures =
	// FindAllObjects<UTexture2D>("Noise");
	template <UObjectOrDerived T>
	std::vector<T*> FindAllObjects(const std::string& objectName)
	{
		std::vector<T*> objectInstances;

		for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; i--)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);

			if (uObject && uObject->IsA<T>())
			{
				if (uObject->GetFullName().find(objectName) != std::string::npos)
				{
					objectInstances.push_back(static_cast<T*>(uObject));
				}
			}
		}

		return objectInstances;
	}

	// Get all object instances of a class type that contain one of the search terms in its full name
	template <UObjectOrDerived T>
	std::vector<T*> FindAllObjectsThatMatch(const std::vector<std::string>& searchTerms, const std::vector<std::string>& avoidTerms = {})
	{
		std::vector<T*> objectInstances;

		for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; i--)
		{
			UObject* uObject = UObject::GObjObjects()->at(i);

			if (uObject && uObject->IsA<T>())
			{
				auto fullName = uObject->GetFullName();

				for (const auto& searchTerm : searchTerms)
				{
					if (fullName.find(searchTerm) != std::string::npos)
					{
						// check if name contains any blacklisted terms
						bool matchesBlacklistedTerm = false;
						for (const auto& avoidTerm : avoidTerms)
						{
							if (fullName.find(avoidTerm) != std::string::npos)
							{
								matchesBlacklistedTerm = true;
								break;
							}
						}

						if (!matchesBlacklistedTerm)
						{
							objectInstances.push_back(static_cast<T*>(uObject));
						}

						break; // stop looping thru search terms
					}
				}
			}
		}

		return objectInstances;
	}

	class UClass* FindStaticClass(const std::string& className);

	class UFunction* FindStaticFunction(const std::string& functionName);

	// Creates a new transient instance of a class which then adds it to globals.
	// YOU are required to make sure these objects eventually get eaten up by the garbage collector in some shape or form.
	// Example: UObject* newObject = CreateInstance<UObject>();
	template <UObjectOrDerived T>
	T* CreateInstance()
	{
		T* returnObject = nullptr;

		T*      defaultObject = GetDefaultInstanceOf<T>();
		UClass* staticClass   = T::StaticClass();

		if (defaultObject && staticClass)
			returnObject = static_cast<T*>(defaultObject->DuplicateObject(defaultObject, defaultObject->Outer, staticClass));

		// Making sure newly created object doesn't get randomly destoyed by the garbage collector when we don't want it do.
		if (returnObject)
			MarkInvincible(returnObject);

		return returnObject;
	}

	template <UObjectOrDerived T>
	static T* getArchetype(const UObject* obj)
	{
		if (!validUObject(obj) || !validUObject(obj->ObjectArchetype) || !obj->ObjectArchetype->IsA<T>())
			return nullptr;

		return static_cast<T*>(obj->ObjectArchetype);
	}

	// Set an object's flags to prevent it from being destoryed.
	void MarkInvincible(class UObject* object);

	// Set object as a temporary object and marks it for the garbage collector to destroy.
	void MarkForDestroy(class UObject* object);
	void SimpleMarkForDestroy(UObject* object);

private:
	class UCanvas*             I_UCanvas;
	class AHUD*                I_AHUD;
	class UGameViewportClient* I_UGameViewportClient;
	class APlayerController*   I_APlayerController;

public: // Use these functions to access these specific class instances, they will be set automatically; always remember to null check!
	class UEngine*             IUEngine();
	class UAudioDevice*        IUAudioDevice();
	class AWorldInfo*          IAWorldInfo();
	class UCanvas*             IUCanvas();
	class AHUD*                IAHUD();
	class UGameViewportClient* IUGameViewportClient();
	class ULocalPlayer*        IULocalPlayer();
	class APlayerController*   IAPlayerController();
	class UFileSystem*         IUFileSystem();
	struct FUniqueNetId        GetUniqueID();

public:
	AGFxHUD_TA*                      hud                    = nullptr;
	UGFxDataStore_X*                 dataStore              = nullptr;
	USaveData_TA*                    saveData               = nullptr;
	UOnlinePlayer_X*                 onlinePlayer           = nullptr;
	UCarColorSet_TA*                 colorPalette           = nullptr;
	USeqAct_SetStadiumTeamColors_TA* setStadiumColorsSeqAct = nullptr;

public:
	UGFxDataStore_X* GetDataStore();
	UOnlinePlayer_X* GetOnlinePlayer();
	UCarColorSet_TA* GetColorPalette();

	// custom
	USeqAct_SetStadiumTeamColors_TA* getSetStadiumTeamColorsSeqAct();
	AGameEvent_Team_TA*              getTeamGameEvent();
	APlayerController*               getPlayerController();
	AGFxHUD_TA*                      getHUD();
	AGameEvent_TA*                   getGameEvent();
	UMatchType_TA*                   getMatchType();

	// misc
	std::string fullNameWithoutClass(UObject* obj);
	void spawnNotification(const std::string& title, const std::string& content, float duration, bool log = false);
};


extern class InstancesComponent Instances;