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
}

void TexturesComponent::initPaths()
{
	fs::path dataFolderPath = gameWrapper->GetDataFolder();

	if (!fs::exists(dataFolderPath))
	{
		LOGERROR("Unable to resolve bakkesmod data folder path");
		return;
	}

	m_acBallTexturesFolder = dataFolderPath / "acplugin" / "BallTextures";

	if (!fs::exists(m_acBallTexturesFolder))
	{
		LOGERROR("AlphaConsole BallTextures folder doesn't exist!");
		return;
	}
}

void TexturesComponent::initCvars()
{
	// bools
	registerCvar_bool(Cvars::clearUnusedTexturesOnLoading, false).bindTo(m_clearUnusedTexturesOnLoadingScreens);
}

void TexturesComponent::initHooks()
{
	auto clearUnusedTextures = [this](std::string event)
	{
		DEBUGLOG("[HOOK]: {}", event);

		if (!*m_pluginEnabled || !*m_clearUnusedTexturesOnLoadingScreens)
			return;

		runCommand(Commands::clearUnusedSavedTextures);
	};
	hookEventPost(Events::EngineShare_X_EventPreLoadMap, clearUnusedTextures);
	hookEventPost(Events::EngineShare_X_EventPreLoadMap, clearUnusedTextures);

	// the super hook... even fires when skipping around in a replay
	hookWithCallerPost(Events::FXActor_Ball_TA_StartBallFadeIn, [this](ActorWrapper Caller, void* Params, std::string event)
	{
		if (gameWrapper->IsInFreeplay())
			return;

		LOG("[HOOK POST]: {}", event);
		if (!*m_pluginEnabled)
			return;

		auto* caller = reinterpret_cast<AFXActor_Ball_TA*>(Caller.memory_address);
		if (!validUObject(caller))
			return;

		auto* ball = caller->Ball;
		if (!validUObject(ball))
			return;

		auto* skin = getAcSelectedTex();
		if (!skin)
			return;

		m_currentlyApplyingTexture = true;
		applySkinToBallAndArchetype(ball, *skin);
		applySkinToBallDissolveMIC(*skin);
		m_currentlyApplyingTexture = false;
	});

	// hooks with caller
	auto handleSetTexParamVal = [this](ActorWrapper Caller, void* Params, ...)
	{
		if (!*m_pluginEnabled || m_currentlyApplyingTexture)
			return;

		auto* mi = reinterpret_cast<UMaterialInstance*>(Caller.memory_address);
		if (!validUObject(mi) || !mi->IsA<UMaterialInstance>())
			return;

		UMaterialInterface* parent = mi->Parent;
		if (!validUObject(parent) || !parent->IsA<UMaterialInterface>())
			return;

		auto* params = reinterpret_cast<UMaterialInstance_execSetTextureParameterValue_Params*>(Params);
		if (!params)
			return;

		handleSetTexParamValue(mi, parent, params);
	};
	hookWithCaller(Events::MI_SetTextureParameterValue, handleSetTexParamVal);
	hookWithCaller(Events::MIC_SetTextureParameterValue, handleSetTexParamVal);
}

void TexturesComponent::initCommands()
{
	registerCommand(Commands::applyTexture, [this](...)
	{
		if (!*m_pluginEnabled)
			return;

		m_currentlyApplyingTexture = true;
		applySelectedSkinToAllBalls();
		m_currentlyApplyingTexture = false;
	});

	registerCommand(Commands::clearSavedTextures, [this](...)
	{
		if (!*m_pluginEnabled)
			return;

		clearSavedTextures();
	});

	registerCommand(Commands::clearUnusedSavedTextures, [this](...)
	{
		if (!*m_pluginEnabled)
			return;

		auto acSelectedTexture_cvar = _globalCvarManager->getCvar(Cvars::acSelectedTexture);
		if (!acSelectedTexture_cvar)
		{
			LOGERROR("Unable to access cvar: {}", Cvars::acSelectedTexture);
			return;
		}

		clearUnusedSavedTextures(acSelectedTexture_cvar.getStringValue());
	});
}



// ##############################################################################################################
// ###############################################    FUNCTIONS    ##############################################
// ##############################################################################################################

void TexturesComponent::applySelectedSkinToAllBalls()
{
	auto* skin = getAcSelectedTex();
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

void TexturesComponent::applySkinToBallAndArchetype(ABall_TA* ball, const BallTextureData& skin)
{
	applySkinToBall(ball, skin);
	applySkinToBallArchetype(InstancesComponent::getArchetype<ABall_TA>(ball), skin);
}

void TexturesComponent::applySkinToBallDissolveMIC(const BallTextureData& skin)
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
	for (const auto& [paramName, tex] : skin.textures)
	{
		if (!validUObject(tex))
			continue;

		FName param{std::wstring(paramName.begin(), paramName.end()).c_str()};

		UTexture* existingTex = nullptr;
		if (!m_ballDissolveMIC->GetTextureParameterValue(param, existingTex))
		{
			LOGERROR("Couldn't get {} texture param value from ball dissolve MIC", paramName);
			continue;
		}
		if (existingTex == tex)
		{
			LOG("Custom {} texture already exists on ball dissolve MIC", paramName);
			continue;
		}

		m_ballDissolveMIC->SetTextureParameterValue(param, tex);
		LOG("Successfully applied {} texture to ball dissolve MIC: {}", paramName, m_ballDissolveMIC->GetName());
	}
}

void TexturesComponent::applySkinToBall(ABall_TA* ball, const BallTextureData& skin)
{
	if (!validUObject(ball))
		return;

	UStaticMeshComponent* mesh = ball->StaticMesh;
	if (!validUObject(mesh))
	{
		LOGERROR("ball->StaticMesh is invalid");
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

	// DEBUGLOG("Finna apply texture to ball mesh: (ball {}) (mesh {}) (MIC {}) {}",
	//     Format::ToHexString(mic),
	//     Format::ToHexString(mesh),
	//     Format::ToHexString(ball),
	//     mesh->GetFullName());

	for (const auto& [paramName, tex] : skin.textures)
	{
		if (!validUObject(tex))
			continue;

		FName param{std::wstring(paramName.begin(), paramName.end()).c_str()};

		UTexture* existingTex = nullptr;
		if (!mic->GetTextureParameterValue(param, existingTex))
		{
			LOGERROR("Couldn't get {} texture param value from ball mesh MIC", paramName);
			continue;
		}
		if (existingTex == tex)
		{
			LOG("Custom {} texture already exists on ball mesh MIC", paramName);
			continue;
		}

		mic->SetTextureParameterValue(param, tex);
		LOG("Successfully applied {} texture to ball mesh MIC: {}", paramName, mesh->GetFullName());
	}
}

void TexturesComponent::applySkinToBallArchetype(ABall_TA* arch, const BallTextureData& skin)
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
	for (const auto& [paramName, tex] : skin.textures)
	{
		if (!validUObject(tex))
			continue;

		FName param{std::wstring(paramName.begin(), paramName.end()).c_str()};

		UTexture* existingTex = nullptr;
		if (!mic->GetTextureParameterValue(param, existingTex))
		{
			LOGERROR("Couldn't get {} texture param value from ball archetype MIC", paramName);
			continue;
		}
		if (existingTex == tex)
		{
			LOG("Custom {} texture already exists on ball archetype MIC", paramName);
			continue;
		}

		mic->SetTextureParameterValue(param, tex);
		LOG("Successfully applied {} texture to ball archetype mesh: {}", paramName, mesh->GetFullName());
	}
}

BallTextureData* TexturesComponent::getAcSelectedTex(bool loadTextures)
{
	auto acSelectedTexture_cvar = getCvar(Cvars::acSelectedTexture);
	if (!acSelectedTexture_cvar)
	{
		LOGERROR("Unable to access AlphaConsole cvar \"{}\"", Cvars::acSelectedTexture);
		LOG("Make sure AlphaConsole is enabled!");
		return nullptr;
	}

	std::string acTexStr = acSelectedTexture_cvar.getStringValue();
	auto        it       = m_textureCache.find(acTexStr);
	if (it == m_textureCache.end())
	{
		if (createBallTexData(acTexStr, loadTextures))
			return &m_textureCache[acTexStr];
		else
			return nullptr;
	}
	else
		return &it->second;
}

void TexturesComponent::updateBallDissolveMIC()
{
	LOG("Searching GObjects for ball dissolve MIC...");

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

// attempts to create BallTextureData and add it to m_textureCache for given AC selected texture string
bool TexturesComponent::createBallTexData(const std::string& acSelectedTexStr, bool loadTextures)
{
	BallTextureData data;
	if (!getInfoFromCvarStr(acSelectedTexStr, data.cvarTextureInfo))
			return false;

	auto jsonData = getJsonFromFile(data.cvarTextureInfo.textureName, data.cvarTextureInfo.jsonFile);
	if (!jsonData.has_value())
	{
		LOGERROR("No JSON data for \"{}\"", acSelectedTexStr);
		return false;
	}

	LOG("Got data for \"{}\" texture from file: {}", data.cvarTextureInfo.textureName, data.cvarTextureInfo.jsonFile.string());

	for (const auto& [paramName, relativeImgPathStr] : jsonData.value().items())
	{
		if (relativeImgPathStr.empty())
			continue;

		// resolve full image path
		fs::path fullPath = data.cvarTextureInfo.jsonFile.parent_path() / relativeImgPathStr;
		fullPath          = fs::weakly_canonical(fullPath); // make the path absolute (resolves ".." and ".")

		data.imgPaths[paramName] = fullPath;

		if (loadTextures)
		{
			// just create the texture.. bc this func is meant to create a new BallTextureData obj anyways, no point in trying to search for
			// an existing one
			data.textures[paramName] = imgPathToTexture(fullPath);
		}
	}

	m_textureCache[acSelectedTexStr] = data;
	return true;
}

std::optional<json> TexturesComponent::getJsonFromFile(const std::string& key, const fs::path& jsonFile)
{
	if (!fs::exists(jsonFile))
	{ 
		LOGERROR("JSON file doesn't exist: {}", jsonFile.string());
		return std::nullopt;
	}

	// read json
	json j;
	std::ifstream file(jsonFile);
	file >> j;

	if (j.contains(key))
	{
		auto& obj = j.at(key);
		if (obj.contains("Params"))
			return obj.at("Params");
		else 
		{
			LOGERROR("\"{}\" JSON object doesn't have a \"Params\" key");
			return std::nullopt;
		}
	}
	else
	{
		LOGERROR("JSON file doesn't have a \"{}\" key: ", key, jsonFile.string());
		return std::nullopt;
	}
}

void TexturesComponent::clearSavedTextures(bool onlyClearActualTextures)
{
	// mark all texture instances to be destroyed by GC
	for (auto& [texName, texData] : m_textureCache)
	{
		for (auto& [param_name, texture] : texData.textures)
			Instances.MarkForDestroy(texture);

		if (onlyClearActualTextures)
			texData.textures.clear();
	}

	if (!onlyClearActualTextures)
		m_textureCache.clear();	// yeet everything
}

void TexturesComponent::clearUnusedSavedTextures(const std::string& acSelectedTexStr, bool onlyClearActualTextures)
{
	BallTextureData currentTexData;
	bool found = false;

	for (auto& [name, texData] : m_textureCache)
	{
		if (!found && name == acSelectedTexStr)
		{
			currentTexData = texData;
			found = true;
			continue;
		}

		// mark all unused texture instances to be destroyed by GC
		for (auto& [param_name, texture] : texData.textures)
			Instances.MarkForDestroy(texture);

		if (onlyClearActualTextures)
			texData.textures.clear();
	}

	if (!onlyClearActualTextures)
	{
		m_textureCache.clear();		// yeet everything
		if (found)
			m_textureCache[acSelectedTexStr] = currentTexData;
	}
}

void TexturesComponent::handleSetTexParamValue(UMaterialInstance* mi, UMaterialInterface* parent, UMaterialInstance_execSetTextureParameterValue_Params* params)
{
	if (!mi || !parent || !params)
		return;

	std::string parentName = parent->GetFullName();
	if (parentName.find("Ball_") == std::string::npos) // exit if the texture isnt being applied to a ball 
		return;

	LOG("SetTexParamVal called on ball: {}", parentName);
	std::string paramName  = params->ParameterName.ToString();
	bool        inFreeplay = gameWrapper->IsInFreeplay();

	auto* skin = getAcSelectedTex(!inFreeplay); // only create new textures if not in freeplay
	if (!skin)
		return;
	auto& texMap = skin->textures;

	if (!inFreeplay) // intercept & change texture if in game (or just not in freeplay)
	{
		if (auto it = texMap.find(paramName); it != texMap.end())
		{
			params->Value = it->second;
			LOG("Intercepted & changed the \"{}\" ball texture that was being applied in a match . . .", paramName);
		}

		m_currentlyApplyingTexture = true;
		applySkinToBallDissolveMIC(*skin);
		m_currentlyApplyingTexture = false;
	}
	else // save texture, only if it's being applied in freeplay
	{
		std::string imgExt = skin->getImgExtension(paramName);
		DEBUGLOG("Image extension: {}", imgExt);
		if (imgExt.empty()) 
			return;
		
		// only save texture if it's not a JPEG (bc AC applies default texture for JPEG images)
		if (imgExt != ".jpg" && imgExt != ".jpeg")
		{
			if (!validUObject(params->Value) || !params->Value->IsA<UTexture2DDynamic>())
			{
				LOGERROR("UTexture* from SetTextureParameterValue param is invalid");
				return;
			}
			auto* tex = static_cast<UTexture2DDynamic*>(params->Value);

			if (auto it = texMap.find(paramName); it == texMap.end())
			{
				texMap[paramName] = tex;
				LOG("Cached the \"{}\" texture from SetTextureParameterValue", paramName);
			}
			else if (it->second != tex) // only cleanup old texture if it's not the same one in params
			{
				Instances.MarkForDestroy(it->second); // cleanup old texture 
				it->second = tex; // overwrite old (finna be destroyed) texture with new one
				LOG("Cached the \"{}\" texture from SetTextureParameterValue", paramName);
			}
		}
		else // if image is JPEG, manually load/apply the texture (bc alphaconsole wont do it)
		{
			loadTexturesForExistingData(*skin);

			if (auto it = texMap.find(paramName); it != texMap.end())
			{
				params->Value = it->second;
				LOG("Intercepted & changed the \"{}\" JPEG texture that was being applied in freeplay...", paramName);
			}

			m_currentlyApplyingTexture = true;
			applySkinToBallDissolveMIC(*skin);
			m_currentlyApplyingTexture = false;
		}
	}
}

bool TexturesComponent::getInfoFromCvarStr(const std::string& acSelectedTexStr, CvarTextureInfo& outCvarInfo)
{
	/*
		Format for acplugin_balltexture_selectedtexture cvar (AC 2.0):
		"subfolder1/subfolder2/json-file-name.json - textureName"
		"<relative path to json file json> - <texture name>"

		Regex patterns:
		R"(([^ ]+\.json) - (.+))"	<-- use only if relative path wont contain spaces (faster)
		R"((.+\.json) - (.+))"		<-- use if relative path might contain spaces (safer)
	*/
	std::smatch match;
	if (!std::regex_match(acSelectedTexStr, match, cvarRegex))
	{
		LOGERROR("{} regex match failed. Input: \"{}\", Pattern: \"{}\"", Cvars::acSelectedTexture, acSelectedTexStr, AC_CVAR_REGEX_PATTERN);
		return false;
	}

	std::string relPathToJson = match[1].str();	// "coolskins/fire/balls.json"
	std::string texName = match[2].str();		// "my cool skin"
	DEBUGLOG("relPathToJson: {}", relPathToJson);
	DEBUGLOG("texName: {}", texName);
	
	// make sure JSON file exists
	outCvarInfo.jsonFile = m_acBallTexturesFolder / relPathToJson;
	if (!fs::exists(outCvarInfo.jsonFile))
	{
		LOGERROR("JSON file doesn't exist: {}", outCvarInfo.jsonFile.string());
		return false;
	}

	outCvarInfo.textureName = texName;
	return true;
}

fs::path TexturesComponent::getImgPath(const std::string& acSelectedTextureStr, const std::string& param)
{
	fs::path imgPath;

	CvarTextureInfo texInfo{};
	if (!getInfoFromCvarStr(acSelectedTextureStr, texInfo))
		return imgPath;

	// get texture json data from json filepath + texture name
	auto jsonData = getJsonFromFile(texInfo.textureName, texInfo.jsonFile);
	LOG("Attempted to get json data from \"{}\" file using \"{}\" texture name...", texInfo.jsonFile.string(), texInfo.textureName);

	if (!jsonData.has_value())
	{
		LOGERROR("No JSON data for {}", acSelectedTextureStr);
		return imgPath;
	}

	// find image path in json data
	std::string relPathStr = jsonData.value()["Params"][param]; // if this don't work, it's user error (json file scuffed)
	imgPath = texInfo.jsonFile.parent_path() / relPathStr;
	imgPath = fs::weakly_canonical(imgPath); // Normalize the path (optional, resolves ".." and ".")

	return imgPath;
}



// ##############################################################################################################
// ##########################################    STATIC FUNCTIONS    ############################################
// ##############################################################################################################

void TexturesComponent::loadTexturesForExistingData(BallTextureData& data, bool forceNewTextures)
{
	if (!forceNewTextures)
	{
		for (const auto&[paramName, imgPath] : data.imgPaths)
		{
			// dont create new texture if a valid one alr exists in the map
			auto it = data.textures.find(paramName);
			if (it != data.textures.end() && validUObject(it->second))
				continue;

			data.textures[paramName] = imgPathToTexture(imgPath);
		}
	}
	else
	{
		// yeet any existing textures in textures map (mark for destroy first)
		for (const auto& [paramName, tex] : data.textures)
			Instances.MarkForDestroy(tex);
		data.textures.clear();

		// load all textures from img_paths map
		for (const auto& [paramName, imgPath] : data.imgPaths)
			data.textures[paramName] = imgPathToTexture(imgPath);
	}
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
		LOG("ERROR: UTexture2DDynamic* from ImageWrapper is invalid");
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
	FString pathFstr = FString::create(imgPath.string());

	int fileSize = UFileSystem::GetFileSize(pathFstr);
	LOG("UFileSystem::GetFileSize() fileSize: {}", fileSize);

	if (fileSize <= 1)
	{
		LOGERROR("\"{}\" is empty (not enough bytes)", imgPath.string());
		return false;
	}
	if (!UFileSystem::LoadFileToBytes(pathFstr, 0, fileSize, outBytes))
	{
		LOGERROR("UFileSystem::LoadFileToBytes(...) returned false. Image might be invalid");
		return false;
	}

	LOG("TArray<uint8_t> image bytes has {} elements", outBytes.size());
	return true;
}



// ##############################################################################################################
// #########################################    DISPLAY FUNCTIONS    ############################################
// ##############################################################################################################

void TexturesComponent::display() 
{
	auto clearUnusedTexturesOnLoading_cvar = getCvar(Cvars::clearUnusedTexturesOnLoading);
	if (!clearUnusedTexturesOnLoading_cvar)
		return;

	GUI::Spacing(2);

	bool clearUnusedTexturesOnLoading = clearUnusedTexturesOnLoading_cvar.getBoolValue();
	if (ImGui::Checkbox("Clear inactive textures on loading screen", &clearUnusedTexturesOnLoading))
	{
		clearUnusedTexturesOnLoading_cvar.setValue(clearUnusedTexturesOnLoading);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Can save memory if you dont switch textures often. Otherwise, leave it unchecked to help reduce lag when "
		                  "switching between textures");
	}

	GUI::Spacing(8);

	if (ImGui::Button("Clear all saved textures"))
	{
		GAME_THREAD_EXECUTE(
			runCommand(Commands::clearSavedTextures);
		);
	}

	GUI::Spacing(2);

	if (ImGui::Button("Clear unused saved textures"))
	{
		GAME_THREAD_EXECUTE(
			runCommand(Commands::clearUnusedSavedTextures);
		);
	}

	GUI::Spacing(4);

	uint8_t num_saved_texture_sets = 0;

	// list of saved texture info
	if (!m_textureCache.empty())
	{
		if (ImGui::CollapsingHeader("saved texture data"))
		{
			GUI::ScopedIndent indent{};

			GUI::Spacing(2);

			for (const auto& [textureName, tex_data] : m_textureCache)
			{
				if (!tex_data.textures.empty())
					num_saved_texture_sets++;

				if (ImGui::CollapsingHeader(textureName.c_str()))
				{
					GUI::ScopedIndent ok{50.0f};

					for (const auto& [param_name, img_path] : tex_data.imgPaths)
					{
						GUI::ScopedID id{&img_path};

						std::string txt = param_name + ":\t" + img_path.string();
						ImGui::Text(txt.c_str());

						GUI::SameLineSpacing_relative(20);

						if (ImGui::Button("Open"))
							Files::OpenFolder(img_path.parent_path());
					}
				}

				ImGui::Spacing();
			}

			GUI::Spacing(2);

			ImGui::Text("Cached ball textures: %d", num_saved_texture_sets);
		}
	}

	GUI::Spacing(2);
}


class TexturesComponent Textures{};