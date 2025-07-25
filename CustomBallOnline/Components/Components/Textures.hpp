#pragma once
#include "Component.hpp"


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

	bool fromJson(const std::string& skinName, const json& j, const fs::path& jsonFile); // skinName param might not be necessary if we using a map
	//json toJson() const; // we aint creating JSON from the plugin
	bool loadTextures();
	void clearLoadedTextures();
};


class TexturesComponent : Component<TexturesComponent>
{
public:
	TexturesComponent() {}
	~TexturesComponent() {}

	static constexpr std::string_view componentName = "Textures";
	void initialize(const std::shared_ptr<GameWrapper>& gw, const std::shared_ptr<const bool>& enabledFlag);

private:
	void initPaths();
	void initCvars();
	void initHooks();
	void initCommands();
	void searchForBallSkins();
	void createCustomJsonFile(const std::string& name, const std::string& imageName, const std::string& fileName);

private:
	// cvar values
	std::shared_ptr<const bool>  m_pluginEnabled; // from main plugin class
	std::shared_ptr<bool>        m_clearUnusedTexturesOnLoadingScreens = std::make_shared<bool>(false);
	std::shared_ptr<std::string> m_selectedTextureStr                  = std::make_shared<std::string>("");

	// flags
	bool m_ballDissolveChanged = false;

	// values
	UMaterialInstanceConstant* m_ballDissolveMIC = nullptr;

	fs::path m_ballTexturesFolder;
	std::unordered_map<std::string, BallSkinData> m_savedTextureData; // name of texture --> all data related to the texture
	//std::unordered_map<std::string, TArray<uint8_t>> savedImgBytes;

private:
	BallSkinData* getSelectedSkin(); // will load/create the skin's textures if they havent alr been loaded/created
	void          applySelectedSkinToAllBalls();
	void          applySkinToBall(ABall_TA* ball, const BallSkinData& skin);
	void          applySkinToBallArchetype(ABall_TA* arch, const BallSkinData& skin);
	void          applySkinToBallAndArchetype(ABall_TA* ball, const BallSkinData& skin);
	void          applySkinToBallDissolveMIC(const BallSkinData& skin);
	void          clearCreatedTextures();
	void          clearUnusedCreatedTextures();
	void          updateBallDissolveMIC();

	static bool               getImgBytes(const fs::path& imagePath, TArray<uint8_t>& outBytes);

public:
	static UTexture2DDynamic* imgPathToTexture(const fs::path& imagePath, bool markInvincible = true);

public:
	// gui
	void display();
	void display_skinDropdown();
	void CreateJSONdisplay();
};


extern class TexturesComponent Textures;