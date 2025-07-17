#include "pch.h"
#include "Textures.hpp"


// ##############################################################################################################
// ###############################################    INIT    ###################################################
// ##############################################################################################################

void TexturesComponent::initialize(const std::shared_ptr<GameWrapper>& gw, const std::shared_ptr<const bool>& enabledFlag)
{
	gameWrapper = gw;
	m_pluginEnabled = enabledFlag;

	initPaths();
	initCvars();
	initHooks();
	initCommands();
	searchForBallSkins();
}

void TexturesComponent::initPaths()
{
	fs::path dataFolderPath = gameWrapper->GetDataFolder();

	if (!fs::exists(dataFolderPath))
	{
		LOG("[ERROR] Unable to resolve bakkesmod data folder path");
		return;
	}

	m_ballTexturesFolder = dataFolderPath / "CustomBallOnline" / "BallTextures";

	if (!fs::exists(m_ballTexturesFolder))
	{
		if (fs::create_directories(m_ballTexturesFolder)) // creates sub folders as well. noice
			LOG("Path didn't exist, so it was created: {}", m_ballTexturesFolder.string());
		else
			LOGERROR("Unable to create directory: {}", m_ballTexturesFolder.string());
	}
}

void TexturesComponent::initCvars()
{
	// bools
	registerCvar_bool(Cvars::clearUnusedTexturesOnLoading, false).bindTo(m_clearUnusedTexturesOnLoadingScreens);

	// strings
	registerCvar_string(Cvars::selectedTexture, "my_ball_skins.json - sconer").bindTo(m_selectedTextureStr); // put back to default "", after done testing
}

void TexturesComponent::initHooks()
{
	hookEventPost(Events::EngineShare_X_EventPreLoadMap, [this](std::string event)
	{
		DEBUGLOG("[HOOK]: {}", event);

		if (!*m_pluginEnabled || !*m_clearUnusedTexturesOnLoadingScreens)
			return;

		runCommand(Commands::clearUnusedSavedTextures);
	});

	hookEventPost(Events::EngineShare_X_EventPreLoadMap, [this](std::string event)
	{
		DEBUGLOG("[HOOK]: {}", event);

		if (!*m_pluginEnabled || !*m_clearUnusedTexturesOnLoadingScreens)
			return;

		runCommand(Commands::clearUnusedSavedTextures);
	});

	// this exists bc not sure if FXActor_Ball_TA::StartBallFadeIn will be called when replacing a bot
	hookEventPost(Events::GFxHUD_TA_HandleReplaceBot, [this](std::string event)
	{
		//return;

		LOG("[HOOK]: {}", event);
		if (!*m_pluginEnabled)
			return;

		if (gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay())
			return;

		runCommand(Commands::applyTexture);
	});

	// might not be necessary given the FXActor_Ball_TA::StartBallFadeIn super hook
	// ... but still enabled bc not sure if FXActor_Ball_TA::StartBallFadeIn is called in casual intermission lobby
	// and getting in a casual intermission lobby is hard to test. unless there's a way to do it manually
	hookEventPost(Events::PlayWithBall_BeginState, [this](std::string event)
	{
		LOG("[HOOK]: {}", event);
		if (!*m_pluginEnabled)
			return;

		runCommand(Commands::applyTexture);
	});

	// super hook... even gets fired when skipping to different times in a replay
	hookWithCallerPost(Events::FXActor_Ball_TA_StartBallFadeIn, [this](ActorWrapper Caller, void* Params, std::string event)
	{
		LOG("[HOOK POST]: {}", event);
		if (!*m_pluginEnabled)
			return;

		auto* caller = reinterpret_cast<AFXActor_Ball_TA*>(Caller.memory_address);
		if (!validUObject(caller))
			return;

		auto* ball = caller->Ball;
		if (!validUObject(ball))
			return;

		auto* skin = getSelectedSkin();
		if (!skin)
			return;

		applySkinToBallAndArchetype(ball, *skin);
		applySkinToBallDissolveMIC(*skin);
	});
}

void TexturesComponent::initCommands()
{
	registerCommand(Commands::applyTexture, [this](...)
	{
		if (!*m_pluginEnabled)
			return;

		applySelectedSkinToAllBalls();
	});

	registerCommand(Commands::clearSavedTextures, [this](...)
	{
		if (!*m_pluginEnabled)
			return;

		clearCreatedTextures();
	});

	registerCommand(Commands::clearUnusedSavedTextures, [this](...)
	{
		if (!*m_pluginEnabled)
			return;

		clearUnusedCreatedTextures();
	});
}

// search for ball skins in BallTextures folder
void TexturesComponent::searchForBallSkins()
{
	// yeet old data
	for (auto& [skinName, data] : m_savedTextureData)
		data.clearLoadedTextures();
	m_savedTextureData.clear();

	try
	{
		for (const auto& entry : fs::recursive_directory_iterator(m_ballTexturesFolder))
		{
			if (!entry.is_regular_file() || Format::ToLower(entry.path().extension().string()) != ".json")
				continue;

			json data = Files::get_json(entry.path());
			if (data.empty() || !data.is_object())
				continue;

			std::error_code ec;
			fs::path        relPathToJsonFile = fs::relative(entry.path(), m_ballTexturesFolder, ec);
			if (ec)
			{
				LOGERROR("Cannot get relative path to JSON file: {}", ec.message());
				continue;
			}

			for (const auto& [name, obj] : data.items())
			{
				BallSkinData skin{};
				if (!skin.fromJson(name, obj, entry.path()))
					continue;

				std::string keyName = std::format("{} - {}", relPathToJsonFile.string(), name);
				m_savedTextureData[keyName] = skin;
			}
		}
	}
	catch (const fs::filesystem_error& e)
	{
		LOGERROR("Unable access or parse files in directory: {}", m_ballTexturesFolder.string());
		return;
	}

	GAME_THREAD_EXECUTE(
		std::string resultMsg = std::format("Found {} ball skins", m_savedTextureData.size());
	    Instances.spawnNotification("Custom Ball Online", resultMsg, 3.0f);
	    LOG(resultMsg);
	);
}



// ##############################################################################################################
// ###############################################    FUNCTIONS    ##############################################
// ##############################################################################################################

BallSkinData* TexturesComponent::getSelectedSkin()
{
	auto it = m_savedTextureData.find(*m_selectedTextureStr);
	if (it == m_savedTextureData.end())
		return nullptr;

	auto& skin = it->second;

	if (!skin.bTexturesLoaded)
	{
		// load textures if they haven't already been loaded
		if (!skin.loadTextures())
		{
			LOGERROR("Unable to load textures for skin: \"{}\"", *m_selectedTextureStr);
			return nullptr;
		}
	}

	return &skin;
}

void TexturesComponent::applySelectedSkinToAllBalls()
{
	auto* skin = getSelectedSkin();
	if (!skin)
		return;

	// apply skin to ball dissolve MIC
	applySkinToBallDissolveMIC(*skin);

	// get game event
	auto* maybeEvent = Instances.getGameEvent();
	if (!validUObject(maybeEvent) || !maybeEvent->IsA<AGameEvent_Soccar_TA>())
	{
		LOGERROR("No valid game event found");
		return;
	}
	auto* gameEvent = static_cast<AGameEvent_Soccar_TA*>(maybeEvent);

	// apply skin to game balls & ball archetype
	LOG("Num game balls found: {}", gameEvent->GameBalls.size());
	for (ABall_TA* ball : gameEvent->GameBalls)
		applySkinToBall(ball, *skin);

	applySkinToBallArchetype(gameEvent->BallArchetype, *skin);
}

void TexturesComponent::applySkinToBall(ABall_TA* ball, const BallSkinData& skin)
{
	if (!validUObject(ball))
		return;

	UStaticMeshComponent* mesh = ball->StaticMesh;
	if (!validUObject(mesh))
	{
		LOG("WHOA BUDDY: ball->StaticMesh is invalid");
		return;
	}

	UMaterialInstanceConstant* mic = nullptr;
	if (mesh->Materials.empty())
		mic = mesh->ConditionalCreateMIC(0);
	else if (auto* existingMI = mesh->GetMaterial(0))
	{
		if (validUObject(existingMI) && existingMI->IsA<UMaterialInstanceConstant>())
			mic = static_cast<UMaterialInstanceConstant*>(existingMI);
	}
	if (!validUObject(mic))
	{
		LOGERROR("UMaterialInstanceConstant* (from mesh->ConditionalCreateMIC(0) or mesh->GetMaterial(0)) is invalid");
		return;
	}

	// apply texture(s)

/*
	// debug log... can eventually remove
	LOG("Finna apply texture to ball mesh: (ball {}) (mesh {}) (MIC {}) {}",
	Format::ToHexString(mic),
	Format::ToHexString(mesh),
	Format::ToHexString(ball),
	mesh->GetFullName());
*/

	for (const auto& [paramName, tex] : skin.paramTextures)
	{
		if (!validUObject(tex.texture))
			continue;

		FName param{std::wstring(paramName.begin(), paramName.end()).c_str()};

		UTexture* existingTex = nullptr;
		if (!mic->GetTextureParameterValue(param, existingTex))
		{
			LOGERROR("Couldn't get {} texture param value from ball mesh MIC", paramName);
			continue;
		}
		if (existingTex == tex.texture)
		{
			LOG("Custom {} texture already exists on ball mesh MIC", paramName);
			continue;
		}

		mic->SetTextureParameterValue(param, tex.texture);
		LOG("Successfully applied {} texture to ball mesh MIC: {}", paramName, mesh->GetFullName());
	}
}

void TexturesComponent::applySkinToBallArchetype(ABall_TA* arch, const BallSkinData& skin)
{
	if (!validUObject(arch))
		return;

	UStaticMeshComponent* mesh = arch->StaticMesh;
	if (!validUObject(mesh))
	{
		LOGERROR("UStaticMeshComponent* from arch->StaticMesh is invalid");
		return;
	}

	UMaterialInstanceConstant* mic = nullptr;
	if (mesh->Materials.empty())
		mic = mesh->ConditionalCreateMIC(0);
	else if (auto* existingMI = mesh->GetMaterial(0))
	{
		if (validUObject(existingMI) && existingMI->IsA<UMaterialInstanceConstant>())
			mic = static_cast<UMaterialInstanceConstant*>(existingMI);
	}
	if (!validUObject(mic))
	{
		LOGERROR("UMaterialInstanceConstant* (from mesh->ConditionalCreateMIC(0) or mesh->GetMaterial(0)) is invalid");
		return;
	}

	// apply texture(s)
	for (const auto& [paramName, tex] : skin.paramTextures)
	{
		if (!validUObject(tex.texture))
			continue;

		FName param{std::wstring(paramName.begin(), paramName.end()).c_str()};

		UTexture* existingTex = nullptr;
		if (!mic->GetTextureParameterValue(param, existingTex))
		{
			LOGERROR("Couldn't get {} texture param value from ball archetype MIC", paramName);
			continue;
		}
		if (existingTex == tex.texture)
		{
			LOG("Custom {} texture already exists on ball archetype MIC", paramName);
			continue;
		}

		mic->SetTextureParameterValue(param, tex.texture);
		LOG("Successfully applied {} texture to ball archetype mesh: {}", paramName, mesh->GetFullName());
	}
}

void TexturesComponent::applySkinToBallAndArchetype(ABall_TA* ball, const BallSkinData& skin)
{
	applySkinToBall(ball, skin);
	applySkinToBallArchetype(InstancesComponent::getArchetype<ABall_TA>(ball), skin);
}

void TexturesComponent::applySkinToBallDissolveMIC(const BallSkinData& skin)
{
	if (!validUObject(m_ballDissolveMIC))
	{
		updateBallDissolveMIC();
		if (!m_ballDissolveMIC)
		{
			LOGERROR("Unable to find ball dissolve MIC in GObjects");
			return;
		}
	}

	// apply texture(s)
	for (const auto& [paramName, tex] : skin.paramTextures)
	{
		if (!validUObject(tex.texture))
			continue;

		FName param{std::wstring(paramName.begin(), paramName.end()).c_str()};

		UTexture* existingTex = nullptr;
		if (!m_ballDissolveMIC->GetTextureParameterValue(param, existingTex))
		{
			LOGERROR("Couldn't get {} texture param value from ball dissolve MIC", paramName);
			continue;
		}
		if (existingTex == tex.texture)
		{
			LOG("Custom {} texture already exists on ball dissolve MIC", paramName);
			continue;
		}

		m_ballDissolveMIC->SetTextureParameterValue(param, tex.texture);
		LOG("Successfully applied {} texture to ball dissolve MIC: {}", paramName, m_ballDissolveMIC->GetName());
	}
}

// TODO: maybe perhaps put this it in Instances if it makes more sense
void TexturesComponent::updateBallDissolveMIC()
{
	for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; --i)
	{
		UObject* uObject = UObject::GObjObjects()->at(i);
		if (!validUObject(uObject) || !uObject->IsA<UMaterialInstanceConstant>())
			continue;

		if (uObject->GetName().find("BallDissolve_Soccar") != 0)
			continue;

		m_ballDissolveMIC = static_cast<UMaterialInstanceConstant*>(uObject);
		return;
	}

	m_ballDissolveMIC = nullptr;
}

// #define USE_BM_METHOD
UTexture2DDynamic* TexturesComponent::imgPathToTexture(const fs::path& imgPath, bool markInvincible)
{
#ifdef USE_BM_METHOD

	// ez BM method
	auto               image     = ImageWrapper(imgPath, true);
	UTexture2DDynamic* customTex = reinterpret_cast<UTexture2DDynamic*>(image.GetCanvasTex());
	if (!validUObject(customTex))
	{
		LOGERROR("UTexture2DDynamic* from ImageWrapper is invalid");
		return nullptr;
	}

	return customTex;

#else

	// RLSDK method
	if (!fs::exists(imgPath))
	{
		LOGERROR("Path doesn't exist: \"{}\"", imgPath.string());
		return nullptr;
	}

	TArray<uint8_t> imgBytes{};
	if (!getImgBytes(imgPath, imgBytes))
		return nullptr;

	UTexture2DDynamic* customTex = Instances.CreateInstance<UTexture2DDynamic>();
	if (!customTex)
	{
		LOGERROR("Failed to create UTexture2DDynamic instance");
		return nullptr;
	}

	/*
	    // image color fixes for new textures
	    custom_texture->RGBE = true;
	    custom_texture->SRGB = false;
	    custom_texture->bIsResolveTarget = false;
	*/

	customTex->CompressionFullDynamicRange = true;
	customTex->CompressionSettings         = ETextureCompressionSettings::TC_NormalmapUncompressed;
	customTex->Init(50, 50, EPixelFormat::PF_A8R8G8B8, false); // <---- "false" arg is crucial to prevent scuffed colors

	LOG("Loading {} bytes to your newly created texture...", imgBytes.size());

	// check file extension and update texture accordingly
	std::string imgExtension = Format::ToLower(imgPath.extension().string());

	if (imgExtension == ".png")
	{
		customTex->UpdateMipFromPNG(0, imgBytes);
	}
	else if (imgExtension == ".jpg" || imgExtension == ".jpeg")
	{
		customTex->UpdateMipFromJPEG(0, imgBytes);
	}
	else
	{
		LOGERROR("Invalid image format. Please make sure your image is a 32-bit PNG or JPEG");
		return nullptr;
	}

	if (markInvincible)
		Instances.MarkInvincible(customTex);

	return customTex;

#endif // USE_BM_METHOD
}

bool TexturesComponent::getImgBytes(const fs::path& imgPath, TArray<uint8_t>& outBytes)
{
	// if (savedImgBytes.contains(imagePath))
	//{
	//	LOG("yeee we already have it :))");
	//	return savedImgBytes[imagePath];
	// }

	FString pathFstr = FString::create(imgPath.string());

	int fileSize = UFileSystem::GetFileSize(pathFstr);
	LOG("UFileSystem::GetFileSize() fileSize: {}", fileSize);

	if (fileSize <= 1)
	{
		LOGERROR("\"{}\" is empty (not enough bytes)", imgPath.string());
		return false;
	}

	if (UFileSystem::LoadFileToBytes(pathFstr, 0, fileSize, outBytes))
	{
		LOG("TArray<uint8_t> image bytes has {} elements", outBytes.size());
		return true;
	}
	else
	{
		LOGERROR("UFileSystem::LoadFileToBytes(...) returned false. Image might be invalid", outBytes.size());
		return false;
	}
}

void TexturesComponent::clearCreatedTextures()
{
	for (auto& [name, skin] : m_savedTextureData)
	{
		// mark all textures to be destroyed by GC
		for (auto& [param_name, tex] : skin.paramTextures)
			Instances.SimpleMarkForDestroy(tex.texture);

		// clear shit
		skin.paramTextures.clear();
		skin.bTexturesLoaded = false;
	}
}

void TexturesComponent::clearUnusedCreatedTextures()
{
	BallSkinData currentSkin{};
	bool         currentSkinFound = false;

	for (auto& [name, skin] : m_savedTextureData)
	{
		if (name == *m_selectedTextureStr)
		{
			currentSkin = skin;
			currentSkinFound = true;
			continue;
		}

		// mark all textures to be destroyed by GC
		for (auto& [param_name, tex] : skin.paramTextures)
			Instances.SimpleMarkForDestroy(tex.texture);
	}

	m_savedTextureData.clear();
	if (currentSkinFound)
		m_savedTextureData[*m_selectedTextureStr] = currentSkin;
}



// ##############################################################################################################
// #########################################    DISPLAY FUNCTIONS    ############################################
// ##############################################################################################################

void TexturesComponent::display() 
{
	auto clearUnusedTexturesOnLoading_cvar = getCvar(Cvars::clearUnusedTexturesOnLoading);
	if (!clearUnusedTexturesOnLoading_cvar)
	{
		ImGui::Text("wtf, \"%s\" cvar is null", Cvars::clearUnusedTexturesOnLoading.name);
		return;
	}

	bool clearUnusedTexturesOnLoading = clearUnusedTexturesOnLoading_cvar.getBoolValue();
	if (ImGui::Checkbox("Clear inactive textures on loading screen", &clearUnusedTexturesOnLoading))
		clearUnusedTexturesOnLoading_cvar.setValue(clearUnusedTexturesOnLoading);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Can save memory if you dont switch textures often. Otherwise, leave it unchecked to help reduce lag when "
		                  "switching between textures");

	GUI::Spacing(8);

	if (ImGui::Button("Open BallTextures folder"))
		Files::OpenFolder(m_ballTexturesFolder);

	GUI::Spacing(2);

	display_skinDropdown();

	GUI::Spacing(8);

	if (ImGui::Button("Clear all cached textures"))
	{
		GAME_THREAD_EXECUTE(
			runCommand(Commands::clearSavedTextures);
		);
	}

	ImGui::Spacing();

	if (ImGui::Button("Clear unused cached textures"))
	{
		GAME_THREAD_EXECUTE(
			runCommand(Commands::clearUnusedSavedTextures);
		);
	}

	GUI::Spacing(2);

	uint8_t numCachedSkins = 0;

	// list of saved texture info
	if (!Textures.m_savedTextureData.empty())
	{
		if (ImGui::CollapsingHeader("Cached texture data"))
		{
			GUI::ScopedIndent indent{};

			GUI::Spacing(2);

			for (const auto& [skinName, skinData] : m_savedTextureData)
			{
				if (skinData.paramTextures.empty() || !skinData.bTexturesLoaded)
					continue;

				GUI::ScopedID id{&skinData};
				numCachedSkins++;
				
				if (ImGui::CollapsingHeader(skinName.c_str()))
				{
					GUI::ScopedIndent ok{50.0f};

					for (const auto& [paramName, texData] : skinData.paramTextures)
					{
						GUI::ScopedID id{&texData};

						std::string txt = std::format("{}:\t{}", paramName, texData.imgPath.string());
						ImGui::Text(txt.c_str());

						GUI::SameLineSpacing_relative(20);

						if (ImGui::Button("Open"))
							Files::OpenFolder(texData.imgPath.parent_path());
					}
				}

				ImGui::Spacing();
			}

			GUI::Spacing(2);

			ImGui::Text("Cached ball skins: %d", numCachedSkins);
		}
	}

	GUI::Spacing(2);
}

void TexturesComponent::display_skinDropdown()
{
	auto selectedTexture_cvar = getCvar(Cvars::selectedTexture);
	if (!selectedTexture_cvar)
		return;

	char searchBuffer[128] = ""; // text buffer for search input

	if (ImGui::BeginSearchableCombo(
	        "Ball skins", selectedTexture_cvar.getStringValue().c_str(), searchBuffer, sizeof(searchBuffer), "search..."))
	{
		std::string searchQuery = Format::ToLower(searchBuffer); // convert search text to lower

		for (const auto& [name, skin] : m_savedTextureData)
		{
			GUI::ScopedID id{&skin};

			const std::string& skinName      = skin.name;
			const std::string  skinNameLower = Format::ToLower(skinName);

			if (!searchQuery.empty()) // only render option if there's text in search box & it matches the key name
			{
				if (skinNameLower.find(searchQuery) == std::string::npos)
					continue;

				if (ImGui::Selectable(skinName.c_str(), name == *m_selectedTextureStr))
				{
					selectedTexture_cvar.setValue(name);

					GAME_THREAD_EXECUTE(
						applySelectedSkinToAllBalls();
					);
				}
			}
			else // if there's no text in search box, render all possible key options
			{
				if (ImGui::Selectable(skinName.c_str(), name == *m_selectedTextureStr))
				{
					selectedTexture_cvar.setValue(name);

					GAME_THREAD_EXECUTE(
						applySelectedSkinToAllBalls();
					);
				}
			}
		}

		ImGui::EndCombo();
	}

	GUI::SameLineSpacing_relative(10);

	if (ImGui::Button("Refresh"))
		searchForBallSkins();
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Search BallTextures folder for skins\n\n(Also clears all cached textures)");

	ImGui::Text("%i skins", m_savedTextureData.size());
}



// ##############################################################################################################
// ###########################################    STRUCT FUNCTIONS    ###########################################
// ##############################################################################################################

bool BallSkinData::loadTextures()
{
	for (auto& [paramName, texData] : paramTextures)
	{
		if (validUObject(texData.texture))
			continue;

		auto* createdTex = TexturesComponent::imgPathToTexture(texData.imgPath);
		if (!createdTex)
			return false;

		texData.texture = createdTex;
	}

	bTexturesLoaded = true;
	return true;
}

void BallSkinData::clearLoadedTextures()
{
	for (auto& [paramName, tex] : paramTextures)
	{
		if (!tex.texture)
			continue;

		Instances.SimpleMarkForDestroy(tex.texture);
		tex.texture = nullptr; // overwrite with nullptr
	}

	bTexturesLoaded = false;
}

bool BallSkinData::fromJson(const std::string& skinName, const json& j, const fs::path& jsonFilePath)
{
	if (j.empty() || !j.is_object() || !j.contains("Params"))
	{
		LOG("ERROR: Invalid json data. Unable to populate BallSkinData instance");
		return false;
	}

	if (!fs::exists(jsonFilePath))
	{
		LOG("ERROR: Invalid filepath: {}", jsonFilePath.string());
		return false;
	}

	for (const auto& [paramName, relativeImgPath] : j["Params"].items()) // param name --> relative path to image file
	{
		if (relativeImgPath.empty())
			continue;

		// resolve full image path
		fs::path fullImgPath = jsonFilePath.parent_path() / relativeImgPath;
		fullImgPath          = fs::weakly_canonical(fullImgPath);

		// add to map
		paramTextures.emplace(paramName, TexData{fullImgPath, nullptr});
	}

	if (paramTextures.empty())
	{
		LOG("ERROR: No valid param textures found in JSON data.... It might be improperly configured");
		return false;
	}

	name     = skinName; 
	jsonFile = jsonFilePath;
	return true;
}


class TexturesComponent Textures{};