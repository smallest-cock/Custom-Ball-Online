#pragma once
#include "Component.hpp"
#include <regex>

struct SkinJsonDataForImgui
{
	char jsonFileName[128] = "";
	char skinName[128]     = "";
	char diffusePath[128]  = "";
	char normalPath[128]   = "";
	char maskPath[128]     = "";

	void clear();
};

struct SkinJsonData
{
	std::string jsonFileName;
	std::string skinName;
	std::string diffusePath;
	std::string normalPath;
	std::string maskPath;

	SkinJsonData(const SkinJsonDataForImgui& skin)
	    : jsonFileName(skin.jsonFileName), skinName(skin.skinName), diffusePath(skin.diffusePath), normalPath(skin.normalPath),
	      maskPath(skin.maskPath)
	{
	}

	bool validateData(const fs::path& ballTexFolder);
	json toJson() const;
};

struct CvarTextureInfo
{
	fs::path    jsonFile;
	std::string textureName;
};

struct BallTextureData
{
	CvarTextureInfo                                     cvarTextureInfo;
	std::unordered_map<std::string, fs::path>           imgPaths; // img_paths[param_name] ---> C:\users\.....\texture_image.jpg
	std::unordered_map<std::string, UTexture2DDynamic*> textures; // textures[param_name]	---> UTexture2DDynamic*

	std::string getImgExtension(const std::string& param_name)
	{
		std::string extension;

		if (auto it = imgPaths.find(param_name); it != imgPaths.end())
			extension = Format::ToLower(it->second.extension().string());

		return extension;
	}
};

class TexturesComponent : Component<TexturesComponent>
{
	friend struct BallTextureData;

public:
	TexturesComponent() {}
	~TexturesComponent() {}

	static constexpr std::string_view componentName = "Textures";
	void                              initialize(const std::shared_ptr<GameWrapper>& gw, const std::shared_ptr<const bool>& enabledFlag);

private:
	void initPaths();
	void initCvars();
	void initHooks();
	void initCommands();

private:
	// cvar values
	std::shared_ptr<const bool>  m_pluginEnabled; // from main plugin class
	std::shared_ptr<bool>        m_clearUnusedTexturesOnLoadingScreens = std::make_shared<bool>(false);
	std::shared_ptr<std::string> m_acSelectedTextureStr;

	// flags
	bool m_freeToSaveTextures       = true;
	bool m_currentlyApplyingTexture = false;

	// values
	fs::path                                         m_acBallTexturesFolder;
	std::unordered_map<std::string, BallTextureData> m_textureCache;
	// UMaterialInstanceConstant* m_ballDissolveMIC = nullptr;

	static constexpr auto          AC_CVAR_REGEX_PATTERN = R"((.+\.json) - (.+))";
	inline static const std::regex cvarRegex{AC_CVAR_REGEX_PATTERN};

private:
	// old existing funcs
	std::optional<json> getJsonFromFile(const std::string& key, const fs::path& jsonFile);
	fs::path            getImgPath(const std::string& acSelectedTextureStr, const std::string& paramName);
	bool                getInfoFromCvarStr(const std::string& acSelectedTextureStr, CvarTextureInfo& outCvarInfo);

	void handleSetTexParamValue(
	    UMaterialInstance* mi, UMaterialInterface* parent, UMaterialInstance_execSetTextureParameterValue_Params* params);
	void clearSavedTextures(bool onlyClearActualTextures = true);
	void clearUnusedSavedTextures(const std::string& currentACTexName, bool onlyClearActualTextures = true);

	static void               loadTexturesForExistingData(BallTextureData& data, bool forceNewTextures = false);
	static UTexture2DDynamic* imgPathToTexture(const fs::path& imagePath, bool markInvincible = true);
	static bool               getImgBytes(const fs::path& imagePath, TArray<uint8_t>& outBytes);

	// new funcs
	BallTextureData*           getAcSelectedTex(bool loadTextures = true);
	UMaterialInstanceConstant* getBallDissolveMIC();

	bool createBallTexData(const std::string& acSelectedTexStr, bool loadTextures);

	void applySelectedSkinToAllBalls();
	void applySkinToBall(ABall_TA* ball, const BallTextureData& skin);
	void applySkinToBallArchetype(ABall_TA* arch, const BallTextureData& skin);
	void applySkinToBallAndArchetype(ABall_TA* ball, const BallTextureData& skin);
	void applySkinToBallDissolveMIC(const BallTextureData& skin);
	void createSkinJsonFile(const SkinJsonDataForImgui& skin);

	// gui
private:
	void display_skinJsonCreator();

public:
	void display();
};

extern class TexturesComponent Textures;