#pragma once
#include "Component.hpp"

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

struct TexData
{
	fs::path           imgPath;
	UTexture2DDynamic* texture = nullptr;
};

struct BallSkinData
{
	std::string                              name; // might not be necessary if BallSkinData are stored in a map... name can be the key
	fs::path                                 jsonFile;
	std::unordered_map<std::string, TexData> paramTextures;
	bool                                     bTexturesLoaded = false;

	bool fromJson(
	    const std::string& skinName, const json& j, const fs::path& jsonFile); // skinName param might not be necessary if we using a map
	// json toJson() const; // we aint creating JSON from the plugin
	bool loadTextures();
	void clearLoadedTextures();
};

class TexturesComponent : Component<TexturesComponent>
{
	friend struct BallSkinData;

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
	void searchForBallSkins();

private:
	// cvar values
	std::shared_ptr<const bool>  m_pluginEnabled; // from main plugin class
	std::shared_ptr<bool>        m_clearUnusedTexturesOnLoadingScreens = std::make_shared<bool>(false);
	std::shared_ptr<std::string> m_selectedTextureStr                  = std::make_shared<std::string>("");

	// flags
	bool m_ballDissolveChanged = false;

	// values
	// UMaterialInstanceConstant* m_ballDissolveMIC = nullptr; // storing raw pointers aint it fam

	fs::path                                      m_ballTexturesFolder;
	std::unordered_map<std::string, BallSkinData> m_savedTextureData; // name of texture --> all data related to the texture
	                                                                  // std::unordered_map<std::string, TArray<uint8_t>> savedImgBytes;

private:
	BallSkinData*              getSelectedSkin(); // will load/create the skin's textures if they havent alr been loaded/created
	void                       applySelectedSkinToAllBalls();
	void                       applySkinToBall(ABall_TA* ball, const BallSkinData& skin);
	void                       applySkinToBallArchetype(ABall_TA* arch, const BallSkinData& skin);
	void                       applySkinToBallAndArchetype(ABall_TA* ball, const BallSkinData& skin);
	void                       applySkinToBallDissolveMIC(const BallSkinData& skin);
	void                       clearCreatedTextures();
	void                       clearUnusedCreatedTextures();
	UMaterialInstanceConstant* getBallDissolveMIC();
	void                       createSkinJsonFile(const SkinJsonDataForImgui& skin);

	static bool               getImgBytes(const fs::path& imagePath, TArray<uint8_t>& outBytes);
	static UTexture2DDynamic* imgPathToTexture(const fs::path& imagePath, bool markInvincible = true);

	// gui
private:
	void display_skinDropdown();
	void display_skinJsonCreator();

public:
	void display();
};

extern class TexturesComponent Textures;