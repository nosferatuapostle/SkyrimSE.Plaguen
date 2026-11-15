#include <algorithm>
#include <filesystem>
#include <string>
#include <system_error>
#include <SKSEMenuFramework.h>
#include <PrismaUI_API.h>

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef DEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = SKSE::log::log_directory();
	if (!path) {
		return false;
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef DEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	SKSE::log::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		SKSE::log::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_SSE_1_5_39) {
		SKSE::log::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

namespace RE
{
	void DebugNotification(const char* a_notification, const char* a_soundToPlay = (const char*)0, bool a_cancelIfAlreadyQueued = true)
	{
		using func_t = decltype(&DebugNotification);
		REL::Relocation<func_t> func{ RELOCATION_ID(52050, 52933) };
		return func(a_notification, a_soundToPlay, a_cancelIfAlreadyQueued);
	}
}

RE::GameSettingCollection* GSC = RE::GameSettingCollection::GetSingleton();

namespace FormLookup
{
	struct Key
	{
		RE::FormID localID{};
		std::string_view plugin{};
	};

	template <typename T>
	[[nodiscard]] T* Get(const Key& key) noexcept
	{
		static_assert(std::is_base_of_v<RE::TESForm, T>, "T must inherit from RE::TESForm");

		auto* dataHandler = RE::TESDataHandler::GetSingleton();

		if (!dataHandler) {
			return nullptr;
		}

		return dataHandler->LookupForm<T>(key.localID, key.plugin);
	}

	template <typename T>
	[[nodiscard]] T* Get(RE::FormID localID, std::string_view plugin) noexcept
	{
		return Get<T>({ localID, plugin });
	}
}

namespace Plugin
{
	inline constexpr std::string_view kSkyrim = "Skyrim.esm";
	inline constexpr std::string_view kDawnguard = "Dawnguard.esm";
}

namespace FormIDKeys
{
	namespace MGEF
	{
		inline constexpr FormLookup::Key kWarriorStone{ 0x000E5F4B, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kMageStone{ 0x000E5F48, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kThiefStone{ 0x000E5F44, Plugin::kSkyrim };
	}

	namespace RACE
	{
		inline constexpr FormLookup::Key kArgonian{ 0x13740, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kBreton{ 0x13741, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kDarkElf{ 0x13742, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kHighElf{ 0x13743, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kImperial{ 0x13744, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kKhajiit{ 0x13745, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kNord{ 0x13746, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kOrc{ 0x13747, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kRedguard{ 0x13748, Plugin::kSkyrim };
		inline constexpr FormLookup::Key kWoodElf{ 0x13749, Plugin::kSkyrim };

		inline constexpr FormLookup::Key kArgonianVampire{ 0x8883a, Plugin::kDawnguard };
		inline constexpr FormLookup::Key kBretonVampire{ 0x8883c, Plugin::kDawnguard };
		inline constexpr FormLookup::Key kDarkElfVampire{ 0x8883d, Plugin::kDawnguard };
		inline constexpr FormLookup::Key kHighElfVampire{ 0x88840, Plugin::kDawnguard };
		inline constexpr FormLookup::Key kImperialVampire{ 0x88844, Plugin::kDawnguard };
		inline constexpr FormLookup::Key kKhajiitVampire{ 0x88845, Plugin::kDawnguard };
		inline constexpr FormLookup::Key kNordVampire{ 0x88794, Plugin::kDawnguard };
		inline constexpr FormLookup::Key kOrcVampire{ 0xa82b9, Plugin::kDawnguard };
		inline constexpr FormLookup::Key kRedguardVampire{ 0x88846, Plugin::kDawnguard };
		inline constexpr FormLookup::Key kWoodElfVampire{ 0x88884, Plugin::kDawnguard };
	}
	
	//inline constexpr RE::FormID RACE_ARGONIAN = 79680;
	//inline constexpr RE::FormID RACE_BRETON = 79681;
	//inline constexpr RE::FormID RACE_DARKELF = 79682;
	//inline constexpr RE::FormID RACE_HIGHELF = 79683;
	//inline constexpr RE::FormID RACE_IMPERIAL = 79684;
	//inline constexpr RE::FormID RACE_KHAJIIT = 79685;
	//inline constexpr RE::FormID RACE_NORD = 79686;
	//inline constexpr RE::FormID RACE_ORC = 79687;
	//inline constexpr RE::FormID RACE_REDGUARD = 79688;
	//inline constexpr RE::FormID RACE_WOODELF = 79689;
	//inline constexpr RE::FormID RACE_ARGONIAN_VAMPIRE = 559162;
	//inline constexpr RE::FormID RACE_BRETON_VAMPIRE = 559164;
	//inline constexpr RE::FormID RACE_DARKELF_VAMPIRE = 559165;
	//inline constexpr RE::FormID RACE_HIGHELF_VAMPIRE = 559168;
	//inline constexpr RE::FormID RACE_IMPERIAL_VAMPIRE = 559172;
	//inline constexpr RE::FormID RACE_KHAJIIT_VAMPIRE = 559173;
	//inline constexpr RE::FormID RACE_NORD_VAMPIRE = 558996;
	//inline constexpr RE::FormID RACE_ORC_VAMPIRE = 688825;
	//inline constexpr RE::FormID RACE_REDGUARD_VAMPIRE = 559174;
	//inline constexpr RE::FormID RACE_WOODELF_VAMPIRE = 559236;
	
	//inline constexpr RE::FormID RACE_VAMPIRE_BEAST = 10298;
	//inline constexpr RE::FormID RACE_WEREWOLF_BEAST = 843140;

	//inline constexpr RE::FormID RACE_OLD = 425176;
	//inline constexpr RE::FormID RACE_NORD_CHILD = 181851;

	//inline constexpr RE::FormID RACE_CHICKEN = 692637;
	//inline constexpr RE::FormID RACE_COW = 321413;
	//inline constexpr RE::FormID RACE_DOG = 78318;
	//inline constexpr RE::FormID RACE_RABBIT = 449689;
	//inline constexpr RE::FormID RACE_HORSE = 910597;
	//inline constexpr RE::FormID RACE_DEER = 450201;
	//inline constexpr RE::FormID RACE_CRAB = 763205;
	//inline constexpr RE::FormID RACE_WOLF = 78346;
	//inline constexpr RE::FormID RACE_ICETROLL = 78342;
	//inline constexpr RE::FormID RACE_GIANT = 78329;
}

namespace
{
	struct Stats
	{
		float hp{};
		float mp{};
		float st{};
	};

	struct RaceData
	{
		Stats base;
		Stats growth;
	};

	struct RaceEntry
	{
		FormLookup::Key key;
		RaceData data;
	};

	constexpr RaceEntry kRaceTable[] = {
		{ FormIDKeys::RACE::kArgonian, { { 75, 75, 100 }, { 4, 2, 5 } } },
		{ FormIDKeys::RACE::kBreton, { { 50, 110, 90 }, { 2, 5, 4 } } },
		{ FormIDKeys::RACE::kDarkElf, { { 60, 95, 95 }, { 3, 5, 5 } } },
		{ FormIDKeys::RACE::kHighElf, { { 50, 125, 75 }, { 2, 6, 3 } } },
		{ FormIDKeys::RACE::kImperial, { { 70, 80, 100 }, { 3, 3, 5 } } },
		{ FormIDKeys::RACE::kKhajiit, { { 65, 70, 115 }, { 3, 2, 6 } } },
		{ FormIDKeys::RACE::kNord, { { 85, 60, 105 }, { 4, 2, 5 } } },
		{ FormIDKeys::RACE::kOrc, { { 90, 50, 110 }, { 5, 1, 5 } } },
		{ FormIDKeys::RACE::kRedguard, { { 80, 55, 115 }, { 4, 2, 5 } } },
		{ FormIDKeys::RACE::kWoodElf, { { 55, 90, 105 }, { 2, 3, 4 } } },

		{ FormIDKeys::RACE::kArgonianVampire, { { 75, 75, 100 }, { 5, 3, 6 } } },
		{ FormIDKeys::RACE::kBretonVampire, { { 50, 110, 90 }, { 3, 6, 5 } } },
		{ FormIDKeys::RACE::kDarkElfVampire, { { 60, 95, 95 }, { 4, 6, 6 } } },
		{ FormIDKeys::RACE::kHighElfVampire, { { 50, 125, 75 }, { 3, 7, 4 } } },
		{ FormIDKeys::RACE::kImperialVampire, { { 70, 80, 100 }, { 4, 4, 6 } } },
		{ FormIDKeys::RACE::kKhajiitVampire, { { 65, 70, 115 }, { 4, 3, 7 } } },
		{ FormIDKeys::RACE::kNordVampire, { { 85, 60, 105 }, { 5, 3, 6 } } },
		{ FormIDKeys::RACE::kOrcVampire, { { 90, 50, 110 }, { 6, 2, 6 } } },
		{ FormIDKeys::RACE::kRedguardVampire, { { 80, 55, 115 }, { 5, 3, 6 } } },
		{ FormIDKeys::RACE::kWoodElfVampire, { { 55, 90, 105 }, { 3, 4, 5 } } },
	};

	bool TryGetBaseValues(RE::TESRace* race, RaceData& outData)
	{
		for (const auto& entry : kRaceTable) {
			auto r = FormLookup::Get<RE::TESRace>(entry.key);
			if (r == race) {
				outData = entry.data;
				return true;
			}
		}

		return false;
	}

	void UpdateValues(RE::Actor* actor, const std::string debugMessage = "")
	{
		RaceData data;
		if (!TryGetBaseValues(actor->race, data)) {
			SKSE::log::warn("Unknown race 0x{:08X} - name: {}, race: {}", actor->race->formID, actor->GetDisplayFullName(),
				actor->race->GetFullName());
			return;
		}

		//auto warriorStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_WARRIOR_STONE);
		//auto mageStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_MAGE_STONE);
		//auto thiefStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_THIEF_STONE);
		auto warriorStone = FormLookup::Get<RE::EffectSetting>(FormIDKeys::MGEF::kWarriorStone);
		auto mageStone = FormLookup::Get<RE::EffectSetting>(FormIDKeys::MGEF::kMageStone);
		auto thiefStone = FormLookup::Get<RE::EffectSetting>(FormIDKeys::MGEF::kThiefStone);

		if (actor->HasMagicEffect(warriorStone)) {
			data.growth.hp += 1.0f;
		} else if (actor->HasMagicEffect(mageStone)) {
			data.growth.mp += 1.0f;
		} else if (actor->HasMagicEffect(thiefStone)) {
			data.growth.st += 1.0f;
		}

		auto level = actor->GetLevel();

		float armor = actor->GetActorValueMax(RE::ActorValue::kDamageResist);

		float hp = data.base.hp + (level - 1) * data.growth.hp + armor;
		float mp = data.base.mp + (level - 1) * data.growth.mp;
		float st = data.base.st + (level - 1) * data.growth.st;

		float oldHP = actor->GetBaseActorValue(RE::ActorValue::kHealth);
		float oldMP = actor->GetBaseActorValue(RE::ActorValue::kMagicka);
		float oldST = actor->GetBaseActorValue(RE::ActorValue::kStamina);

		actor->SetBaseActorValue(RE::ActorValue::kHealth, hp);
		actor->SetBaseActorValue(RE::ActorValue::kMagicka, mp);
		actor->SetBaseActorValue(RE::ActorValue::kStamina, st);

		actor->SetBaseActorValue(RE::ActorValue::kCarryWeight, 100 + data.base.hp);

		if (!debugMessage.empty()) {
			SKSE::log::info("{}", debugMessage);
			SKSE::log::info("Name: {}, Race: {}, {} lvl", actor->GetDisplayFullName(), actor->race->GetFullName(), level);
			SKSE::log::info("HEALTH: {} => {}", oldHP, hp);
			SKSE::log::info("MAGICKA: {} => {}", oldMP, mp);
			SKSE::log::info("STAMINA: {} => {}", oldST, st);
		}
	}

	static void UpdateValues(const std::string debugMessage = "")
	{
		UpdateValues(RE::PlayerCharacter::GetSingleton(), debugMessage);
	}
}

namespace PrismaUICode
{
	using PrismaInterface = PRISMA_UI_API::IVPrismaUI1;

	inline PrismaInterface* g_prismaUI = nullptr;
	inline PrismaView g_view = 0;

	inline bool g_viewReady = false;

	inline std::mutex g_viewMutex;
	inline std::mutex g_combatMutex;

	inline std::unordered_map<RE::FormID, RE::ACTOR_COMBAT_STATE> g_combatants;

	inline RE::ACTOR_COMBAT_STATE g_playerCombatState = RE::ACTOR_COMBAT_STATE::kNone;

	inline std::uint32_t g_lastLevel = 0;
	inline float g_lastXP = -1.0f;
	inline float g_lastRequiredXP = -1.0f;

	inline float g_resourceUpdateTimer = 0.0f;
	inline float g_targetUpdateTimer = 0.0f;
	inline std::array<float, 6> g_lastResources{ -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f };
	inline RE::FormID g_lastTargetID = 0;
	inline float g_lastTargetHealth = -1.0f;
	inline float g_lastTargetMaxHealth = -1.0f;

	inline std::atomic_bool g_viewHidden{ true };
	inline std::atomic_bool g_playerWorldReady{ false };

	inline std::atomic_uint32_t g_transientHideMenuCount{ 0 };

	inline float g_visibilityTimer = 0.0f;

	constexpr float kVisibilityCheckInterval = 0.5f;

	static constexpr std::array<std::string_view, 20> kHiddenMenus = { RE::InventoryMenu::MENU_NAME, RE::MagicMenu::MENU_NAME,
		RE::MapMenu::MENU_NAME, RE::StatsMenu::MENU_NAME, RE::JournalMenu::MENU_NAME, RE::TweenMenu::MENU_NAME,
		RE::ContainerMenu::MENU_NAME, RE::BarterMenu::MENU_NAME, RE::GiftMenu::MENU_NAME, RE::DialogueMenu::MENU_NAME,
		RE::LockpickingMenu::MENU_NAME, RE::BookMenu::MENU_NAME, RE::FavoritesMenu::MENU_NAME, RE::Console::MENU_NAME,
		RE::CraftingMenu::MENU_NAME, RE::TrainingMenu::MENU_NAME, RE::SleepWaitMenu::MENU_NAME, RE::RaceSexMenu::MENU_NAME,
		RE::LevelUpMenu::MENU_NAME, RE::LoadingMenu::MENU_NAME };

	static constexpr std::array<std::string_view, 4> kTransientHideTokens = { "photo", "screenshot", "capture", "freecamera" };

	static void Invoke(const std::string& a_script)
	{
		std::scoped_lock lock(g_viewMutex);

		if (!g_prismaUI || !g_viewReady || !g_view) {
			return;
		}

		g_prismaUI->Invoke(g_view, a_script.c_str());
	}

	struct HudSettings
	{
		bool playerEnabled{ true };
		bool enemyEnabled{ true };
		bool showHealth{ true };
		bool showMagicka{ true };
		bool showStamina{ true };
		bool showExperience{ true };

		float playerLeft{ 2.75f };
		float playerBottom{ 7.5f };
		float playerScale{ 1.0f };
		float playerOpacity{ 0.96f };
		float resourceWidth{ 400.0f };
		float diamondSize{ 100.0f };

		float enemyLeft{ 50.0f };
		float enemyTop{ 9.55f };
		float enemyScale{ 1.0f };
		float enemyOpacity{ 0.97f };
		float enemyWidth{ 380.0f };
	};

	inline HudSettings g_hudSettings{};
	inline std::mutex g_hudSettingsMutex;
	inline bool g_menuRegistered = false;
	inline SKSEMenuFramework::Model::Event* g_menuEvent = nullptr;
	static inline char g_moduleLocator{};

	static std::filesystem::path GetSettingsPath()
	{
		HMODULE module{};

		if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				&g_moduleLocator, &module)) {
			SKSE::log::error("PrismaUICode: GetModuleHandleEx failed: {}", GetLastError());

			return {};
		}

		std::vector<char> buffer(MAX_PATH);

		for (;;) {
			const DWORD length = GetModuleFileNameA(module, buffer.data(), static_cast<DWORD>(buffer.size()));

			if (length == 0) {
				SKSE::log::error("PrismaUICode: GetModuleFileName failed: {}", GetLastError());

				return {};
			}

			if (length < buffer.size()) {
				buffer.resize(length);
				break;
			}

			if (buffer.size() >= 32768) {
				SKSE::log::error("PrismaUICode: Module path exceeds maximum supported length.");

				return {};
			}

			buffer.resize(std::min(buffer.size() * 2, size_t{ 32768 }));
		}

		const std::filesystem::path dllPath(buffer.data());

		const auto settingsPath = dllPath.parent_path() / "SOTNCombatHUD.ini";

		SKSE::log::info("PrismaUICode: Settings path resolved: {}", settingsPath.string());

		return settingsPath;
	}

	using IniDocument = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;

	static std::string Trim(std::string_view value)
	{
		std::size_t first = 0;
		std::size_t last = value.size();

		while (first < last && std::isspace(static_cast<unsigned char>(value[first]))) {
			++first;
		}

		while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1]))) {
			--last;
		}

		return std::string(value.substr(first, last - first));
	}

	static IniDocument ReadIniFile(const std::filesystem::path& path)
	{
		IniDocument document;

		std::ifstream file(path, std::ios::binary);

		if (!file.is_open()) {
			SKSE::log::info("PrismaUICode: Settings file not found, using defaults: {}", path.string());

			return document;
		}

		std::string section;
		std::string line;
		bool firstLine = true;

		while (std::getline(file, line)) {
			// Remove UTF-8 BOM.
			if (firstLine) {
				firstLine = false;

				if (line.size() >= 3 && static_cast<unsigned char>(line[0]) == 0xEF &&
					static_cast<unsigned char>(line[1]) == 0xBB && static_cast<unsigned char>(line[2]) == 0xBF) {
					line.erase(0, 3);
				}
			}

			const auto content = Trim(line);

			if (content.empty() || content.front() == ';' || content.front() == '#') {
				continue;
			}

			// Section.
			if (content.front() == '[' && content.back() == ']') {
				section = Trim(std::string_view(content).substr(1, content.size() - 2));

				continue;
			}

			// Key = Value.
			const auto separator = content.find('=');

			if (separator == std::string::npos) {
				continue;
			}

			const auto key = Trim(std::string_view(content).substr(0, separator));

			const auto value = Trim(std::string_view(content).substr(separator + 1));

			if (!key.empty()) {
				document[section][key] = value;
			}
		}

		if (file.bad()) {
			SKSE::log::error("PrismaUICode: Failed while reading settings file: {}", path.string());
		}

		return document;
	}

	static const std::string* FindIniValue(const IniDocument& document, const char* section, const char* key)
	{
		const auto sectionIt = document.find(section);

		if (sectionIt == document.end()) {
			return nullptr;
		}

		const auto valueIt = sectionIt->second.find(key);

		if (valueIt == sectionIt->second.end()) {
			return nullptr;
		}

		return &valueIt->second;
	}

	static bool ReadBoolSetting(const IniDocument& document, const char* section, const char* key, bool fallback)
	{
		const auto* value = FindIniValue(document, section, key);

		if (!value) {
			return fallback;
		}

		std::string normalized = *value;

		std::transform(normalized.begin(), normalized.end(), normalized.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		if (normalized == "1" || normalized == "true" || normalized == "yes" || normalized == "on") {
			return true;
		}

		if (normalized == "0" || normalized == "false" || normalized == "no" || normalized == "off") {
			return false;
		}

		return fallback;
	}

	static float ReadFloatSetting(const IniDocument& document, const char* section, const char* key, float fallback,
		float minimum, float maximum)
	{
		const auto* value = FindIniValue(document, section, key);

		if (!value || value->empty()) {
			return fallback;
		}

		float parsed{};

		const auto [ptr, error] =
			std::from_chars(value->data(), value->data() + value->size(), parsed, std::chars_format::general);

		if (error != std::errc{} || ptr != value->data() + value->size() || !std::isfinite(parsed)) {
			return fallback;
		}

		return std::clamp(parsed, minimum, maximum);
	}

	static void LoadHudSettings()
	{
		const auto path = GetSettingsPath();

		if (path.empty()) {
			SKSE::log::error("PrismaUICode: Cannot load settings: invalid path.");

			return;
		}

		const auto ini = ReadIniFile(path);

		HudSettings settings{};

		// Visibility
		settings.playerEnabled = ReadBoolSetting(ini, "Visibility", "PlayerHUD", settings.playerEnabled);

		settings.enemyEnabled = ReadBoolSetting(ini, "Visibility", "EnemyHUD", settings.enemyEnabled);

		settings.showHealth = ReadBoolSetting(ini, "Visibility", "Health", settings.showHealth);

		settings.showMagicka = ReadBoolSetting(ini, "Visibility", "Magicka", settings.showMagicka);

		settings.showStamina = ReadBoolSetting(ini, "Visibility", "Stamina", settings.showStamina);

		settings.showExperience = ReadBoolSetting(ini, "Visibility", "Experience", settings.showExperience);

		// Player HUD
		settings.playerLeft = ReadFloatSetting(ini, "PlayerHUD", "Left", settings.playerLeft, 0.0f, 40.0f);

		settings.playerBottom = ReadFloatSetting(ini, "PlayerHUD", "Bottom", settings.playerBottom, 0.0f, 35.0f);

		settings.playerScale = ReadFloatSetting(ini, "PlayerHUD", "Scale", settings.playerScale, 0.70f, 1.50f);

		settings.playerOpacity = ReadFloatSetting(ini, "PlayerHUD", "Opacity", settings.playerOpacity, 0.10f, 1.00f);

		settings.resourceWidth = ReadFloatSetting(ini, "PlayerHUD", "ResourceWidth", settings.resourceWidth, 220.0f, 580.0f);

		settings.diamondSize = ReadFloatSetting(ini, "PlayerHUD", "DiamondSize", settings.diamondSize, 56.0f, 140.0f);

		// Enemy HUD
		settings.enemyLeft = ReadFloatSetting(ini, "EnemyHUD", "Left", settings.enemyLeft, 0.0f, 100.0f);

		settings.enemyTop = ReadFloatSetting(ini, "EnemyHUD", "Top", settings.enemyTop, 0.0f, 45.0f);

		settings.enemyScale = ReadFloatSetting(ini, "EnemyHUD", "Scale", settings.enemyScale, 0.70f, 1.50f);

		settings.enemyOpacity = ReadFloatSetting(ini, "EnemyHUD", "Opacity", settings.enemyOpacity, 0.10f, 1.00f);

		settings.enemyWidth = ReadFloatSetting(ini, "EnemyHUD", "Width", settings.enemyWidth, 180.0f, 700.0f);

		{
			std::scoped_lock lock(g_hudSettingsMutex);
			g_hudSettings = settings;
		}

		SKSE::log::info("PrismaUICode: HUD settings loaded from '{}'", path.string());
	}

	static void SaveHudSettings()
	{
		HudSettings settings;

		{
			std::scoped_lock lock(g_hudSettingsMutex);
			settings = g_hudSettings;
		}

		const auto settingsPath = GetSettingsPath();

		if (settingsPath.empty()) {
			SKSE::log::error("PrismaUICode: Cannot save settings: invalid path.");

			return;
		}

		std::error_code ec;

		std::filesystem::create_directories(settingsPath.parent_path(), ec);

		if (ec) {
			SKSE::log::error("PrismaUICode: Cannot create settings directory: {}", ec.message());

			return;
		}

		// Build complete INI document.
		const std::string contents = fmt::format(
			R"ini(; Symphony of the Night - Combat HUD
				; Automatically generated settings file.

				[Visibility]
				PlayerHUD={}
				EnemyHUD={}
				Health={}
				Magicka={}
				Stamina={}
				Experience={}

				[PlayerHUD]
				Left={:.6f}
				Bottom={:.6f}
				Scale={:.6f}
				Opacity={:.6f}
				ResourceWidth={:.6f}
				DiamondSize={:.6f}

				[EnemyHUD]
				Left={:.6f}
				Top={:.6f}
				Scale={:.6f}
				Opacity={:.6f}
				Width={:.6f}
				)ini",
				settings.playerEnabled ? 1 : 0, settings.enemyEnabled ? 1 : 0, settings.showHealth ? 1 : 0,
				settings.showMagicka ? 1 : 0, settings.showStamina ? 1 : 0, settings.showExperience ? 1 : 0,

				settings.playerLeft, settings.playerBottom, settings.playerScale, settings.playerOpacity, settings.resourceWidth,
				settings.diamondSize,

				settings.enemyLeft, settings.enemyTop, settings.enemyScale, settings.enemyOpacity, settings.enemyWidth);

		// Write to a temporary file first.
		auto tempPath = settingsPath;
		tempPath += L".tmp";

		std::ofstream file(tempPath, std::ios::binary | std::ios::trunc);

		if (!file.is_open()) {
			SKSE::log::error("PrismaUICode: Cannot open temporary settings file '{}'", tempPath.string());

			return;
		}

		file.write(contents.data(), static_cast<std::streamsize>(contents.size()));

		file.flush();

		bool writeSuccess = file.good();

		file.close();

		writeSuccess = writeSuccess && !file.fail();

		if (!writeSuccess) {
			std::filesystem::remove(tempPath, ec);

			SKSE::log::error("PrismaUICode: Failed writing temporary settings file '{}'", tempPath.string());

			return;
		}

		// Replace the original INI only after successful writing.
		if (!MoveFileExW(tempPath.c_str(), settingsPath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
			const DWORD error = GetLastError();

			std::filesystem::remove(tempPath, ec);

			SKSE::log::error("PrismaUICode: Failed to replace settings file '{}'. Win32 error: {}", settingsPath.string(), error);

			return;
		}

		SKSE::log::info("PrismaUICode: HUD settings successfully saved to '{}'", settingsPath.string());
	}

	static std::string BuildSettingsScript(const HudSettings& s)
	{
		return fmt::format(
			"window.applyHudSettings({{playerEnabled:{},enemyEnabled:{},showHealth:{},showMagicka:{},showStamina:{},"
		    "showExperience:{},"
			"playerLeft:{},playerBottom:{},playerScale:{},playerOpacity:{},resourceWidth:{},diamondSize:{},"
			"enemyLeft:{},enemyTop:{},enemyScale:{},enemyOpacity:{},enemyWidth:{}}})",
			s.playerEnabled ? "true" : "false", s.enemyEnabled ? "true" : "false", s.showHealth ? "true" : "false",
			s.showMagicka ? "true" : "false", s.showStamina ? "true" : "false", s.showExperience ? "true" : "false", s.playerLeft,
			s.playerBottom, s.playerScale, s.playerOpacity, s.resourceWidth, s.diamondSize, s.enemyLeft, s.enemyTop, s.enemyScale,
			s.enemyOpacity, s.enemyWidth);
	}

	static void ApplyHudSettingsLive(const HudSettings& settings)
	{
		if (auto* task = SKSE::GetTaskInterface()) {
			task->AddTask([settings]() { Invoke(BuildSettingsScript(settings)); });
		} else {
			Invoke(BuildSettingsScript(settings));
		}
	}

	static void __stdcall RenderHudSettingsMenu()
	{
		HudSettings settings;
		{
			std::scoped_lock lock(g_hudSettingsMutex);
			settings = g_hudSettings;
		}

		bool changed = false;
		ImGuiMCP::Text("SYMPHONY OF THE NIGHT - COMBAT HUD");
		ImGuiMCP::Text("Changes are applied live. Save Settings to keep them after restart.");
		ImGuiMCP::Separator();

		if (ImGuiMCP::CollapsingHeader("Visibility")) {
			changed |= ImGuiMCP::Checkbox("Player HUD", &settings.playerEnabled);
			changed |= ImGuiMCP::Checkbox("Enemy HUD", &settings.enemyEnabled);
			ImGuiMCP::Separator();
			changed |= ImGuiMCP::Checkbox("Health", &settings.showHealth);
			changed |= ImGuiMCP::Checkbox("Magicka", &settings.showMagicka);
			changed |= ImGuiMCP::Checkbox("Stamina", &settings.showStamina);
			changed |= ImGuiMCP::Checkbox("Experience", &settings.showExperience);
		}

		if (ImGuiMCP::CollapsingHeader("Player HUD")) {
			ImGuiMCP::Text("Position");
			changed |= ImGuiMCP::SliderFloat("Left Offset (%)", &settings.playerLeft, 0.0f, 40.0f, "%.1f");
			changed |= ImGuiMCP::SliderFloat("Bottom Offset (%)", &settings.playerBottom, 0.0f, 35.0f, "%.1f");
			ImGuiMCP::Separator();
			ImGuiMCP::Text("Native Size and Appearance");
			changed |= ImGuiMCP::SliderFloat("Native Size##player", &settings.playerScale, 0.70f, 1.50f, "%.2fx");
			changed |= ImGuiMCP::SliderFloat("Opacity##player", &settings.playerOpacity, 0.10f, 1.00f, "%.2f");
			changed |= ImGuiMCP::SliderFloat("Bar Width", &settings.resourceWidth, 220.0f, 580.0f, "%.0f px");
			changed |= ImGuiMCP::SliderFloat("Level Diamond Size", &settings.diamondSize, 56.0f, 140.0f, "%.0f px");
		}

		if (ImGuiMCP::CollapsingHeader("Enemy HUD")) {
			ImGuiMCP::Text("Position");
			changed |= ImGuiMCP::SliderFloat("Horizontal Position (%)", &settings.enemyLeft, 0.0f, 100.0f, "%.1f");
			changed |= ImGuiMCP::SliderFloat("Top Offset (%)", &settings.enemyTop, 0.0f, 45.0f, "%.1f");
			ImGuiMCP::Separator();
			ImGuiMCP::Text("Native Size and Appearance");
			changed |= ImGuiMCP::SliderFloat("Native Size##enemy", &settings.enemyScale, 0.70f, 1.50f, "%.2fx");
			changed |= ImGuiMCP::SliderFloat("Opacity##enemy", &settings.enemyOpacity, 0.10f, 1.00f, "%.2f");
			changed |= ImGuiMCP::SliderFloat("Panel Width", &settings.enemyWidth, 180.0f, 700.0f, "%.0f px");
		}

		ImGuiMCP::Separator();
		if (ImGuiMCP::Button("Reset to Defaults")) {
			settings = HudSettings{};
			changed = true;
		}
		ImGuiMCP::SameLine();
		if (ImGuiMCP::Button("Save Settings")) {
			{
				std::scoped_lock lock(g_hudSettingsMutex);
				g_hudSettings = settings;
			}
			SaveHudSettings();
		}

		if (changed) {
			{
				std::scoped_lock lock(g_hudSettingsMutex);
				g_hudSettings = settings;
			}
			ApplyHudSettingsLive(settings);
		}
	}

	static void __stdcall OnMenuFrameworkEvent(SKSEMenuFramework::Model::EventType eventType)
	{
		if (eventType == SKSEMenuFramework::Model::kCloseMenu) {
			SaveHudSettings();
		}
	}

	static void TryRegisterHudSettingsMenu()
	{
		if (g_menuRegistered || !GetMenuFrameworkModule()) {
			return;
		}

		SKSEMenuFramework::SetSection("Symphony of the Night");
		SKSEMenuFramework::AddSectionItem("Combat HUD", RenderHudSettingsMenu);
		g_menuEvent = SKSEMenuFramework::AddEvent(OnMenuFrameworkEvent, 0.0f);
		g_menuRegistered = true;
		SKSE::log::info("PrismaUICode: SKSE Menu Framework settings page registered (v{})",
			SKSEMenuFramework::GetMenuFrameworkVersion());
	}

	static void SetGameFade(bool a_fadingToBlack)
	{
		Invoke(std::string("window.setGameFade(") + (a_fadingToBlack ? "true" : "false") + ")");
	}

	static void SetCombatState(RE::ACTOR_COMBAT_STATE a_state)
	{
		switch (a_state) {
		case RE::ACTOR_COMBAT_STATE::kCombat:
			Invoke("window.setCombatState('combat')");
			break;

		case RE::ACTOR_COMBAT_STATE::kSearching:
			Invoke("window.setCombatState('searching')");
			break;

		default:
			Invoke("window.setCombatState('idle')");
			break;
		}
	}

	static void RefreshCombatState()
	{
		RE::ACTOR_COMBAT_STATE newState = RE::ACTOR_COMBAT_STATE::kNone;

		bool stateChanged = false;

		{
			std::scoped_lock lock(g_combatMutex);

			bool hasSearching = false;

			for (const auto& [id, state] : g_combatants) {
				if (state == RE::ACTOR_COMBAT_STATE::kCombat) {
					newState = RE::ACTOR_COMBAT_STATE::kCombat;
					break;
				}

				if (state == RE::ACTOR_COMBAT_STATE::kSearching) {
					hasSearching = true;
				}
			}

			if (newState != RE::ACTOR_COMBAT_STATE::kCombat && hasSearching) {
				newState = RE::ACTOR_COMBAT_STATE::kSearching;
			}

			if (newState != g_playerCombatState) {
				g_playerCombatState = newState;
				stateChanged = true;
			}
		}

		if (stateChanged) {
			SetCombatState(newState);
		}
	}

	static void ResetCombatTracking()
	{
		{
			std::scoped_lock lock(g_combatMutex);

			g_combatants.clear();
			g_playerCombatState = RE::ACTOR_COMBAT_STATE::kNone;
		}

		SetCombatState(RE::ACTOR_COMBAT_STATE::kNone);
	}

	static bool IsKnownHiddenMenu(std::string_view name)
	{
		return std::any_of(kHiddenMenus.begin(), kHiddenMenus.end(), [name](const auto& menu) { return name == menu; });
	}

	static bool IsTransientHideMenu(std::string_view name)
	{
		std::string lowered;
		lowered.reserve(name.size());

		for (char ch : name) {
			lowered.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
		}

		return std::any_of(kTransientHideTokens.begin(), kTransientHideTokens.end(),
			[&lowered](const auto& token) { return lowered.find(token) != std::string::npos; });
	}

	static bool IsAnyHiddenMenuOpen(RE::UI* ui)
	{
		if (!ui) {
			return false;
		}

		return std::any_of(kHiddenMenus.begin(), kHiddenMenus.end(), [ui](const auto& menu) { return ui->IsMenuOpen(menu); });
	}

	static void HideHUD()
	{
		PrismaInterface* ui = nullptr;
		PrismaView view = 0;

		{
			std::scoped_lock lock(g_viewMutex);

			if (!g_prismaUI || !g_viewReady || !g_view) {
				return;
			}

			if (g_viewHidden.exchange(true)) {
				return;
			}

			ui = g_prismaUI;
			view = g_view;
		}

		ui->Hide(view);
		SKSE::log::info("PrismaUICode: HUD hidden");
	}

	static void ShowHUD()
	{
		PrismaInterface* ui = nullptr;
		PrismaView view = 0;

		{
			std::scoped_lock lock(g_viewMutex);

			if (!g_prismaUI || !g_viewReady || !g_view) {
				return;
			}

			if (!g_viewHidden.exchange(false)) {
				return;
			}

			ui = g_prismaUI;
			view = g_view;
		}

		ui->Show(view);
		SKSE::log::info("PrismaUICode: HUD shown");
	}

	static void UpdateHUDVisibility()
	{
		auto* player = RE::PlayerCharacter::GetSingleton();

		const bool worldReady = player && player->Is3DLoaded();

		g_playerWorldReady.store(worldReady, std::memory_order_release);

		if (!worldReady) {
			HideHUD();
			return;
		}

		auto* ui = RE::UI::GetSingleton();

		if (!ui) {
			HideHUD();
			return;
		}

		const bool mainMenu = ui->IsMenuOpen(RE::MainMenu::MENU_NAME);

		const bool loadingMenu = ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME);

		const bool paused = ui->GameIsPaused();

		const bool modal = ui->IsModalMenuOpen();

		const bool applicationMenu = ui->IsApplicationMenuOpen();

		const bool hiddenMenu = IsAnyHiddenMenuOpen(ui);

		const auto* camera = RE::PlayerCamera::GetSingleton();

		const bool freeCamera = camera && camera->IsInFreeCameraMode();

		const bool transient = g_transientHideMenuCount.load(std::memory_order_acquire) > 0;

		const bool blocked =
			mainMenu || loadingMenu || paused || modal || applicationMenu || hiddenMenu || freeCamera || transient;

		if (blocked) {
			HideHUD();
		} else {
			ShowHUD();
		}
	}

	static std::string EscapeJSString(std::string_view value)
	{
		std::string escaped;
		escaped.reserve(value.size());
		for (const unsigned char ch : value) {
			switch (ch) {
			case '\\':
				escaped += "\\\\";
				break;
			case '\'':
				escaped += "\\\'";
				break;
			case '\n':
				escaped += " ";
				break;
			case '\r':
				escaped += " ";
				break;
			default:
				escaped += static_cast<char>(ch);
				break;
			}
		}
		return escaped;
	}

	static void Update(float deltaTime)
	{
		TryRegisterHudSettingsMenu();
		g_visibilityTimer += deltaTime;
		g_resourceUpdateTimer += deltaTime;
		g_targetUpdateTimer += deltaTime;

		if (g_visibilityTimer >= kVisibilityCheckInterval) {
			g_visibilityTimer = 0.0f;
			UpdateHUDVisibility();
		}

		auto* player = RE::PlayerCharacter::GetSingleton();

		if (!player || !player->skills || !player->skills->data) {
			return;
		}

		const auto level = player->GetLevel();
		const auto currentXP = player->skills->data->xp;
		const auto requiredXP = player->skills->data->levelThreshold;

		if (level != g_lastLevel || currentXP != g_lastXP || requiredXP != g_lastRequiredXP) {
			g_lastLevel = level;
			g_lastXP = currentXP;
			g_lastRequiredXP = requiredXP;

			const auto script = "window.setLevelProgress(" + std::to_string(level) + "," + std::to_string(currentXP) + "," +
			                    std::to_string(requiredXP) + ")";
			Invoke(script);
		}

		if (g_resourceUpdateTimer >= 0.10f) {
			g_resourceUpdateTimer = 0.0f;
			const std::array<float, 6> values{ player->GetActorValue(RE::ActorValue::kHealth),
				player->GetActorValueMax(RE::ActorValue::kHealth), player->GetActorValue(RE::ActorValue::kMagicka),
				player->GetActorValueMax(RE::ActorValue::kMagicka), player->GetActorValue(RE::ActorValue::kStamina),
				player->GetActorValueMax(RE::ActorValue::kStamina) };

			if (values != g_lastResources) {
				g_lastResources = values;
				const auto script = "window.setResourceBars(" + std::to_string(values[0]) + "," + std::to_string(values[1]) +
				                    "," + std::to_string(values[2]) + "," + std::to_string(values[3]) + "," +
				                    std::to_string(values[4]) + "," + std::to_string(values[5]) + ")";
				Invoke(script);
			}
		}

		if (g_targetUpdateTimer >= 0.10f) {
			g_targetUpdateTimer = 0.0f;
			auto currentTarget = player->currentCombatTarget.get();
			auto* targetActor = currentTarget.get();

			if (targetActor && targetActor != player && !targetActor->IsDead()) {
				const auto targetID = targetActor->GetFormID();
				const float hp = targetActor->GetActorValue(RE::ActorValue::kHealth);
				const float maxHP = targetActor->GetActorValueMax(RE::ActorValue::kHealth);
				if (targetID != g_lastTargetID || hp != g_lastTargetHealth || maxHP != g_lastTargetMaxHealth) {
					g_lastTargetID = targetID;
					g_lastTargetHealth = hp;
					g_lastTargetMaxHealth = maxHP;
					const auto name = EscapeJSString(targetActor->GetDisplayFullName());
					const auto script = "window.setEnemyHealth('" + name + "'," + std::to_string(targetActor->GetLevel()) + "," +
					                    std::to_string(hp) + "," + std::to_string(maxHP) + ")";
					Invoke(script);
				}
			} else if (g_lastTargetID != 0) {
				g_lastTargetID = 0;
				g_lastTargetHealth = -1.0f;
				g_lastTargetMaxHealth = -1.0f;
				Invoke("window.setEnemyHidden()");
			}
		}
	}

	static void OnDOMReady(PrismaView a_view)
	{
		{
			std::scoped_lock lock(g_viewMutex);

			g_view = a_view;
			g_viewReady = true;
		}

		SKSE::log::info("PrismaUICode: DOM ready, view={}", a_view);

		RE::ACTOR_COMBAT_STATE currentState;

		{
			std::scoped_lock lock(g_combatMutex);
			currentState = g_playerCombatState;
		}

		SetCombatState(currentState);

		HudSettings settings;
		{
			std::scoped_lock lock(g_hudSettingsMutex);
			settings = g_hudSettings;
		}
		Invoke(BuildSettingsScript(settings));

		g_lastLevel = 0;
		g_lastXP = -1.0f;
		g_lastRequiredXP = -1.0f;
		g_lastResources.fill(-1.0f);
		g_lastTargetID = 0;
		g_lastTargetHealth = -1.0f;
		g_lastTargetMaxHealth = -1.0f;
		g_resourceUpdateTimer = 0.11f;
		g_targetUpdateTimer = 0.11f;
		Update(0.0f);
	}

	static void InitializePrismaUI()
	{
		g_prismaUI = PRISMA_UI_API::RequestPluginAPI<PrismaInterface>();

		if (!g_prismaUI) {
			SKSE::log::error("PrismaUICode: Prisma UI API unavailable");
			return;
		}

		const auto view = g_prismaUI->CreateView("CombatHUD/index.html", OnDOMReady);

		if (!view) {
			SKSE::log::error("PrismaUICode: Failed to create view");
			return;
		}

		{
			std::scoped_lock lock(g_viewMutex);

			g_view = view;
			g_viewHidden = true;
		}

		g_prismaUI->SetOrder(view, 100);

		g_prismaUI->Hide(view);

		SKSE::log::info("PrismaUICode: View created and hidden");
	}
}

namespace
{
	constexpr float kCritChancePerStack = 5.0f;
	constexpr float kDuration = 5.0f;

	inline int g_currentStacks = 0;
	inline float g_critTimer = 0.0f;

	static void ResetCritBonus()
	{
		if (g_currentStacks <= 0) {
			return;
		}

		const float bonus = kCritChancePerStack * static_cast<float>(g_currentStacks);

		RE::PlayerCharacter::GetSingleton()->ModActorValue(RE::ACTOR_VALUE_MODIFIER::kPermanent, RE::ActorValue::kCriticalChance,
			-bonus);

		//SKSE::log::info("CritBonus: removed {} stacks (-{} CritChance)", g_currentStacks, bonus);

		g_currentStacks = 0;
		g_critTimer = 0.0f;
	}
}

namespace Event
{
	class EventSink
	{
	public:
		template <typename EventType>
		static void Register(RE::BSEventNotifyControl (*handler)(const EventType*, RE::BSTEventSource<EventType>*))
		{
			struct Sink : RE::BSTEventSink<EventType>
			{
				decltype(handler) _handler;

				explicit Sink(decltype(handler) h) : _handler(h) {}

				RE::BSEventNotifyControl ProcessEvent(const EventType* e, RE::BSTEventSource<EventType>* s) override
				{
					return _handler(e, s);
				}
			};

			static Sink instance(handler);

			if constexpr (std::is_same_v<EventType, SKSE::ActionEvent>) {
				if (auto* source = SKSE::GetActionEventSource()) {
					source->AddEventSink(&instance);
				} else {
					SKSE::log::error("Failed to register ActionEvent: source unavailable");
				}

			}
			else if constexpr (std::is_same_v<EventType, RE::MenuOpenCloseEvent> ||
							   std::is_same_v<EventType, RE::MenuModeChangeEvent>) {
				if (auto* ui = RE::UI::GetSingleton()) {
					ui->AddEventSink<EventType>(&instance);
				} else {
					SKSE::log::error("Failed to register UI event: UI unavailable");
				}

			}
			else {
				if (auto* source = RE::ScriptEventSourceHolder::GetSingleton()) {
					source->AddEventSink<EventType>(&instance);
				} else {
					SKSE::log::error("Failed to register game event: source unavailable");
				}
			}
		}
	};

	RE::BSEventNotifyControl OnMenuOpenClose(const RE::MenuOpenCloseEvent* event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		if (!event) {
			return RE::BSEventNotifyControl::kContinue;
		}

		const std::string_view name = event->menuName;

		{
			using namespace PrismaUICode;

			if (name == RE::FaderMenu::MENU_NAME) {
				SetGameFade(event->opening);

				return RE::BSEventNotifyControl::kContinue;
			}

			if (IsTransientHideMenu(name)) {
				if (event->opening) {
					g_transientHideMenuCount.fetch_add(1);
				} else {
					auto count = g_transientHideMenuCount.load();

					while (count > 0 && !g_transientHideMenuCount.compare_exchange_weak(count, count - 1)) {}
				}
			}

			if (name == RE::MainMenu::MENU_NAME && event->opening) {
				g_playerWorldReady.store(false);
				g_transientHideMenuCount.store(0);

				ResetCombatTracking();

				HideHUD();

				return RE::BSEventNotifyControl::kContinue;
			}

			if (event->opening && (IsKnownHiddenMenu(name) || IsTransientHideMenu(name))) {
				HideHUD();
			} else {
				UpdateHUDVisibility();
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnCellFullyLoaded(const RE::TESCellFullyLoadedEvent* e, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>*)
	{
		if (!e || !e->cell)
			return RE::BSEventNotifyControl::kContinue;

		SKSE::GetTaskInterface()->AddTask([cell = e->cell]() {
			if (!cell)
				return;

			cell->ForEachReference([](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult {
				if (!ref)
					return RE::BSContainer::ForEachResult::kContinue;

				auto* actor = ref->As<RE::Actor>();
				if (actor && !actor->IsPlayerRef()) {
					UpdateValues(actor, "CELL FULLY LOADED");
				}
				return RE::BSContainer::ForEachResult::kContinue;
			});
		});

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnFastTravelEnd(const RE::TESFastTravelEndEvent*, RE::BSTEventSource<RE::TESFastTravelEndEvent>*)
	{
		UpdateValues("FAST TRAVEL END");

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnSwitchRaceComplete(const RE::TESSwitchRaceCompleteEvent*, RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>*)
	{
		UpdateValues("SWITCH RACE COMPLETE");

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnActorAction(const SKSE::ActionEvent* e, RE::BSTEventSource<SKSE::ActionEvent>*)
	{
		if (!e->actor->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		if (e->type.any(SKSE::ActionEvent::Type::kBeginDraw)) {
			UpdateValues("ACTOR ACTION");
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnMagicEffectApply(const RE::TESMagicEffectApplyEvent* e, RE::BSTEventSource<RE::TESMagicEffectApplyEvent>*)
	{
		if (!e->target.get()->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* mgef = RE::TESForm::LookupByID<RE::EffectSetting>(e->magicEffect);
		if (!mgef) {
			return RE::BSEventNotifyControl::kContinue;
		}
		//const char* name = mgef->GetFullName() ? mgef->GetFullName() : "None";
		//const char* editorID = mgef->GetFormEditorID() ? mgef->GetFormEditorID() : "None";
		//auto formID = mgef->GetFormID();
		//SKSE::log::info("MGEF -> Name: {} | EditorID: {} | FormID: 0x{:08X}", name, editorID, formID);

		if (mgef->HasKeywordByEditorID("UpdateValues")) {
			UpdateValues();
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnContainerChanged(const RE::TESContainerChangedEvent* e, RE::BSTEventSource<RE::TESContainerChangedEvent>*)
	{
		if (!e || !e->baseObj)
			return RE::BSEventNotifyControl::kContinue;

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnEquip(const RE::TESEquipEvent* e, RE::BSTEventSource<RE::TESEquipEvent>*)
	{
		if (!e || !e->actor) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto a = e->actor.get()->As<RE::Actor>();

		if (a->IsPlayerRef() || a->IsPlayerTeammate()) {
			SKSE::GetTaskInterface()->AddTask([actor = a]() { UpdateValues(actor, "EQUIP"); });
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnHit(const RE::TESHitEvent* e, RE::BSTEventSource<RE::TESHitEvent>*)
	{
		if (!e || !e->cause || !e->cause->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* player = RE::PlayerCharacter::GetSingleton();
		if (!player) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* warriorStone = FormLookup::Get<RE::EffectSetting>(FormIDKeys::MGEF::kWarriorStone);
		//auto* warriorStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_WARRIOR_STONE);

		if (!warriorStone || !player->HasMagicEffect(warriorStone)) {
			return RE::BSEventNotifyControl::kContinue;
		}

		++g_currentStacks;
		g_critTimer = kDuration;

		player->ModActorValue(RE::ACTOR_VALUE_MODIFIER::kPermanent, RE::ActorValue::kCriticalChance, kCritChancePerStack);

		SKSE::log::info("CritBonus: stack {} (+{} CritChance)", g_currentStacks, kCritChancePerStack);

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnCombat(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>*)
	{
		if (!a_event) {
			return RE::BSEventNotifyControl::kContinue;
		}

		{
			using namespace PrismaUICode;

			auto player = RE::PlayerCharacter::GetSingleton();

			auto* actorRef = a_event->actor.get();
			auto* actor = actorRef ? actorRef->As<RE::Actor>() : nullptr;

			if (!actor || actor == player) {
				return RE::BSEventNotifyControl::kContinue;
			}

			auto* eventTarget = a_event->targetActor.get();

			auto currentTarget = actor->currentCombatTarget.get();
			auto* currentTargetActor = currentTarget.get();

			const bool searchingFlag = actor->boolBits.all(RE::Actor::BOOL_BITS::kSearchingInCombat);

			auto* combatGroup = actor->GetCombatGroup();

			RE::NiPointer<RE::Actor> groupSearchTarget;

			if (combatGroup) {
				groupSearchTarget = combatGroup->targetToSearchFor.get();
			}

			auto* groupSearchTargetActor = groupSearchTarget.get();

			const auto eventState = a_event->newState.get();

			const auto actorID = actor->GetFormID();

			{
				std::scoped_lock lock(g_combatMutex);

				auto it = g_combatants.find(actorID);

				const bool wasTracked = it != g_combatants.end();

				const bool nativeSearchingForPlayer =
					searchingFlag && (groupSearchTargetActor == player || (!groupSearchTargetActor && wasTracked));

				switch (eventState) {
				case RE::ACTOR_COMBAT_STATE::kNone:

					if (!eventTarget || eventTarget == player || currentTargetActor != player) {
						g_combatants.erase(actorID);
					}

					break;

				case RE::ACTOR_COMBAT_STATE::kCombat:

					if (eventTarget == player || (!eventTarget && currentTargetActor == player)) {
						g_combatants[actorID] = RE::ACTOR_COMBAT_STATE::kCombat;
					} else if (currentTargetActor != player) {
						g_combatants.erase(actorID);
					}

					break;

				case RE::ACTOR_COMBAT_STATE::kSearching:

					if (eventTarget == player || (!eventTarget && currentTargetActor == player) || nativeSearchingForPlayer) {
						g_combatants[actorID] = RE::ACTOR_COMBAT_STATE::kSearching;
					} else {
						g_combatants.erase(actorID);
					}

					break;

				default:
					break;
				}
			}

			RefreshCombatState();

		}

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnDeath(const RE::TESDeathEvent* e, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		if (!e || !e->actorDying) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* player = RE::PlayerCharacter::GetSingleton();

		auto* dyingActor = e->actorDying.get()->As<RE::Actor>();

		{
			using namespace PrismaUICode;

			{
				std::scoped_lock lock(g_combatMutex);
				g_combatants.erase(dyingActor->GetFormID());
			}

			RefreshCombatState();
		}

		if (player && player->skills && player->skills->data && e->actorKiller && e->actorKiller.get()->IsPlayerRef() &&
			dyingActor) {
			const RE::Actor* killerActor = e->actorKiller.get()->As<RE::Actor>();

			if (dyingActor != killerActor) {
				const float playerlvl = player->GetLevel();
				const float actorlvl = dyingActor->GetLevel();

				const float exp = actorlvl / playerlvl * actorlvl + 1.0f + actorlvl;

				player->skills->data->xp += exp;

				std::string message =
					fmt::format("{} {} lvl, +{}xp", dyingActor->GetName(), dyingActor->GetLevel(), static_cast<int>(exp));

				RE::DebugNotification(message.c_str());
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	void Update(float deltaTime)
	{
		PrismaUICode::Update(deltaTime);

		if (g_currentStacks > 0) {
			auto* warriorStone = FormLookup::Get<RE::EffectSetting>(FormIDKeys::MGEF::kWarriorStone);
			//auto* warriorStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_WARRIOR_STONE);

			if (warriorStone && RE::PlayerCharacter::GetSingleton()->HasMagicEffect(warriorStone)) {
				g_critTimer -= deltaTime;

				if (g_critTimer <= 0.0f) {
					ResetCritBonus();
				}
			}
			else {
				ResetCritBonus();
			}
		}
	}

	void Initialize()
	{
		EventSink::Register(OnMenuOpenClose);
		EventSink::Register(OnCellFullyLoaded);
		EventSink::Register(OnFastTravelEnd);
		EventSink::Register(OnSwitchRaceComplete);
		EventSink::Register(OnActorAction);
		EventSink::Register(OnMagicEffectApply);
		EventSink::Register(OnContainerChanged);
		EventSink::Register(OnEquip);
		EventSink::Register(OnHit);
		EventSink::Register(OnCombat);
		EventSink::Register(OnDeath);
	}
}

namespace VirtualHook
{
	class ArrowGravity
	{
		static float thunk(RE::Projectile* projectile)
		{
			//if (auto shooter = projectile->shooter.get(); shooter && shooter->As<RE::Actor>())
			//{
			//	if (shooter->As<RE::Actor>()->HasPerk(RE::TESForm::LookupByID<RE::BGSPerk>(0x00079354)))
			//	{
			//		return 0;
			//	}
			//}

			return func(projectile);
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:

		static void Install() { func = REL::Relocation<uintptr_t>(RE::VTABLE_ArrowProjectile[0]).write_vfunc(0xb5, thunk); }
	};

	class PlayerUpdate
	{
		static void thunk(RE::PlayerCharacter* a, float dt)
		{
			func(a, dt);
			if (a->GetActorValueMax(RE::ActorValue::kSpeedMult) < 80.0f) {
				a->playerFlags.isSprinting = false;
			}
			Event::Update(dt);
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install() { func = REL::Relocation<uintptr_t>(RE::PlayerCharacter::VTABLE[0]).write_vfunc(0xAD, thunk); }
	};

	class ActorUpdate
	{
		static void thunk(RE::Actor *a, float dt)
		{
			func(a, dt);
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install() { func = REL::Relocation<uintptr_t>(RE::Character::VTABLE[0]).write_vfunc(0xAD, thunk); }
	};

	template <class TActor>
	class HandleHealthDamage
	{
		static void thunk(RE::Actor* a_this, RE::Actor* a_attacker, float a_damage)
		{
			func(a_this, a_attacker, a_damage);
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install() { func = REL::Relocation<uintptr_t>(TActor::VTABLE[0]).write_vfunc(0x104, thunk); }
	};

	template <class TActor>
	struct ModActorValue
	{
		static void thunk(RE::ActorValueOwner* a_this, RE::ACTOR_VALUE_MODIFIER a_modifier, RE::ActorValue a_value,
			float a_amount)
		{
			func(a_this, a_modifier, a_value, a_amount);
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static void Install() { func = REL::Relocation<std::uintptr_t>(TActor::VTABLE[5]).write_vfunc(0x06, thunk); }
	};	

	template <class TEffect>
	struct ModifyActorValueEffect
	{
		static void thunk(RE::ValueModifierEffect* a_this, RE::Actor* a_actor, float a_value, RE::ActorValue a_actorValue)
		{
			func(a_this, a_actor, a_value, a_actorValue);
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static void Install() { func = REL::Relocation<std::uintptr_t>(TEffect::VTABLE[0]).write_vfunc(0x20, thunk); }
	};

	static void Initialize()
	{
		//ArrowGravity::Install();

		PlayerUpdate::Install();
		ActorUpdate::Install();

		HandleHealthDamage<RE::Character>::Install();
		HandleHealthDamage<RE::PlayerCharacter>::Install();

		//ModActorValue<RE::Character>::Install();
		//ModActorValue<RE::PlayerCharacter>::Install();

		//ModifyActorValueEffect<RE::ValueModifierEffect>::Install();
		//ModifyActorValueEffect<RE::DualValueModifierEffect>::Install();
		//ModifyActorValueEffect<RE::PeakValueModifierEffect>::Install();
		//ModifyActorValueEffect<RE::AbsorbEffect>::Install();
		//ModifyActorValueEffect<RE::AccumulatingValueModifierEffect>::Install();
		//ModifyActorValueEffect<RE::TargetValueModifierEffect>::Install();
		//ModifyActorValueEffect<RE::ValueAndConditionsEffect>::Install();
	}
}

namespace Hook
{
	class LevelUpMenu
	{
		static void thunk()
		{
			const auto player = RE::PlayerCharacter::GetSingleton();
			player->skills->AdvanceLevel(false);
			UpdateValues("LEVEL UP");
		}

	public:
		static void Install() { SKSE::GetTrampoline().write_call<5>(REL::ID(51638).address() + 0xf8e, &thunk); }
	};

	class Training
	{
	private:
		struct MiscStat
		{
			const char* name;
			const char* additionalName;
			std::uint32_t value;
			std::uint32_t additionalValue;
			std::uint8_t category;
		};

		inline static RE::Setting* trainingNumAllowedPerLevelSetting = nullptr;
		inline static std::uint32_t baseTrainingNumAllowedPerLevel = 0;
		inline static RE::UI::Create_t* originalCreateTrainingMenu = nullptr;

		static std::optional<std::uint32_t> GetTrainingSessions()
		{
			static REL::Relocation<const RE::BSTArray<MiscStat>*> stats{ RELOCATION_ID(514503, 400662) };
			if (!stats->data()) {
				return std::nullopt;
			}
			for (const auto& stat : *stats) {
				if (stat.name && ::_stricmp(stat.name, "Training Sessions") == 0) {
					return stat.value;
				}
			}
			return std::nullopt;
		}

		static std::uint32_t CalculateTrainingLimit
		(std::uint32_t baseLimit, std::uint32_t level, std::uint32_t trainingSessions,
			std::uint32_t skillTrainingsThisLevel)
		{
			const auto earned = static_cast<std::uint64_t>(level) * baseLimit;
			const auto spentBeforeLevel =
				trainingSessions > skillTrainingsThisLevel ? trainingSessions - skillTrainingsThisLevel : 0;
			const auto remaining = earned > spentBeforeLevel ? earned - spentBeforeLevel : 0;
			const auto limit = std::max(remaining, static_cast<std::uint64_t>(skillTrainingsThisLevel));
			return static_cast<std::uint32_t>(std::min(limit, std::uint64_t{ std::numeric_limits<std::int32_t>::max() }));
		}

		static void UpdateTrainingLimit()
		{
			const auto* player = RE::PlayerCharacter::GetSingleton();
			const auto trainingSessions = GetTrainingSessions();
			if (!player || !trainingSessions) {
				SKSE::log::error("Cannot read player training data");
				return;
			}
			trainingNumAllowedPerLevelSetting->data.i = static_cast<std::int32_t>(CalculateTrainingLimit(
				baseTrainingNumAllowedPerLevel, player->GetLevel(), *trainingSessions, player->skillTrainingsThisLevel));
		}

		static RE::IMenu* CreateTrainingMenu()
		{
			UpdateTrainingLimit();
			return originalCreateTrainingMenu();
		}

	public:
		static void Install()
		{
			auto* setting = GSC->GetSetting("iTrainingNumAllowedPerLevel");
			auto* ui = RE::UI::GetSingleton();
			if (!ui) {
				SKSE::log::error("Cannot initialize training");
				return;
			}
			const auto entry = ui->menuMap.find(RE::TrainingMenu::MENU_NAME);
			if (entry == ui->menuMap.end() || !entry->second.create) {
				SKSE::log::error("Training menu is not available");
				return;
			}

			baseTrainingNumAllowedPerLevel = static_cast<std::uint32_t>(std::max(setting->data.i, 0));
			trainingNumAllowedPerLevelSetting = setting;
			originalCreateTrainingMenu = entry->second.create;
			entry->second.create = CreateTrainingMenu;
		}
	};

	class IsOverEncumbered
	{
		static bool thunk(RE::Actor* actor)
		{
			if (actor && actor->IsPlayerRef()) {
				return true;
			}

			return func(actor);
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install() { func = SKSE::GetTrampoline().write_call<5>(REL::ID(36457).address() + 0x1E, thunk); }
	};

	class UpdateAV
	{
		static void actor_regeneration(RE::Actor* a, float dt)
		{	
			const float fixVal = 0.01f;

			float hp = a->GetActorValueMax(RE::ActorValue::kHealth);
			float mp = a->GetActorValueMax(RE::ActorValue::kMagicka);
			float st = a->GetActorValueMax(RE::ActorValue::kStamina);

			float hpRate = a->GetActorValueMax(RE::ActorValue::kHealRate);
			float mpRate = a->GetActorValueMax(RE::ActorValue::kMagickaRate);
			float stRate = a->GetActorValueMax(RE::ActorValue::kStaminaRate);

			float hpRateMult = a->GetActorValueMax(RE::ActorValue::kHealRateMult) * fixVal;
			float mpRateMult = a->GetActorValueMax(RE::ActorValue::kMagickaRateMult) * fixVal;
			float stRateMult = a->GetActorValueMax(RE::ActorValue::kStaminaRateMult) * fixVal;

			float hpRegen = hp * hpRate * hpRateMult;
			float mpRegen = mp * mpRate * mpRateMult;
			float stRegen = st * stRate * stRateMult;

			//const float hpPercent = 0.002f;
			//const float mpPercent = 0.003f;
			//const float stPercent = 0.004f;

			//float hpRegen = (hp * hpPercent + hpRate) * hpRateMult;
			//float mpRegen = (mp * mpPercent + mpRate) * mpRateMult;
			//float stRegen = (st * stPercent + stRate) * stRateMult;

			a->RestoreActorValue(RE::ActorValue::kHealth, hpRegen * dt);
			a->RestoreActorValue(RE::ActorValue::kMagicka, mpRegen * dt);
			a->RestoreActorValue(RE::ActorValue::kStamina, stRegen * dt);
		}

		static void thunk(RE::Actor* a, RE::ActorValue av, float dt)
		{
			actor_regeneration(a, dt);
			
			if (a->IsPlayerRef() && a->As<RE::PlayerCharacter>()->IsGodMode()) {
				func(a, av, dt);
				return;
			}

			//if (a->actorState1.sprinting) {
			//	float stCost = (5 + a->equippedWeight * 0.2f);
			//	a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			//} else if (a->actorState1.running) {
			//	float stCost = (1 + a->equippedWeight * 0.1f);
			//	a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			//} else

			if (a->actorState1.swimming) {
				float stCost = (1 + a->GetActorValueMax(RE::ActorValue::kStamina) * 0.01f);
				a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			}
			
			if (a->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kBowDrawn ||
				a->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kBowAttached) {
				float stCost = 20;

				auto overdrawPerk = RE::TESForm::LookupByID<RE::BGSPerk>(0x000BABED);
				if (overdrawPerk && a->HasPerk(overdrawPerk)) {
					stCost = 10;
				}

				a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			}

			func(a, av, dt);
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install()
		{
			func = SKSE::GetTrampoline().write_call<5>(REL::ID(37510).address() + 0x1b, thunk);
		}
	};

	class RestoreAV
	{
		static bool thunk(RE::Actor* a, RE::ActorValue av, float) { return func(a, av, 0.0f); }

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install()
		{
			func = SKSE::GetTrampoline().write_call<5>(REL::ID(37510).address() + 0x176, thunk);
		}
	};

	class SendHitEventSE
	{
		static void thunk(RE::ScriptEventSourceHolder* a_holder, RE::NiPointer<RE::TESObjectREFR>& a_target,
			RE::NiPointer<RE::TESObjectREFR>& a_aggressor, RE::FormID a_source, RE::FormID a_projectile, RE::HitData& a_hitData)
		{
			func(a_holder, a_target, a_aggressor, a_source, a_projectile, a_hitData);
		}
		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install() { func = SKSE::GetTrampoline().write_call<5>(REL::ID(37633).address() + 0x16A, &thunk); }
	};

	class GetBlockCost
	{
		static float thunk(RE::HitData* hitdata)
		{
			//return 999999.1f;
			return func(hitdata);
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install()
		{
			func = SKSE::GetTrampoline().write_call<5>(REL::ID(37633).address() + 0x8d4, thunk);
		}
	};

	class GetDamage
	{
		static float thunk(void* _weap, RE::ActorValueOwner* a, float DamageMult, char isbow)
		{
			if (a->GetActorValue(RE::ActorValue::kStamina) < a->GetPermanentActorValue(RE::ActorValue::kStamina) * 0.22f) {
				// decrease attack and move speed
				return func(_weap, a, DamageMult, isbow) * 0.6f;
			}
			return func(_weap, a, DamageMult, isbow);
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install()
		{
			func = SKSE::GetTrampoline().write_call<5>(REL::ID(42832).address() + 0x1a5, thunk);
		}
	};

	class GetThisAttackChance
	{
		static float thunk(RE::Actor* me, RE::Actor* he, RE::BGSAttackData* my_attackData)
		{
			if (he->GetActorValue(RE::ActorValue::kStamina) >= 0.0f) {
				return 1.0f;
			}

			return func(me, he, my_attackData);
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install()
		{
			func = SKSE::GetTrampoline().write_call<5>(REL::ID(48139).address() + 0x2ae, thunk);
		}
	};

	class Jump
	{
		static void thunk(RE::Actor* a)
		{
			func(a);
			if (a->IsPlayerRef() && !a->As<RE::PlayerCharacter>()->IsGodMode()) {
				a->DamageActorValue(RE::ActorValue::kStamina, 8.0f + a->equippedWeight * 0.2f);
				return;
			}
		}

		static inline REL::Relocation<decltype(thunk)> func;

	public:
		static void Install()
		{
			func = SKSE::GetTrampoline().write_branch<5>(REL::ID(41349).address() + 0x114,	thunk);
		}
	};

	class CalcAttackStaminaDrain
	{
		static float thunk(RE::ActorValueOwner* avo, RE::BGSAttackData* attack, float)
		{
			auto a = skyrim_cast<RE::Actor*>(avo);

			float equipCost = a->equippedWeight * 0.4f;
			float regularCost = 4.0f + equipCost * 0.4f;
			
			if (attack->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack))
			{
				return 18.0f + equipCost;
			}
			else if (auto currentSt = a->GetActorValue(RE::ActorValue::kStamina); currentSt < regularCost) {
				return currentSt;
			}

			return regularCost;
		}

		static float thunk1(RE::ActorValueOwner* a, RE::BGSAttackData* attack)
		{
			return thunk(a, attack, func1(a, attack));
		}

		static float thunk2(RE::ActorValueOwner* a, RE::BGSAttackData* attack)
		{
			return thunk(a, attack, func2(a, attack));
		}

		static float thunk3(RE::ActorValueOwner* a, RE::BGSAttackData* attack)
		{
			return thunk(a, attack, func3(a, attack));
		}

		static float thunk4(RE::ActorValueOwner* a, RE::BGSAttackData* attack)
		{
			return thunk(a, attack, func4(a, attack));
		}

		static inline REL::Relocation<decltype(thunk1)> func1;
		static inline REL::Relocation<decltype(thunk2)> func2;
		static inline REL::Relocation<decltype(thunk3)> func3;
		static inline REL::Relocation<decltype(thunk4)> func4;

	public:

		static void Install()
		{
			func1 = SKSE::GetTrampoline().write_call<5>(REL::ID(48139).address() + 0x29b, thunk1);
			func2 = SKSE::GetTrampoline().write_call<5>(REL::ID(38047).address() + 0xbb, thunk2);
			func3 = SKSE::GetTrampoline().write_call<5>(REL::ID(37650).address() + 0x16e, thunk3);
			func4 = SKSE::GetTrampoline().write_call<5>(REL::ID(28629).address() + 0x1561, thunk4);
		}
	};

	class PowerAttackStaminaHook
	{
	private:
		using GetAttackStamina_t = float (*)(RE::ActorValueOwner*, RE::BGSAttackData*);
		static inline REL::Relocation<GetAttackStamina_t> _GetAttackStaminaPlayerCharacter;

		static float Hook_GetAttackStaminaPlayerCharacter(RE::ActorValueOwner* a_actorValueOwner, RE::BGSAttackData* a_attackData)
		{
			return _GetAttackStaminaPlayerCharacter(a_actorValueOwner, a_attackData);
		}

		static inline REL::Relocation<GetAttackStamina_t> _GetAttackStaminaActor;

		static float Hook_GetAttackStaminaActor(RE::ActorValueOwner* a_actorValueOwner, RE::BGSAttackData* a_attackData)
		{
			return _GetAttackStaminaActor(a_actorValueOwner, a_attackData);
		}

	public:
		static void Install()
		{
			constexpr REL::ID getAttackStaminaPlayerCharacterID(39003);
			constexpr auto offsetPlayer = 0xBB;
			_GetAttackStaminaPlayerCharacter = SKSE::GetTrampoline().write_call<5>(
				getAttackStaminaPlayerCharacterID.address() + offsetPlayer, Hook_GetAttackStaminaPlayerCharacter);

			constexpr REL::ID getAttackStaminaActorID(39005);
			constexpr auto offsetActor = 0xBB;
			_GetAttackStaminaActor =
				SKSE::GetTrampoline().write_call<5>(getAttackStaminaActorID.address() + offsetActor, Hook_GetAttackStaminaActor);
		}
	};

	namespace ArmorRescaled
	{
		constexpr REL::Offset kInject1Offset(0x00743617);
		constexpr REL::Offset kInject2Offset(0x00624FA2);

		constexpr uint8_t kInject1Bytes[] = { 0xF3, 0x0F, 0x10, 0x4D, 0x77, 0xF3, 0x0F, 0x58, 0xC8, 0xF3, 0x0F, 0x11, 0x4D,
			0x77 };

		constexpr uint8_t kInject2Bytes[] = { 0xF3, 0x0F, 0x10, 0x0D, 0x22, 0xD0, 0xF1, 0x00, 0xF3, 0x44, 0x0F, 0x58, 0xC0, 0x44,
			0x0F, 0x2F, 0xC1, 0x72, 0x04, 0x44, 0x0F, 0x28, 0xC1 };

		//bool TestBytes(uintptr_t a_addr, const uint8_t* a_expected, size_t a_size)
		//{
		//	for (size_t i = 0; i < a_size; i++) {
		//		if (*reinterpret_cast<uint8_t*>(a_addr + i) != a_expected[i]) {
		//			SKSE::log::error("Byte mismatch at 0x{:X}+{}: got 0x{:02X}, expected 0x{:02X}", a_addr, i, *reinterpret_cast<uint8_t*>(a_addr + i), a_expected[i]);
		//			return false;
		//		}
		//	}
		//	return true;
		//}

		static inline float ConvertDamageResist(float vanillaResist)
		{
			vanillaResist = 0.0f;
			return vanillaResist;
		}

		static void Install()
		{
			auto& trampoline = SKSE::GetTrampoline();

			{
				struct Inject1 : Xbyak::CodeGenerator
				{
					Inject1(uintptr_t a_retAddr) : Xbyak::CodeGenerator(34, nullptr)
					{
						movss(xmm0, ptr[rbp + 0x77]);
						mov(rax, reinterpret_cast<uintptr_t>(&ConvertDamageResist));
						call(rax);
						movss(ptr[rbp + 0x77], xmm0);
						mov(rax, a_retAddr);
						jmp(rax);
					}
				};

				uintptr_t addr = kInject1Offset.address();
				Inject1 code(addr + sizeof(kInject1Bytes));
				code.ready();

				void* trampBuf = trampoline.allocate(code.getSize());
				std::memcpy(trampBuf, code.getCode(), code.getSize());

				trampoline.write_branch<5>(addr, trampBuf);
				for (size_t i = 5; i < sizeof(kInject1Bytes); i++) {
					REL::safe_write(addr + i, uint8_t(0x90));
				}
			}

			{
				struct Inject2 : Xbyak::CodeGenerator
				{
					Inject2() : Xbyak::CodeGenerator(sizeof(kInject2Bytes), nullptr)
					{
						movss(xmm0, xmm8);
						mov(rax, reinterpret_cast<uintptr_t>(&ConvertDamageResist));
						call(rax);
						movss(xmm8, xmm0);
					}
				};

				Inject2 code;
				code.ready();

				uint8_t patch[sizeof(kInject2Bytes)];
				std::memset(patch, 0x90, sizeof(patch));
				std::memcpy(patch, code.getCode(), code.getSize());

				REL::safe_write(kInject2Offset.address(), patch, sizeof(patch));
			}
		}
	}

	static void Initialize()
	{
		LevelUpMenu::Install();
		Training::Install();
		IsOverEncumbered::Install();
		UpdateAV::Install();
		RestoreAV::Install();
		SendHitEventSE::Install();
		GetBlockCost::Install();
		GetDamage::Install();
		GetThisAttackChance::Install();
		Jump::Install();
		CalcAttackStaminaDrain::Install();
		PowerAttackStaminaHook::Install();
		ArmorRescaled::Install();
	}
}

namespace
{
	using namespace Xbyak;
	using namespace Xbyak::util;

	// These offsets and signatures are tied to the exact SegmentedHUD.dll
	// uploaded for this analysis. The plugin refuses to patch unknown builds.
	constexpr std::uint32_t kExpectedTimeDateStamp = 0x6A8C0276;
	constexpr std::uint32_t kExpectedSizeOfImage = 0x00198000;

	constexpr std::uintptr_t kGridBuilderRva = 0x00021870;
	constexpr std::uintptr_t kGridBuilderCallRva = 0x00022FD5;
	constexpr std::uintptr_t kGroupHelperRva = 0x00022EB0;

	constexpr std::uintptr_t kTargetStateRva = 0x00178440;

	// Internal Target::HitSink -> target-name setter call sites.
	// Both calls pass the exact actor reference that Segmented HUD stores as its target name.
	constexpr std::uintptr_t kTargetActorSetterCall1Rva = 0x000479C9;
	constexpr std::uintptr_t kTargetActorSetterCall2Rva = 0x00047A73;
	constexpr std::uintptr_t kTargetActorSetterRva = 0x00047CD0;
	constexpr std::array<std::uint8_t, 5> kExpectedTargetActorSetterCall1{ 0xE8, 0x02, 0x03, 0x00, 0x00 };
	constexpr std::array<std::uint8_t, 5> kExpectedTargetActorSetterCall2{ 0xE8, 0x58, 0x02, 0x00, 0x00 };

	// Dear ImGui 1.92.8 ImFont::GetFontBaked and its default density argument.
	constexpr std::uintptr_t kGetFontBakedRva = 0x0009CE80;
	constexpr std::uintptr_t kFontDefaultDensityRva = 0x0011490C;
	constexpr std::array<std::uint8_t, 9> kExpectedGetFontBakedPrologue{ 0x40, 0x53, 0x48, 0x81, 0xEC, 0x90, 0x00, 0x00, 0x00 };
	constexpr std::array<std::uint8_t, 4> kExpectedFontDefaultDensity{ 0x00, 0x00, 0x80, 0xBF };  // -1.0f

	// HudRoot target-name getter call: SegmentedHUD.dll+0x21F7E -> +0x47CB0.
	constexpr std::uintptr_t kTargetNameCallRva = 0x00021F7E;
	constexpr std::uintptr_t kTargetNameGetterRva = 0x00047CB0;
	constexpr std::array<std::uint8_t, 5> kExpectedTargetNameCall{ 0xE8, 0x2D, 0x5D, 0x02, 0x00 };

	// Target-name main text draw call: SegmentedHUD.dll+0x22138 -> +0x9C560.
	constexpr std::uintptr_t kTargetTextDrawCallRva = 0x00022138;
	constexpr std::uintptr_t kTargetTextDrawRva = 0x0009C560;
	constexpr std::array<std::uint8_t, 5> kExpectedTargetTextDrawCall{ 0xE8, 0x23, 0xA4, 0x07, 0x00 };

	// HudRoot::DrawPlayer path passes this address as the per-group state pointer.
	// The target/enemy path uses +0x178440 instead, so it is intentionally untouched.
	constexpr std::uintptr_t kPlayerStateRva = 0x00178428;

	constexpr std::array<std::uint8_t, 5> kExpectedGridCall{ 0xE8, 0x96, 0xE8, 0xFF, 0xFF };

	constexpr std::array<std::uint8_t, 7> kExpectedPlayerStateReference{ 0x4C, 0x8D, 0x0D, 0x47, 0x66, 0x15, 0x00 };

	constexpr std::array<std::uint8_t, 7> kExpectedTargetStateReference{ 0x4C, 0x8D, 0x0D, 0xEF, 0x64, 0x15, 0x00 };

	constexpr std::uint32_t kDefaultMaxCells = 50;
	constexpr std::uint32_t kMinMaxCells = 1;
	constexpr std::uint32_t kMaxMaxCells = 64;

	SKSE::Trampoline g_gridTrampoline;
	SKSE::Trampoline g_targetActorTrampoline1;
	SKSE::Trampoline g_targetActorTrampoline2;
	SKSE::Trampoline g_targetNameTrampoline;
	SKSE::Trampoline g_targetTextDrawTrampoline;

	std::atomic_bool g_targetSingleRow{ true };
	std::atomic_bool g_targetShowLevel{ true };
	std::atomic_bool g_targetThreatColors{ true };
	std::atomic<std::int32_t> g_targetLevel{ 0 };
	std::atomic_bool g_targetLevelValid{ false };
	std::atomic_bool g_enabled{ true };
	std::atomic_uint32_t g_maxCells{ kDefaultMaxCells };
	std::uintptr_t g_segmentedHudBase{};
	bool g_hookInstalled{};

	void LogInfo(const std::string& message) { SKSE::log::info("[SegmentedHUDTrueScale] {}", message); }

	void LogWarn(const std::string& message) { SKSE::log::warn("[SegmentedHUDTrueScale] {}", message); }

	bool ReadRel32Target(const std::uint8_t* moduleBase, std::uintptr_t instructionRva, std::uint8_t expectedOpcode,
		std::uintptr_t& targetRva)
	{
		if (moduleBase[instructionRva] != expectedOpcode) {
			return false;
		}

		std::int32_t displacement{};
		std::memcpy(&displacement, moduleBase + instructionRva + 1, sizeof(displacement));
		targetRva = instructionRva + 5 + static_cast<std::intptr_t>(displacement);
		return true;
	}

	bool ReadRipRelativeLeaTarget(const std::uint8_t* moduleBase, std::uintptr_t instructionRva,
		const std::array<std::uint8_t, 7>& expectedBytes, std::uintptr_t& targetRva)
	{
		if (std::memcmp(moduleBase + instructionRva, expectedBytes.data(), expectedBytes.size()) != 0) {
			return false;
		}

		std::int32_t displacement{};
		std::memcpy(&displacement, moduleBase + instructionRva + 3, sizeof(displacement));
		targetRva = instructionRva + 7 + static_cast<std::intptr_t>(displacement);
		return true;
	}

	bool ValidateSegmentedHud(std::uintptr_t& moduleBase)
	{
		const auto module = GetModuleHandleW(L"SegmentedHUD.dll");
		if (!module) {
			LogWarn("SegmentedHUD.dll is not loaded. No patch was installed.");
			return false;
		}

		const auto base = reinterpret_cast<std::uintptr_t>(module);
		const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
		if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0 || dos->e_lfanew > 0x1000) {
			LogWarn("SegmentedHUD.dll has an invalid DOS header. No patch was installed.");
			return false;
		}

		const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
		if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
			nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
			LogWarn("SegmentedHUD.dll is not the expected x64 PE image. No patch was installed.");
			return false;
		}

		if (nt->FileHeader.TimeDateStamp != kExpectedTimeDateStamp || nt->OptionalHeader.SizeOfImage != kExpectedSizeOfImage) {
			LogWarn("Unsupported SegmentedHUD.dll build (PE timestamp or image size differs). No patch was installed.");
			return false;
		}

		const auto* bytes = reinterpret_cast<const std::uint8_t*>(base);
		if (std::memcmp(bytes + kGridBuilderCallRva, kExpectedGridCall.data(), kExpectedGridCall.size()) != 0) {
			LogWarn("Grid-builder call signature does not match. No patch was installed.");
			return false;
		}

		std::uintptr_t gridTargetRva{};
		if (!ReadRel32Target(bytes, kGridBuilderCallRva, 0xE8, gridTargetRva) || gridTargetRva != kGridBuilderRva) {
			LogWarn("Grid-builder call resolves to an unexpected target. No patch was installed.");
			return false;
		}

		std::uintptr_t playerHelperTargetRva{};
		if (!ReadRel32Target(bytes, 0x21DEC, 0xE8, playerHelperTargetRva) || playerHelperTargetRva != kGroupHelperRva) {
			LogWarn("Player HUD helper call signature does not match. No patch was installed.");
			return false;
		}

		std::uintptr_t targetHelperTargetRva{};
		if (!ReadRel32Target(bytes, 0x21F5C, 0xE8, targetHelperTargetRva) || targetHelperTargetRva != kGroupHelperRva) {
			LogWarn("Target HUD helper call signature does not match. No patch was installed.");
			return false;
		}

		std::uintptr_t playerStateTargetRva{};
		if (!ReadRipRelativeLeaTarget(bytes, 0x21DDA, kExpectedPlayerStateReference, playerStateTargetRva) ||
			playerStateTargetRva != kPlayerStateRva) {
			LogWarn("Player-state reference signature does not match. No patch was installed.");
			return false;
		}

		std::uintptr_t targetStateTargetRva{};
		if (!ReadRipRelativeLeaTarget(bytes, 0x21F4A, kExpectedTargetStateReference, targetStateTargetRva) ||
			targetStateTargetRva != 0x00178440) {
			LogWarn("Target-state reference signature does not match. No patch was installed.");
			return false;
		}

		if (std::memcmp(bytes + kTargetActorSetterCall1Rva, kExpectedTargetActorSetterCall1.data(),
				kExpectedTargetActorSetterCall1.size()) != 0 ||
			std::memcmp(bytes + kTargetActorSetterCall2Rva, kExpectedTargetActorSetterCall2.data(),
				kExpectedTargetActorSetterCall2.size()) != 0) {
			LogWarn("Target actor-setter call signatures do not match. No patch was installed.");
			return false;
		}

		std::uintptr_t targetSetter1Rva{};
		std::uintptr_t targetSetter2Rva{};
		if (!ReadRel32Target(bytes, kTargetActorSetterCall1Rva, 0xE8, targetSetter1Rva) ||
			targetSetter1Rva != kTargetActorSetterRva ||
			!ReadRel32Target(bytes, kTargetActorSetterCall2Rva, 0xE8, targetSetter2Rva) ||
			targetSetter2Rva != kTargetActorSetterRva) {
			LogWarn("Target actor-setter calls do not resolve to the expected internal function. No patch was installed.");
			return false;
		}

		if (std::memcmp(bytes + kGetFontBakedRva, kExpectedGetFontBakedPrologue.data(), kExpectedGetFontBakedPrologue.size()) !=
				0 ||
			std::memcmp(bytes + kFontDefaultDensityRva, kExpectedFontDefaultDensity.data(), kExpectedFontDefaultDensity.size()) !=
				0) {
			LogWarn("Dear ImGui font-metrics signature does not match. No patch was installed.");
			return false;
		}

		if (std::memcmp(bytes + kTargetNameCallRva, kExpectedTargetNameCall.data(), kExpectedTargetNameCall.size()) != 0) {
			LogWarn("Target-name call signature does not match. No patch was installed.");
			return false;
		}

		std::uintptr_t targetNameGetterRva{};
		if (!ReadRel32Target(bytes, kTargetNameCallRva, 0xE8, targetNameGetterRva) ||
			targetNameGetterRva != kTargetNameGetterRva) {
			LogWarn("Target-name getter resolves to an unexpected target. No patch was installed.");
			return false;
		}

		if (std::memcmp(bytes + kTargetTextDrawCallRva, kExpectedTargetTextDrawCall.data(), kExpectedTargetTextDrawCall.size()) !=
			0) {
			LogWarn("Target text-draw call signature does not match. No patch was installed.");
			return false;
		}

		std::uintptr_t targetTextDrawRva{};
		if (!ReadRel32Target(bytes, kTargetTextDrawCallRva, 0xE8, targetTextDrawRva) || targetTextDrawRva != kTargetTextDrawRva) {
			LogWarn("Target text-draw call resolves to an unexpected target. No patch was installed.");
			return false;
		}

		moduleBase = base;
		LogInfo("Validated SegmentedHUD.dll: timestamp=0x6A8C0276, SizeOfImage=0x198000, expected patch site found.");
		return true;
	}

	void CaptureTargetActorLevel(std::uintptr_t rawForm) noexcept
	{
		// The actor reference comes from Segmented HUD's own Target::HitSink path,
		// immediately before its original target-name setter receives the same form.
		g_targetLevelValid.store(false, std::memory_order_release);

		if (rawForm == 0) {
			return;
		}

		auto* form = reinterpret_cast<RE::TESForm*>(rawForm);
		auto* actor = form->As<RE::Actor>();
		if (!actor) {
			return;
		}

		g_targetLevel.store(static_cast<std::int32_t>(actor->GetLevel()), std::memory_order_relaxed);
		g_targetLevelValid.store(true, std::memory_order_release);
	}

	using TargetNameGetter_t = const char*(__fastcall*)();

	const char* __fastcall GetTargetNameWithLevel() noexcept
	{
		if (!g_segmentedHudBase) {
			return nullptr;
		}

		const auto originalGetter = reinterpret_cast<TargetNameGetter_t>(g_segmentedHudBase + kTargetNameGetterRva);
		const char* originalName = originalGetter();

		if (!originalName || !*originalName || !g_targetShowLevel.load(std::memory_order_relaxed) ||
			!g_targetLevelValid.load(std::memory_order_acquire)) {
			return originalName;
		}

		try {
			// The target actor is captured from Segmented HUD's internal HitSink,
			// not from the player's crosshair. The HUD consumes this buffer immediately.
			thread_local std::string displayName;
			displayName.clear();
			displayName.reserve(16 + std::strlen(originalName));
			displayName.append("lv.");
			displayName.append(std::to_string(g_targetLevel.load(std::memory_order_relaxed)));
			displayName.push_back(' ');
			displayName.append(originalName);
			return displayName.c_str();
		} catch (...) {
			return originalName;
		}
	}

	// Dear ImGui's IM_COL32 packing on little-endian Windows is AABBGGRR.
	constexpr std::uint32_t PackImGuiRGB(std::uint8_t r, std::uint8_t g, std::uint8_t b) noexcept
	{
		return (static_cast<std::uint32_t>(b) << 16) | (static_cast<std::uint32_t>(g) << 8) | static_cast<std::uint32_t>(r);
	}

	std::uint32_t GetTargetThreatTextColor(std::uint32_t originalColor) noexcept
	{
		if (!g_targetThreatColors.load(std::memory_order_relaxed) || !g_targetLevelValid.load(std::memory_order_acquire)) {
			return originalColor;
		}

		auto* player = RE::PlayerCharacter::GetSingleton();
		if (!player) {
			return originalColor;
		}

		const auto playerLevel = static_cast<std::int32_t>(player->GetLevel());
		const auto targetLevel = g_targetLevel.load(std::memory_order_relaxed);
		const auto difference = targetLevel - playerLevel;

		// <= -5: blue-gray; -4..+9: gray; +10..+14: orange;
		// +15..+24: red; >= +25: purple.
		std::uint32_t rgb{};
		if (difference <= -5) {
			rgb = PackImGuiRGB(137, 158, 174);
		} else if (difference >= 25) {
			rgb = PackImGuiRGB(181, 116, 224);
		} else if (difference >= 15) {
			rgb = PackImGuiRGB(226, 69, 69);
		} else if (difference >= 10) {
			rgb = PackImGuiRGB(239, 151, 55);
		} else {
			rgb = PackImGuiRGB(207, 207, 207);
		}

		// Preserve Segmented HUD's original alpha/fade value.
		return (originalColor & 0xFF000000u) | rgb;
	}

	struct HudVec2
	{
		float x;
		float y;
	};

	// Layout recovered from this Segmented HUD build's Dear ImGui 1.92.8:
	// ImVector<float> IndexAdvanceX; float FallbackAdvanceX; float Size;
	struct ImVectorFloatView
	{
		std::int32_t size;
		std::int32_t capacity;
		const float* data;
	};

	struct ImFontBakedView
	{
		ImVectorFloatView indexAdvanceX;
		float fallbackAdvanceX;
		float size;
	};

	static_assert(offsetof(ImFontBakedView, fallbackAdvanceX) == 0x10);
	static_assert(offsetof(ImFontBakedView, size) == 0x14);

	using GetFontBaked_t = const ImFontBakedView*(__fastcall*)(void*, float, float);

	std::optional<float> MeasureAsciiTextWidth(void* font, float fontSize, const char* begin, const char* end) noexcept
	{
		if (!g_segmentedHudBase || !font || !begin || !end || end < begin || !std::isfinite(fontSize) || fontSize <= 0.0f) {
			return std::nullopt;
		}

		const auto getFontBaked = reinterpret_cast<GetFontBaked_t>(g_segmentedHudBase + kGetFontBakedRva);
		const auto* baked = getFontBaked(font, fontSize, -1.0f);
		if (!baked || !baked->indexAdvanceX.data || baked->indexAdvanceX.size <= 0 || !std::isfinite(baked->size) ||
			baked->size <= 0.0f) {
			return std::nullopt;
		}

		const auto scale = fontSize / baked->size;
		float width = 0.0f;
		for (auto* current = begin; current != end; ++current) {
			const auto codepoint = static_cast<unsigned char>(*current);
			if (codepoint >= static_cast<std::uint32_t>(baked->indexAdvanceX.size)) {
				return std::nullopt;  // This prefix is intentionally ASCII-only.
			}

			auto advance = baked->indexAdvanceX.data[codepoint];
			if (!std::isfinite(advance) || advance <= 0.0f) {
				advance = baked->fallbackAdvanceX;
			}
			if (!std::isfinite(advance) || advance < 0.0f) {
				return std::nullopt;
			}
			width += advance * scale;
		}

		if (!std::isfinite(width)) {
			return std::nullopt;
		}
		return width;
	}

	using TargetTextDraw_t = void(__fastcall*)(void* drawContext, void* font, float fontSize, const HudVec2* position,
		std::uint32_t originalColor, const char* textBegin, const char* textEnd, float wrapWidth, const void* clipRect);

	void __fastcall DrawTargetTextSplit(void* drawContext, void* font, float fontSize, const HudVec2* position,
		std::uint32_t originalColor, const char* textBegin, const char* textEnd, float wrapWidth, const void* clipRect) noexcept
	{
		if (!g_segmentedHudBase) {
			return;
		}

		const auto originalDraw = reinterpret_cast<TargetTextDraw_t>(g_segmentedHudBase + kTargetTextDrawRva);

		const auto drawOriginal = [&] {
			originalDraw(drawContext, font, fontSize, position, originalColor, textBegin, textEnd, wrapWidth, clipRect);
		};

		if (!font || !textBegin || !position || !g_targetShowLevel.load(std::memory_order_relaxed) ||
			!g_targetThreatColors.load(std::memory_order_relaxed) || !g_targetLevelValid.load(std::memory_order_acquire)) {
			drawOriginal();
			return;
		}

		const char* end = textEnd;

		if (!end) {
			std::size_t length = 0;
			constexpr std::size_t kMaxTargetLabelLength = 2048;

			while (length < kMaxTargetLabelLength && textBegin[length] != '\0') {
				++length;
			}

			if (length == kMaxTargetLabelLength) {
				drawOriginal();
				return;
			}

			end = textBegin + length;
		}

		if (end - textBegin < 6 || textBegin[0] != 'l' || textBegin[1] != 'v' || textBegin[2] != '.') {
			drawOriginal();
			return;
		}

		const char* separator = textBegin + 3;

		while (separator < end && *separator >= '0' && *separator <= '9') {
			++separator;
		}

		if (separator == textBegin + 3 || separator >= end || *separator != ' ' || separator + 1 >= end) {
			drawOriginal();
			return;
		}

		// IMPORTANT:
		// The second argument is the actual ImFont object.
		const auto suffixPositionOffset = MeasureAsciiTextWidth(font, fontSize, textBegin, separator + 1);

		if (!suffixPositionOffset) {
			drawOriginal();
			return;
		}

		HudVec2 suffixPosition{ position->x + *suffixPositionOffset, position->y };

		const auto prefixColor = GetTargetThreatTextColor(originalColor);

		const auto suffixWrapWidth = wrapWidth > 0.0f ? std::max(0.0f, wrapWidth - *suffixPositionOffset) : wrapWidth;

		// Colored level.
		originalDraw(drawContext, font, fontSize, position, prefixColor, textBegin, separator, wrapWidth, clipRect);

		// Neutral actor name.
		originalDraw(drawContext, font, fontSize, &suffixPosition, originalColor, separator + 1, end, suffixWrapWidth, clipRect);
	}

	constexpr std::uint64_t PackGridArguments(std::uint32_t per, std::uint32_t maxCols) noexcept
	{
		return (static_cast<std::uint64_t>(maxCols) << 32) | per;
	}

	std::uint32_t CeilDivide(std::uint32_t value, std::uint32_t divisor) noexcept
	{
		if (divisor == 0) {
			return 0;
		}
		return value / divisor + (value % divisor != 0 ? 1u : 0u);
	}

	// Computes grid arguments for a single-row bar while preserving the original
	// Segmented HUD PointsPerSeg and MaxPerRow settings.
	// The plugin MaxCells setting is an additional cap; its default is 50.
	std::uint64_t BuildSingleRowGridArguments(std::uint32_t currentMaximum, std::uint32_t originalPer,
		std::uint32_t originalMaxCols) noexcept
	{
		if (currentMaximum == 0) {
			return PackGridArguments(originalPer, originalMaxCols);
		}

		const auto configuredMaxCells =
			std::clamp<std::uint32_t>(g_maxCells.load(std::memory_order_relaxed), kMinMaxCells, kMaxMaxCells);

		// Respect Segmented HUD's MaxPerRow when it supplies a valid value.
		// Otherwise use this plugin's configured MaxCells (default: 50).
		const auto maxCells = originalMaxCols > 0 ? std::min(configuredMaxCells, originalMaxCols) : configuredMaxCells;

		// Keep the original PointsPerSeg value. Increase it only when necessary
		// to fit the complete resource bar into the permitted single row.
		const auto basePer = std::max(originalPer, 1u);
		const auto requiredPer = CeilDivide(currentMaximum, maxCells);
		const auto per = std::max(basePer, requiredPer);

		// Passing the actual cell count as the column count prevents wrapping.
		const auto columns = std::max(1u, CeilDivide(currentMaximum, per));
		return PackGridArguments(per, columns);
	}

	// Called by the Xbyak stub immediately before Segmented HUD's original grid builder.
	// No custom HP/MP/ST proportional scaling is performed here: each bar keeps its
	// own original PointsPerSeg value and is only adjusted if it would wrap to another row.
	__declspec(noinline) std::uint64_t __fastcall ComputeGridArguments(std::uintptr_t stats, std::uintptr_t groupState,
		std::uint32_t originalPer, std::uint32_t originalMaxCols, std::uint32_t currentMaximum) noexcept
	{
		if (g_segmentedHudBase == 0 || stats == 0) {
			return PackGridArguments(originalPer, originalMaxCols);
		}

		const bool isTarget = groupState == g_segmentedHudBase + kTargetStateRva;
		const bool isPlayer = groupState == g_segmentedHudBase + kPlayerStateRva;

		if (!isTarget && !isPlayer) {
			return PackGridArguments(originalPer, originalMaxCols);
		}

		// Enemy: preserve the original multi-row layout when SingleRow is disabled.
		if (isTarget && !g_targetSingleRow.load(std::memory_order_relaxed)) {
			return PackGridArguments(originalPer, originalMaxCols);
		}

		// Player: Enabled controls this plugin's single-row adjustment.
		// When disabled, Segmented HUD receives its original grid arguments unchanged.
		if (isPlayer && !g_enabled.load(std::memory_order_relaxed)) {
			return PackGridArguments(originalPer, originalMaxCols);
		}

		return BuildSingleRowGridArguments(currentMaximum, originalPer, originalMaxCols);
	}

	class GridBuilderStub final : public Xbyak::CodeGenerator
	{
	public:
		GridBuilderStub(std::uintptr_t helperAddress, std::uintptr_t originalGridBuilder) : Xbyak::CodeGenerator(0x100)
		{
			// Entered by CALL at SegmentedHUD.dll+0x22FD5; RSP%16 == 8.
			// 0x48 aligns the stack and provides shadow space, arg #5 and locals.
			sub(rsp, 0x48);

			mov(qword[rsp + 0x20], r9);   // C++ helper argument #5: current maximum
			mov(qword[rsp + 0x28], rcx);  // original output buffer
			mov(qword[rsp + 0x30], r9);   // preserve original R9 for grid builder
			mov(dword[rsp + 0x38], edx);  // original per
			mov(dword[rsp + 0x3C], r8d);  // original maxCols

			// Original caller keeps R14=stats and R15=group state/cache.
			mov(rcx, r14);
			mov(rdx, r15);
			mov(r8d, dword[rsp + 0x38]);
			mov(r9d, dword[rsp + 0x3C]);

			mov(rax, helperAddress);
			call(rax);

			// Packed return: low DWORD=per, high DWORD=maxCols.
			mov(edx, eax);
			shr(rax, 32);
			mov(r8d, eax);

			mov(rcx, qword[rsp + 0x28]);
			mov(r9, qword[rsp + 0x30]);
			add(rsp, 0x48);

			// Tail-jump: the original call's return address is preserved.
			mov(rax, originalGridBuilder);
			jmp(rax);

			ready();
		}
	};

	class TargetNameStub final : public Xbyak::CodeGenerator
	{
	public:
		explicit TargetNameStub(std::uintptr_t helperAddress) : Xbyak::CodeGenerator(0x40)
		{
			// The patched instruction is a CALL. Tail-jump into the helper so its
			// RET returns directly to the original caller.
			mov(rax, helperAddress);
			jmp(rax);
			ready();
		}
	};

	class TargetActorCaptureStub final : public Xbyak::CodeGenerator
	{
	public:
		TargetActorCaptureStub(std::uintptr_t helperAddress, std::uintptr_t originalSetter) : Xbyak::CodeGenerator(0x80)
		{
			// Entered through CALL with the actor/form in RCX. Preserve RCX across
			// our helper call, then tail-jump to Segmented HUD's original setter.
			sub(rsp, 0x28);  // 32-byte shadow space + one qword local; aligned for CALL.
			mov(qword[rsp + 0x20], rcx);

			mov(rax, helperAddress);
			call(rax);

			mov(rcx, qword[rsp + 0x20]);
			add(rsp, 0x28);
			mov(rax, originalSetter);
			jmp(rax);
			ready();
		}
	};

	class TargetTextDrawStub final : public Xbyak::CodeGenerator
	{
	public:
		explicit TargetTextDrawStub(std::uintptr_t helperAddress) : Xbyak::CodeGenerator(0x40)
		{
			// Preserve the original call-site ABI and let C++ draw the level prefix
			// and actor name separately through Segmented HUD's original text helper.
			mov(rax, helperAddress);
			jmp(rax);
			ready();
		}
	};

	bool InstallHook()
	{
		if (g_hookInstalled) {
			return true;
		}

		std::uintptr_t moduleBase{};
		if (!ValidateSegmentedHud(moduleBase)) {
			return false;
		}

		g_segmentedHudBase = moduleBase;

		try {
			// This trampoline must be allocated near SegmentedHUD.dll, not Skyrim.exe,
			// because all patched CALL rel32 instructions are inside that DLL.
			constexpr std::size_t kLocalTrampolineSize = 0x1000;

			const auto gridPatch = moduleBase + kGridBuilderCallRva;
			const auto actorPatch1 = moduleBase + kTargetActorSetterCall1Rva;
			const auto actorPatch2 = moduleBase + kTargetActorSetterCall2Rva;
			const auto namePatch = moduleBase + kTargetNameCallRva;
			const auto textDrawPatch = moduleBase + kTargetTextDrawCallRva;

			// Each trampoline is allocated relative to its own CALL instruction.
			if (g_gridTrampoline.empty()) {
				g_gridTrampoline.create(kLocalTrampolineSize, reinterpret_cast<void*>(gridPatch));
			}

			if (g_targetActorTrampoline1.empty()) {
				g_targetActorTrampoline1.create(kLocalTrampolineSize, reinterpret_cast<void*>(actorPatch1));
			}

			if (g_targetActorTrampoline2.empty()) {
				g_targetActorTrampoline2.create(kLocalTrampolineSize, reinterpret_cast<void*>(actorPatch2));
			}

			if (g_targetNameTrampoline.empty()) {
				g_targetNameTrampoline.create(kLocalTrampolineSize, reinterpret_cast<void*>(namePatch));
			}

			if (g_targetTextDrawTrampoline.empty()) {
				g_targetTextDrawTrampoline.create(kLocalTrampolineSize, reinterpret_cast<void*>(textDrawPatch));
			}

			LogInfo("Five local Segmented HUD trampolines initialized.");

			const auto gridBuilderAddress = moduleBase + kGridBuilderRva;
			const auto targetActorSetterAddress = moduleBase + kTargetActorSetterRva;

			const auto gridHelperAddress = reinterpret_cast<std::uintptr_t>(&ComputeGridArguments);
			const auto captureHelperAddress = reinterpret_cast<std::uintptr_t>(&CaptureTargetActorLevel);
			const auto drawHelperAddress = reinterpret_cast<std::uintptr_t>(&DrawTargetTextSplit);

			GridBuilderStub gridStub(gridHelperAddress, gridBuilderAddress);
			TargetActorCaptureStub targetActorStub(captureHelperAddress, targetActorSetterAddress);

			TargetNameStub nameStub(reinterpret_cast<std::uintptr_t>(&GetTargetNameWithLevel));
			TargetTextDrawStub textDrawStub(drawHelperAddress);

			auto* gridCode = g_gridTrampoline.allocate(gridStub.getSize());
			auto* actorCode1 = g_targetActorTrampoline1.allocate(targetActorStub.getSize());
			auto* actorCode2 = g_targetActorTrampoline2.allocate(targetActorStub.getSize());
			auto* nameCode = g_targetNameTrampoline.allocate(nameStub.getSize());
			auto* textDrawCode = g_targetTextDrawTrampoline.allocate(textDrawStub.getSize());

			if (!gridCode || !actorCode1 || !actorCode2 || !nameCode || !textDrawCode) {
				g_segmentedHudBase = 0;
				LogWarn("Failed to allocate one or more local trampolines.");
				return false;
			}

			std::memcpy(gridCode, gridStub.getCode(), gridStub.getSize());
			std::memcpy(actorCode1, targetActorStub.getCode(), targetActorStub.getSize());
			std::memcpy(actorCode2, targetActorStub.getCode(), targetActorStub.getSize());
			std::memcpy(nameCode, nameStub.getCode(), nameStub.getSize());
			std::memcpy(textDrawCode, textDrawStub.getCode(), textDrawStub.getSize());

			LogInfo("Installing grid hook...");
			const auto gridOriginal = g_gridTrampoline.write_call<5>(gridPatch, reinterpret_cast<std::uintptr_t>(gridCode));

			LogInfo("Installing target actor hook 1...");
			const auto actorOriginal1 =
				g_targetActorTrampoline1.write_call<5>(actorPatch1, reinterpret_cast<std::uintptr_t>(actorCode1));

			LogInfo("Installing target actor hook 2...");
			const auto actorOriginal2 =
				g_targetActorTrampoline2.write_call<5>(actorPatch2, reinterpret_cast<std::uintptr_t>(actorCode2));

			LogInfo("Installing target name hook...");
			const auto nameOriginal = g_targetNameTrampoline.write_call<5>(namePatch, reinterpret_cast<std::uintptr_t>(nameCode));

			LogInfo("Installing target text draw hook...");
			const auto textDrawOriginal =
				g_targetTextDrawTrampoline.write_call<5>(textDrawPatch, reinterpret_cast<std::uintptr_t>(textDrawCode));

			if (gridOriginal != gridBuilderAddress || actorOriginal1 != targetActorSetterAddress ||
				actorOriginal2 != targetActorSetterAddress || nameOriginal != moduleBase + kTargetNameGetterRva ||
				textDrawOriginal != moduleBase + kTargetTextDrawRva) {
				LogWarn("One of the patched call sites returned an unexpected original target.");
				return false;
			}

			g_hookInstalled = true;
			LogInfo("Installed player scaling and Segmented HUD internal target hooks.");
			LogInfo("Enemy label format: lv.<level> <actor name>; threat color applies to the level prefix only.");
			LogInfo("Enemy actor level is captured from Target::HitSink; no crosshair lookup is used.");
			return true;
		} catch (const std::exception& e) {
			g_segmentedHudBase = 0;
			LogWarn(std::string("Failed to install hook: ") + e.what());
			return false;
		} catch (...) {
			g_segmentedHudBase = 0;
			LogWarn("Failed to install hook due to an unknown exception.");
			return false;
		}
	}
}

namespace VirtualAmmo
{
	namespace
	{
		// Actor/TESObjectREFR virtual function slots from CommonLibSSE.
		constexpr std::size_t kRemoveItemVFunc = 0x56;
		constexpr std::size_t kGetCurrentAmmoVFunc = 0x9E;

		// The catalog intentionally prefers the exact material keyword first.
		// Rank is only used as a fallback when a matching AMMO record does not exist.
		enum class Material : std::uint8_t
		{
			kIron,
			kSteel,
			kOrcish,
			kElven,
			kDwarven,
			kNordic,
			kFalmer,
			kFalmerHoned,
			kGlass,
			kStalhrim,
			kEbony,
			kDaedric,
			kDragonbone,
			kTotal
		};

		struct MaterialInfo
		{
			Material material;
			std::string_view keyword;
			std::uint8_t rank;
		};

		// Higher-rank materials are tested first so unusual records carrying more
		// than one material keyword resolve deterministically. Materials without a
		// dedicated vanilla projectile map to the nearest supported projectile tier.
		constexpr std::array<MaterialInfo, 22> kMaterials{ { { Material::kDragonbone, "DLC1WeapMaterialDragonbone", 7 },
			{ Material::kDragonbone, "WeapMaterialDragonbone", 7 }, { Material::kDaedric, "WeapMaterialDaedric", 7 },
			{ Material::kEbony, "WeapMaterialEbony", 6 }, { Material::kStalhrim, "DLC2WeaponMaterialStalhrim", 5 },
			{ Material::kStalhrim, "WeapMaterialStalhrim", 5 }, { Material::kGlass, "WeapMaterialGlass", 5 },
			{ Material::kFalmerHoned, "WeapMaterialFalmerHoned", 4 }, { Material::kNordic, "DLC2WeaponMaterialNordic", 4 },
			{ Material::kNordic, "WeapMaterialNordic", 4 }, { Material::kFalmer, "WeapMaterialFalmer", 3 },
			{ Material::kDwarven, "WeapDwarvenCrossbow", 3 }, { Material::kDwarven, "WeapMaterialDwarven", 3 },
			{ Material::kElven, "WeapMaterialElven", 3 }, { Material::kOrcish, "WeapMaterialOrcish", 3 },
			{ Material::kSteel, "WeapMaterialSilver", 2 }, { Material::kSteel, "WeapMaterialImperial", 2 },
			{ Material::kSteel, "WeapMaterialDraugrHoned", 2 }, { Material::kSteel, "WeapMaterialSteel", 2 },
			{ Material::kIron, "WeapMaterialDraugr", 1 }, { Material::kIron, "WeapMaterialWood", 1 },
			{ Material::kIron, "WeapMaterialIron", 1 } } };

		enum class AmmoKind : std::uint8_t
		{
			kArrow,
			kBolt
		};

		// [arrow/bolt][material]
		std::array<std::array<RE::TESAmmo*, static_cast<std::size_t>(Material::kTotal)>, 2> g_ammoCatalog{};
		bool g_initialized = false;
		bool g_hooksInstalled = false;
		std::atomic_uint32_t g_ammoOverrideLogCount{ 0 };
		std::atomic_uint32_t g_suppressedRemovalLogCount{ 0 };
		std::atomic_uint32_t g_visualArrowLogCount{ 0 };
		std::atomic_uint32_t g_ammoModelLoadLogCount{ 0 };
		std::mutex g_ammoModelCacheMutex;
		std::unordered_map<RE::FormID, RE::NiPointer<RE::NiNode>> g_ammoModelCache;

		using AttachArrowFunc = void(RE::Actor*, const RE::BSTSmartPointer<RE::BipedAnim>&);
		REL::Relocation<AttachArrowFunc> g_originalAttachArrow;

		constexpr std::size_t ToIndex(Material a_material) noexcept { return static_cast<std::size_t>(a_material); }

		constexpr std::size_t ToIndex(AmmoKind a_kind) noexcept { return static_cast<std::size_t>(a_kind); }

		std::optional<Material> TryDetectMaterial(RE::TESForm* a_form)
		{
			if (!a_form) {
				return std::nullopt;
			}

			for (const auto& info : kMaterials) {
				if (a_form->HasKeywordByEditorID(info.keyword)) {
					return info.material;
				}
			}

			return std::nullopt;
		}

		Material DetectAmmoMaterial(RE::TESAmmo* a_ammo)
		{
			// Called only after HasKnownMaterial() succeeds.
			return TryDetectMaterial(a_ammo).value_or(Material::kIron);
		}

		bool HasKnownMaterial(RE::TESForm* a_form)
		{
			if (!a_form) {
				return false;
			}

			for (const auto& info : kMaterials) {
				if (a_form->HasKeywordByEditorID(info.keyword)) {
					return true;
				}
			}

			return false;
		}

		std::uint8_t GetMaterialRank(Material a_material)
		{
			for (const auto& info : kMaterials) {
				if (info.material == a_material) {
					return info.rank;
				}
			}
			return 1;
		}

		void BuildAmmoCatalog()
		{
			for (auto& byKind : g_ammoCatalog) {
				byKind.fill(nullptr);
			}

			auto* dataHandler = RE::TESDataHandler::GetSingleton();
			if (!dataHandler) {
				SKSE::log::error("VirtualAmmo: TESDataHandler is unavailable; catalog was not built.");
				return;
			}

			std::size_t candidates = 0;
			for (auto* ammo : dataHandler->GetFormArray<RE::TESAmmo>()) {
				if (!ammo || !ammo->GetPlayable() || !ammo->data.projectile || !HasKnownMaterial(ammo)) {
					continue;
				}

				const auto kind = ammo->IsBolt() ? AmmoKind::kBolt : AmmoKind::kArrow;
				const auto material = DetectAmmoMaterial(ammo);
				auto& slot = g_ammoCatalog[ToIndex(kind)][ToIndex(material)];

				// When plugins add duplicate AMMO records for the same material,
				// prefer the lowest FormID (normally the original/master record).
				if (!slot || ammo->GetFormID() < slot->GetFormID()) {
					slot = ammo;
				}
				++candidates;
			}

			// Some vanilla AMMO records do not carry reliable material keywords.
			// Resolve those explicitly by plugin + local FormID as a safe fallback.
			struct VanillaFallback
			{
				Material material;
				AmmoKind kind;
				RE::FormID localFormID;
				std::string_view plugin;
			};
			constexpr std::array<VanillaFallback, 14> kVanillaFallbacks{
				{ { Material::kIron, AmmoKind::kArrow, 0x0001397D, "Skyrim.esm" },
					{ Material::kSteel, AmmoKind::kArrow, 0x0001397F, "Skyrim.esm" },
					{ Material::kOrcish, AmmoKind::kArrow, 0x000139BB, "Skyrim.esm" },
					{ Material::kDwarven, AmmoKind::kArrow, 0x000139BC, "Skyrim.esm" },
					{ Material::kElven, AmmoKind::kArrow, 0x000139BD, "Skyrim.esm" },
					{ Material::kGlass, AmmoKind::kArrow, 0x000139BE, "Skyrim.esm" },
					{ Material::kEbony, AmmoKind::kArrow, 0x000139BF, "Skyrim.esm" },
					{ Material::kDaedric, AmmoKind::kArrow, 0x000139C0, "Skyrim.esm" },
					{ Material::kFalmer, AmmoKind::kArrow, 0x00038341, "Skyrim.esm" },
					{ Material::kDragonbone, AmmoKind::kArrow, 0x000176F4, "Dawnguard.esm" },
					{ Material::kNordic, AmmoKind::kArrow, 0x0002623B, "Dragonborn.esm" },
					{ Material::kStalhrim, AmmoKind::kArrow, 0x00026239, "Dragonborn.esm" },
					{ Material::kSteel, AmmoKind::kBolt, 0x00000BB3, "Dawnguard.esm" },
					{ Material::kDwarven, AmmoKind::kBolt, 0x0000D099, "Dawnguard.esm" } }
			};

			for (const auto& fallback : kVanillaFallbacks) {
				auto& slot = g_ammoCatalog[ToIndex(fallback.kind)][ToIndex(fallback.material)];
				if (auto* ammo = dataHandler->LookupForm<RE::TESAmmo>(fallback.localFormID, fallback.plugin);
					ammo && ammo->GetPlayable() && ammo->data.projectile &&
					(ammo->IsBolt() == (fallback.kind == AmmoKind::kBolt))) {
					slot = ammo;
					++candidates;
				}
			}

			SKSE::log::info("VirtualAmmo: catalog built from {} playable material AMMO records (including vanilla fallbacks).",
				candidates);
			for (std::size_t kind = 0; kind < g_ammoCatalog.size(); ++kind) {
				for (const auto& info : kMaterials) {
					const auto* ammo = g_ammoCatalog[kind][ToIndex(info.material)];
					if (ammo) {
						SKSE::log::debug("VirtualAmmo: {} {} -> {:08X} ({})", kind == ToIndex(AmmoKind::kBolt) ? "bolt" : "arrow",
							info.keyword, ammo->GetFormID(), ammo->GetName());
					}
				}
			}
		}

		RE::TESAmmo* FindAmmo(AmmoKind a_kind, Material a_material)
		{
			const auto kindIndex = ToIndex(a_kind);
			if (kindIndex >= g_ammoCatalog.size()) {
				return nullptr;
			}

			if (auto* exact = g_ammoCatalog[kindIndex][ToIndex(a_material)]) {
				return exact;
			}

			// Fall back to the best available material not stronger than the bow.
			// This avoids assigning a high-tier projectile to a low-tier weapon.
			const auto desiredRank = GetMaterialRank(a_material);
			for (int rank = static_cast<int>(desiredRank); rank >= 1; --rank) {
				for (const auto& info : kMaterials) {
					if (info.rank == rank) {
						if (auto* candidate = g_ammoCatalog[kindIndex][ToIndex(info.material)]) {
							return candidate;
						}
					}
				}
			}

			// Vanilla has no iron bolt. If a custom low-tier crossbow is encountered,
			// steel is the lowest available bolt and is safer than reverting to a
			// real inventory requirement.
			if (a_kind == AmmoKind::kBolt) {
				return g_ammoCatalog[kindIndex][ToIndex(Material::kSteel)];
			}

			return nullptr;
		}

		RE::TESObjectWEAP* GetRangedWeapon(const RE::Actor* a_actor)
		{
			if (!a_actor) {
				return nullptr;
			}

			// Bows and crossbows are normally held in the right-hand process slot.
			// Check both slots to remain tolerant of unusual custom equipment setups.
			for (const bool leftHand : { false, true }) {
				auto* equipped = a_actor->GetEquippedObject(leftHand);
				auto* weapon = equipped ? equipped->As<RE::TESObjectWEAP>() : nullptr;
				if (weapon) {
					const auto type = weapon->GetWeaponType();
					if (type == RE::WEAPON_TYPE::kBow || type == RE::WEAPON_TYPE::kCrossbow) {
						return weapon;
					}
				}
			}

			return nullptr;
		}

		bool IsCatalogAmmo(const RE::TESAmmo* a_ammo)
		{
			if (!a_ammo) {
				return false;
			}

			for (const auto& byKind : g_ammoCatalog) {
				if (std::ranges::find(byKind, a_ammo) != byKind.end()) {
					return true;
				}
			}

			return false;
		}

		RE::TESAmmo* SelectVirtualAmmo(const RE::Actor* a_actor)
		{
			auto* weapon = GetRangedWeapon(a_actor);
			if (!weapon || weapon->IsBound()) {
				return nullptr;
			}

			// Unknown/custom materials use the lowest supported projectile tier.
			// This is deliberate: standard AMMO records are zeroed after their damage
			// is moved to weapons, so leaving an ordinary bow on engine ammo would
			// silently remove its projectile damage.
			const auto material = TryDetectMaterial(weapon).value_or(Material::kIron);

			const auto kind = weapon->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow ? AmmoKind::kBolt : AmmoKind::kArrow;
			return FindAmmo(kind, material);
		}

		bool IsInventoryMutationMenuOpen()
		{
			auto* ui = RE::UI::GetSingleton();
			if (!ui) {
				return false;
			}

			// Do not suppress manual dropping, trading, gifting, or container transfers.
			for (const auto menu : { "InventoryMenu", "ContainerMenu", "BarterMenu", "GiftMenu", "FavoritesMenu", "Crafting Menu",
					 "Dialogue Menu" }) {
				if (ui->IsMenuOpen(menu)) {
					return true;
				}
			}
			return false;
		}

		void TransferAmmoDamageToWeapons()
		{
			auto* dataHandler = RE::TESDataHandler::GetSingleton();
			if (!dataHandler) {
				return;
			}

			std::unordered_set<RE::TESAmmo*> usedAmmo;
			std::size_t changedWeapons = 0;

			for (auto* weapon : dataHandler->GetFormArray<RE::TESObjectWEAP>()) {
				if (!weapon || weapon->IsBound()) {
					continue;
				}

				const auto type = weapon->GetWeaponType();
				if (type != RE::WEAPON_TYPE::kBow && type != RE::WEAPON_TYPE::kCrossbow) {
					continue;
				}

				// Unknown/custom materials mirror SelectVirtualAmmo() and use iron.
				const auto detectedMaterial = TryDetectMaterial(weapon);
				const auto material = detectedMaterial.value_or(Material::kIron);
				if (!detectedMaterial) {
					SKSE::log::warn("VirtualAmmo: weapon {:08X} has no supported material keyword; using iron projectile.",
						weapon->GetFormID());
				}

				const auto kind = type == RE::WEAPON_TYPE::kCrossbow ? AmmoKind::kBolt : AmmoKind::kArrow;
				auto* ammo = FindAmmo(kind, material);
				if (!ammo || !std::isfinite(ammo->data.damage) || ammo->data.damage <= 0.0f) {
					continue;
				}

				const auto ammoDamage = ammo->data.damage;
				const auto transferred = static_cast<std::uint32_t>(std::clamp(std::lround(ammoDamage), 0L, 65535L));
				const auto newDamage = std::min<std::uint32_t>(static_cast<std::uint32_t>(weapon->attackDamage) + transferred,
					std::numeric_limits<std::uint16_t>::max());
				weapon->attackDamage = static_cast<std::uint16_t>(newDamage);
				SKSE::log::debug("VirtualAmmo: weapon {:08X} uses AMMO {:08X}; +{} damage, weapon base now {}.",
					weapon->GetFormID(), ammo->GetFormID(), transferred, weapon->attackDamage);
				usedAmmo.insert(ammo);
				++changedWeapons;
			}

			// Zero only the standard material AMMO records actually selected by a
			// supported bow/crossbow. Unique non-material and special ammo stays intact.
			for (auto* ammo : usedAmmo) {
				SKSE::log::debug("VirtualAmmo: moved {} damage from AMMO {:08X} into supported weapons.", ammo->data.damage,
					ammo->GetFormID());
				ammo->data.damage = 0.0f;
			}

			SKSE::log::info(
				"VirtualAmmo: transferred standard projectile damage to {} bow/crossbow records; {} AMMO records set to zero.",
				changedWeapons, usedAmmo.size());
		}

		// Skyrim's ArrowReleaseHandler has two call sites which attach the nocked arrow.
		// VirtualAmmo has a TESAmmo record but intentionally has no InventoryEntryData.
		// Do not depend on the actor's equipped-quiver node: load the selected ammo NIF
		// directly, clone only its Arrow:0 shape, and attach that shape to the bow/bolt node.
		bool HasCurrentAmmoEntry(const RE::AIProcess* a_process) noexcept
		{
			// In po3 CommonLibSSE, the current ammo InventoryEntryData is exposed
			// as MiddleHighProcessData::bothHands (not AIProcess::GetCurrentAmmo()).
			return a_process && a_process->middleHigh && a_process->middleHigh->bothHands;
		}

		RE::NiNode* GetVirtualAmmoSourceNode(RE::AIProcess* a_process, const RE::BSTSmartPointer<RE::BipedAnim>& a_biped)
		{
			if (!a_process || !a_biped) {
				return nullptr;
			}

			using func_t = RE::NiNode*(RE::AIProcess*, const RE::BSTSmartPointer<RE::BipedAnim>&);
			static REL::Relocation<func_t> getAmmoSourceNode{ REL::ID(38801) };
			return getAmmoSourceNode(a_process, a_biped);
		}

		RE::NiNode* GetAmmoModelRoot(RE::TESAmmo* a_ammo)
		{
			if (!a_ammo) {
				return nullptr;
			}

			const auto formID = a_ammo->GetFormID();
			{
				std::scoped_lock lock(g_ammoModelCacheMutex);
				if (const auto it = g_ammoModelCache.find(formID); it != g_ammoModelCache.end()) {
					return it->second.get();
				}
			}

			const char* modelPath = a_ammo->GetModel();
			if (!modelPath || modelPath[0] == '\0') {
				if (g_ammoModelLoadLogCount.fetch_add(1, std::memory_order_relaxed) < 8) {
					SKSE::log::warn("VirtualAmmo: AMMO {:08X} has no model path.", formID);
				}
				return nullptr;
			}

			RE::NiPointer<RE::NiNode> modelRoot;
			constexpr RE::BSModelDB::DBTraits::ArgsType args{};
			const auto result = RE::BSModelDB::Demand(modelPath, modelRoot, args);
			if (result != RE::BSResource::ErrorCode::kNone || !modelRoot) {
				if (g_ammoModelLoadLogCount.fetch_add(1, std::memory_order_relaxed) < 8) {
					SKSE::log::warn("VirtualAmmo: failed to load AMMO model {:08X}, path='{}', error={}, root={}", formID,
						modelPath, static_cast<int>(result), modelRoot != nullptr);
				}
				return nullptr;
			}

			auto* loadedRoot = modelRoot.get();
			{
				std::scoped_lock lock(g_ammoModelCacheMutex);
				auto [it, inserted] = g_ammoModelCache.emplace(formID, std::move(modelRoot));
				(void)inserted;
				loadedRoot = it->second.get();
			}

			if (g_ammoModelLoadLogCount.fetch_add(1, std::memory_order_relaxed) < 8) {
				SKSE::log::info("VirtualAmmo: loaded AMMO model {:08X} from '{}'.", formID, modelPath);
			}
			return loadedRoot;
		}

		void AttachVirtualAmmoArrow(RE::Actor* a_actor, const RE::BSTSmartPointer<RE::BipedAnim>& a_biped)
		{
			if (a_actor && a_actor->IsPlayerRef()) {
				auto* process = a_actor->currentProcess;
				auto* ammo = SelectVirtualAmmo(a_actor);

				// If AIProcess already has a real ammo inventory entry, let Skyrim handle
				// the native arrow attachment. Only synthesize the visual node when the
				// virtual-ammo path has no InventoryEntryData to drive it.
				if (process && ammo && !HasCurrentAmmoEntry(process) && ammo->GetPlayable()) {
					auto* modelRoot = GetAmmoModelRoot(ammo);
					auto* fixedStrings = RE::FixedStrings::GetSingleton();
					auto* taskQueue = RE::TaskQueueInterface::GetSingleton();

					RE::NiNode* attachNode = nullptr;
					if (ammo->data.flags.any(RE::AMMO_DATA::Flag::kNonBolt)) {
						if (auto* weaponNode = process->GetWeaponNode(a_biped)) {
							attachNode = weaponNode->AsNode();
						}
					} else if (auto* magicNode = process->GetMagicNode(a_biped)) {
						attachNode = magicNode->AsNode();
					}

					// Prefer the Arrow:0 shape from the AMMO's own NIF, independently
					// of any quiver currently equipped on the actor. The old helper-38801
					// source is retained only as a fallback for unusual model layouts.
					RE::NiAVObject* arrowNode =
						modelRoot && fixedStrings ? modelRoot->GetObjectByName(fixedStrings->arrow0) : nullptr;
					const char* arrowSource = arrowNode ? "AMMO NIF" : "actor ammo node fallback";
					if (!arrowNode && fixedStrings) {
						if (auto* actorAmmoNode = GetVirtualAmmoSourceNode(process, a_biped)) {
							arrowNode = actorAmmoNode->GetObjectByName(fixedStrings->arrow0);
						}
					}

					if (arrowNode && attachNode && taskQueue) {
						// Arrow:0 is a separate scene object in the ammo NIF. Clone only it;
						// never attach the NIF root, so its quiver geometry is not displayed.
						auto* arrowClone = static_cast<RE::NiAVObject*>(arrowNode->Clone());
						if (arrowClone) {
							taskQueue->QueueNodeAttach(arrowClone, attachNode);
							if (g_visualArrowLogCount.fetch_add(1, std::memory_order_relaxed) < 8) {
								SKSE::log::info(
									"VirtualAmmo: queued visual Arrow:0 actor={:08X}, AMMO={:08X}, source={}, modelRoot={}, "
								    "attachNode={}",
									a_actor->GetFormID(), ammo->GetFormID(), arrowSource, modelRoot != nullptr,
									attachNode != nullptr);
							}
							return;
						}
					}

					if (g_visualArrowLogCount.fetch_add(1, std::memory_order_relaxed) < 8) {
						SKSE::log::warn(
							"VirtualAmmo: visual Arrow:0 not queued actor={:08X}, AMMO={:08X}, processAmmoEntry={}, "
						    "modelRoot={}, attachNode={}, arrowNode={}, fixedStrings={}, taskQueue={}",
							a_actor->GetFormID(), ammo->GetFormID(), HasCurrentAmmoEntry(process), modelRoot != nullptr,
							attachNode != nullptr, arrowNode != nullptr, fixedStrings != nullptr, taskQueue != nullptr);
					}
				}
			}

			// Preserve Skyrim's original path for non-player actors, real ammo entries,
			// bound weapons, and every fallback case where our visual clone was not queued.
			g_originalAttachArrow(a_actor, a_biped);
		}

		void InstallVisualArrowHook()
		{
			auto& trampoline = SKSE::GetTrampoline();
			const auto handler = REL::ID(40250).address();

			// Both call sites are used by ArrowReleaseHandler for the nocked-arrow
			// visual lifecycle. Keep the original function target so the fallback can
			// invoke Skyrim's real Actor::AttachArrow implementation.
			trampoline.write_call<5>(handler + 0x75, AttachVirtualAmmoArrow);
			g_originalAttachArrow = trampoline.write_branch<5>(handler + 0x9D, AttachVirtualAmmoArrow);

			SKSE::log::info("VirtualAmmo: ArrowReleaseHandler visual-arrow hooks installed.");
		}

		template <class TActor>
		struct GetCurrentAmmoHook
		{
			static RE::TESAmmo* thunk(const RE::Actor* a_actor)
			{
				auto* currentAmmo = func(a_actor);
				auto* virtualAmmo = SelectVirtualAmmo(a_actor);

				// Preserve explicitly equipped special/custom ammo when it has no
				// material keyword. Standard material arrows/bolts are virtualized.
				if (currentAmmo && currentAmmo != virtualAmmo && !IsCatalogAmmo(currentAmmo) && !HasKnownMaterial(currentAmmo)) {
					return currentAmmo;
				}

				if (virtualAmmo) {
					if (g_ammoOverrideLogCount.fetch_add(1, std::memory_order_relaxed) < 8) {
						SKSE::log::info("VirtualAmmo: GetCurrentAmmo actor={:08X} -> AMMO={:08X}",
							a_actor ? a_actor->GetFormID() : 0, virtualAmmo->GetFormID());
					}
					return virtualAmmo;
				}

				return currentAmmo;
			}

			static inline REL::Relocation<decltype(thunk)> func;

			static void Install()
			{
				func = REL::Relocation<std::uintptr_t>(TActor::VTABLE[0]).write_vfunc(kGetCurrentAmmoVFunc, thunk);
				SKSE::log::info("VirtualAmmo: GetCurrentAmmo hook installed.");
			}
		};

		template <class TActor>
		struct RemoveAmmoHook
		{
			static RE::ObjectRefHandle thunk(RE::Actor* a_actor, RE::TESBoundObject* a_item, std::int32_t a_count,
				RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef,
				const RE::NiPoint3* a_dropLoc, const RE::NiPoint3* a_rotate)
			{
				if (a_actor && a_item && a_count == 1 && a_reason == RE::ITEM_REMOVE_REASON::kRemove && a_moveToRef == nullptr &&
					!IsInventoryMutationMenuOpen()) {
					if (auto* ammo = a_item->As<RE::TESAmmo>(); ammo && ammo == SelectVirtualAmmo(a_actor)) {
						// Ammo consumption by the firing path is a plain kRemove. Keep
						// item-management reasons untouched, so users can still drop/store ammo.
						if (g_suppressedRemovalLogCount.fetch_add(1, std::memory_order_relaxed) < 8) {
							SKSE::log::info("VirtualAmmo: suppressed shot ammo removal actor={:08X}, AMMO={:08X}",
								a_actor->GetFormID(), ammo->GetFormID());
						}
						return {};
					}
				}

				return func(a_actor, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
			}

			static inline REL::Relocation<decltype(thunk)> func;

			static void Install()
			{
				func = REL::Relocation<std::uintptr_t>(TActor::VTABLE[0]).write_vfunc(kRemoveItemVFunc, thunk);
				SKSE::log::info("VirtualAmmo: RemoveItem hook installed.");
			}
		};

		void InstallHooks()
		{
			// Most humanoid NPCs use Character; the player has a dedicated vtable.
			GetCurrentAmmoHook<RE::Character>::Install();
			RemoveAmmoHook<RE::Character>::Install();
			GetCurrentAmmoHook<RE::PlayerCharacter>::Install();
			RemoveAmmoHook<RE::PlayerCharacter>::Install();
			InstallVisualArrowHook();
		}
	}

	void Initialize()
	{
		if (g_initialized) {
			return;
		}

		BuildAmmoCatalog();
		TransferAmmoDamageToWeapons();

		if (!g_hooksInstalled) {
			try {
				InstallHooks();
				g_hooksInstalled = true;
			} catch (const std::exception& e) {
				SKSE::log::critical("VirtualAmmo: failed to install hooks: {}", e.what());
				return;
			} catch (...) {
				SKSE::log::critical("VirtualAmmo: failed to install hooks due to an unknown exception.");
				return;
			}
		}

		g_initialized = true;
		SKSE::log::info(
			"VirtualAmmo: initialization completed. NPC weapon flags are preserved; bound weapons are left unchanged.");
	}
}

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	if (!message) {
		return;
	}

	switch (message->type) {
	case SKSE::MessagingInterface::kPostLoad:
		SKSE::GetTaskInterface()->AddTask([] { UpdateValues("POST LOAD"); });
		LogInfo("Received SKSE kPostLoad; checking SegmentedHUD.dll.");
		if (!InstallHook()) {
			LogWarn("Segmented HUD compatibility hooks are inactive. The original HUD remains unmodified by this plugin.");
		}
		break;

	case SKSE::MessagingInterface::kDataLoaded:

		PrismaUICode::LoadHudSettings();
		PrismaUICode::TryRegisterHudSettingsMenu();
		PrismaUICode::InitializePrismaUI();

		Event::Initialize();
		VirtualHook::Initialize();
		Hook::Initialize();

		VirtualAmmo::Initialize();
		break;

	case SKSE::MessagingInterface::kPreLoadGame:
		PrismaUICode::g_playerWorldReady.store(false);
		PrismaUICode::HideHUD();
		break;

	case SKSE::MessagingInterface::kPostLoadGame:
		SKSE::log::info("PrismaUICode: Save loaded");

		PrismaUICode::g_playerWorldReady.store(false);
		break;

	case SKSE::MessagingInterface::kNewGame:
		SKSE::log::info("PrismaUICode: New game");

		PrismaUICode::g_playerWorldReady.store(false);
		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	auto g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(a_skse->QueryInterface(SKSE::LoadInterface::kMessaging));
	if (!g_messaging) {
		SKSE::log::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");
		return false;
	}

	SKSE::log::info("loaded");

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1 << 10);

	g_messaging->RegisterListener("SKSE", SKSEMessageHandler);

	return true;
}
//reqs
//prismaui
//segmented hud
