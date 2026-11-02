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

namespace GameSettings
{
	auto GSC = RE::GameSettingCollection::GetSingleton();
	
	RE::Setting* fDiffMultHPByPCVE = GSC->GetSetting("fDiffMultHPByPCVE");
	RE::Setting* fDiffMultHPByPCE = GSC->GetSetting("fDiffMultHPByPCE");
	RE::Setting* fDiffMultHPByPCN = GSC->GetSetting("fDiffMultHPByPCN");
	RE::Setting* fDiffMultHPByPCH = GSC->GetSetting("fDiffMultHPByPCH");
	RE::Setting* fDiffMultHPByPCVH = GSC->GetSetting("fDiffMultHPByPCVH");
	RE::Setting* fDiffMultHPByPCL = GSC->GetSetting("fDiffMultHPByPCL");
	
	RE::Setting* fDiffMultHPToPCVE = GSC->GetSetting("fDiffMultHPToPCVE");
	RE::Setting* fDiffMultHPToPCE = GSC->GetSetting("fDiffMultHPToPCE");
	RE::Setting* fDiffMultHPToPCN = GSC->GetSetting("fDiffMultHPToPCN");
	RE::Setting* fDiffMultHPToPCH = GSC->GetSetting("fDiffMultHPToPCH");
	RE::Setting* fDiffMultHPToPCVH = GSC->GetSetting("fDiffMultHPToPCVH");
	RE::Setting* fDiffMultHPToPCL = GSC->GetSetting("fDiffMultHPToPCL");

	RE::Setting* fArmorRatingScalingFactor = GSC->GetSetting("fArmorScalingFactor");
	RE::Setting* fMaxArmorRating = GSC->GetSetting("fMaxArmorRating");

	RE::Setting* fPlayerMaxResistance = GSC->GetSetting("fPlayerMaxResistance");

	RE::Setting* fXPPerSkillRank = GSC->GetSetting("fXPPerSkillRank");
	RE::Setting* fXPLevelUpBase = GSC->GetSetting("fXPLevelUpBase");
	RE::Setting* fXPLevelUpMult = GSC->GetSetting("fXPLevelUpMult");

	RE::Setting* iTrainingNumAllowedPerLevel = GSC->GetSetting("iTrainingNumAllowedPerLevel");

	RE::Setting* fCombatDistance = GSC->GetSetting("fCombatDistance");
	RE::Setting* fCombatMagickaRegenRateMult = GSC->GetSetting("fCombatMagickaRegenRateMult");
	RE::Setting* fCombatStaminaRegenRateMult = GSC->GetSetting("fCombatStaminaRegenRateMult");

	RE::Setting* fDamagePCSkillMax = GSC->GetSetting("fDamagePCSkillMax");
	RE::Setting* fDamagePCSkillMin = GSC->GetSetting("fDamagePCSkillMin");

	RE::Setting* fDamageSkillMax = GSC->GetSetting("fDamageSkillMax");
	RE::Setting* fDamageSkillMin = GSC->GetSetting("fDamageSkillMin");

	RE::Setting* fSprintStaminaDrainMult = GSC->GetSetting("fSprintStaminaDrainMult");

	RE::Setting* fStaminaAttackWeaponBase = GSC->GetSetting("fStaminaAttackWeaponBase");
	RE::Setting* fStaminaAttackWeaponMult = GSC->GetSetting("fStaminaAttackWeaponMult");

	RE::Setting* fActorSwimBreathBase = GSC->GetSetting("fActorSwimBreathBase");
	RE::Setting* fActorSwimBreathDamage = GSC->GetSetting("fActorSwimBreathDamage");

	static void Initialize()
	{
		fDiffMultHPByPCVE->data.f = 1.5f;
		fDiffMultHPByPCE->data.f = 1.25f;
		fDiffMultHPByPCN->data.f = 1.0f;
		fDiffMultHPByPCH->data.f = 0.75f;
		fDiffMultHPByPCVH->data.f = 0.5f;
		fDiffMultHPByPCL->data.f = 0.25f;

		fDiffMultHPToPCVE->data.f = 0.5f;
		fDiffMultHPToPCE->data.f = 1.0f;
		fDiffMultHPToPCN->data.f = 1.5f;
		fDiffMultHPToPCH->data.f = 2.0f;
		fDiffMultHPToPCVH->data.f = 2.5f;
		fDiffMultHPToPCL->data.f = 3.0f;

		fArmorRatingScalingFactor->data.f = 0.0f;
		fMaxArmorRating->data.f = 0.0f;

		fPlayerMaxResistance->data.f = 100.0f;

		fXPPerSkillRank->data.f = 1.0f;
		fXPLevelUpBase->data.f = 75.0f;
		fXPLevelUpMult->data.f = 25.0f;

		iTrainingNumAllowedPerLevel->data.i = 2;

		fCombatDistance->data.f = 100.0f;
		fCombatMagickaRegenRateMult->data.f = 0.0f;
		fCombatStaminaRegenRateMult->data.f = 0.0f;

		fDamagePCSkillMax->data.f = 2.0f;
		fDamagePCSkillMin->data.f = 1.0f;
		
		fDamageSkillMax->data.f = 4.0f;
		fDamageSkillMin->data.f = 1.0f;

		fSprintStaminaDrainMult->data.f = 0.0f;

		fStaminaAttackWeaponBase->data.f = 0.0f;
		fStaminaAttackWeaponMult->data.f = 0.0f;

		fActorSwimBreathBase->data.f = 15.0f;
		fActorSwimBreathDamage->data.f = 0.08f;
	}
}

constexpr RE::FormID MGEF_WARRIOR_STONE = 0x000E5F4B;
constexpr RE::FormID MGEF_MAGE_STONE = 0x000E5F48;
constexpr RE::FormID MGEF_THIEF_STONE = 0x000E5F44;

constexpr RE::FormID RACE_ARGONIAN = 79680;
constexpr RE::FormID RACE_BRETON = 79681;
constexpr RE::FormID RACE_DARKELF = 79682;
constexpr RE::FormID RACE_HIGHELF = 79683;
constexpr RE::FormID RACE_IMPERIAL = 79684;
constexpr RE::FormID RACE_KHAJIIT = 79685;
constexpr RE::FormID RACE_NORD = 79686;
constexpr RE::FormID RACE_ORC = 79687;
constexpr RE::FormID RACE_REDGUARD = 79688;
constexpr RE::FormID RACE_WOODELF = 79689;
constexpr RE::FormID RACE_ARGONIAN_VAMPIRE = 559162;
constexpr RE::FormID RACE_BRETON_VAMPIRE = 559164;
constexpr RE::FormID RACE_DARKELF_VAMPIRE = 559165;
constexpr RE::FormID RACE_HIGHELF_VAMPIRE = 559168;
constexpr RE::FormID RACE_IMPERIAL_VAMPIRE = 559172;
constexpr RE::FormID RACE_KHAJIIT_VAMPIRE = 559173;
constexpr RE::FormID RACE_NORD_VAMPIRE = 558996;
constexpr RE::FormID RACE_ORC_VAMPIRE = 688825;
constexpr RE::FormID RACE_REDGUARD_VAMPIRE = 559174;
constexpr RE::FormID RACE_WOODELF_VAMPIRE = 559236;

constexpr RE::FormID RACE_VAMPIRE_BEAST = 10298;
constexpr RE::FormID RACE_WEREWOLF_BEAST = 843140;

constexpr RE::FormID RACE_OLD = 425176;
constexpr RE::FormID RACE_NORD_CHILD = 181851;

constexpr RE::FormID RACE_CHICKEN = 692637;
constexpr RE::FormID RACE_COW = 321413;
constexpr RE::FormID RACE_DOG = 78318;
constexpr RE::FormID RACE_RABBIT = 449689;
constexpr RE::FormID RACE_HORSE = 910597;
constexpr RE::FormID RACE_DEER = 450201;
constexpr RE::FormID RACE_CRAB = 763205;
constexpr RE::FormID RACE_WOLF = 78346;
constexpr RE::FormID RACE_ICETROLL = 78342;
constexpr RE::FormID RACE_GIANT = 78329;

static RE::FormID GetLocalID(RE::FormID id) { return id & 0x00FFFFFF; }

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

	static bool TryGetBaseValues(RE::TESRace* race, RaceData& outData)
	{
		if (!race)
			return false;

		const RE::FormID id = GetLocalID(race->GetFormID());

		switch (id) {
		case RACE_ARGONIAN:
			outData = { { 115.0f, 75.0f, 110.0f }, { 4.0f, 2.0f, 4.0f } };
			return true;
		case RACE_BRETON:
			outData = { { 90.0f, 115.0f, 95.0f }, { 2.0f, 5.0f, 3.0f } };
			return true;
		case RACE_DARKELF:
			outData = { { 105.0f, 105.0f, 105.0f }, { 3.0f, 3.0f, 3.0f } };
			return true;
		case RACE_HIGHELF:
			outData = { { 85.0f, 125.0f, 90.0f }, { 2.0f, 7.0f, 2.0f } };
			return true;
		case RACE_IMPERIAL:
			outData = { { 110.0f, 80.0f, 110.0f }, { 4.0f, 2.0f, 4.0f } };
			return true;
		case RACE_KHAJIIT:
			outData = { { 100.0f, 75.0f, 125.0f }, { 3.0f, 2.0f, 6.0f } };
			return true;
		case RACE_NORD:
			outData = { { 120.0f, 70.0f, 110.0f }, { 5.0f, 1.0f, 4.0f } };
			return true;
		case RACE_ORC:
			outData = { { 125.0f, 60.0f, 115.0f }, { 6.0f, 1.0f, 4.0f } };
			return true;
		case RACE_REDGUARD:
			outData = { { 115.0f, 65.0f, 120.0f }, { 4.0f, 1.0f, 5.0f } };
			return true;
		case RACE_WOODELF:
			outData = { { 95.0f, 90.0f, 115.0f }, { 2.0f, 3.0f, 5.0f } };
			return true;

		case RACE_ARGONIAN_VAMPIRE:
			outData = { { 115.0f, 75.0f, 110.0f }, { 5.0f, 3.0f, 5.0f } };
			return true;
		case RACE_BRETON_VAMPIRE:
			outData = { { 90.0f, 115.0f, 95.0f }, { 3.0f, 6.0f, 4.0f } };
			return true;
		case RACE_DARKELF_VAMPIRE:
			outData = { { 105.0f, 105.0f, 105.0f }, { 4.0f, 4.0f, 4.0f } };
			return true;
		case RACE_HIGHELF_VAMPIRE:
			outData = { { 85.0f, 125.0f, 90.0f }, { 3.0f, 8.0f, 3.0f } };
			return true;
		case RACE_IMPERIAL_VAMPIRE:
			outData = { { 110.0f, 80.0f, 110.0f }, { 5.0f, 3.0f, 5.0f } };
			return true;
		case RACE_KHAJIIT_VAMPIRE:
			outData = { { 100.0f, 75.0f, 125.0f }, { 4.0f, 3.0f, 7.0f } };
			return true;
		case RACE_NORD_VAMPIRE:
			outData = { { 120.0f, 70.0f, 110.0f }, { 6.0f, 2.0f, 5.0f } };
			return true;
		case RACE_ORC_VAMPIRE:
			outData = { { 125.0f, 60.0f, 115.0f }, { 7.0f, 2.0f, 5.0f } };
			return true;
		case RACE_REDGUARD_VAMPIRE:
			outData = { { 115.0f, 65.0f, 120.0f }, { 5.0f, 2.0f, 6.0f } };
			return true;
		case RACE_WOODELF_VAMPIRE:
			outData = { { 95.0f, 90.0f, 115.0f }, { 3.0f, 5.0f, 6.0f } };
			return true;

		case RACE_VAMPIRE_BEAST:
			outData = { { 350.0f, 300.0f, 350.0f }, { 10.0f, 10.0f, 10.0f } };
			return true;
		case RACE_WEREWOLF_BEAST:
			outData = { { 500.0f, 0.0f, 500.0f }, { 15.0f, 0.0f, 15.0f } };
			return true;

		case RACE_OLD:
			outData = { { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } };
			return true;

		case RACE_NORD_CHILD:
			outData = { { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } };
			return true;

		case RACE_CHICKEN:
			outData = { { 100.0f, 0.0f, 200.0f }, { 1.0f, 0.0f, 2.0f } };
			return true;

		case RACE_WOLF:
			outData = { { 100.0f, 0.0f, 200.0f }, { 1.0f, 0.0f, 2.0f } };
			return true;

		case RACE_ICETROLL:
			outData = { { 100.0f, 0.0f, 200.0f }, { 1.0f, 0.0f, 2.0f } };
			return true;

		case RACE_GIANT:
			outData = { { 100.0f, 0.0f, 200.0f }, { 1.0f, 0.0f, 2.0f } };
			return true;

			//constexpr RE::FormID RACE_CHICKEN = 692637;
			//constexpr RE::FormID RACE_COW = 321413;
			//constexpr RE::FormID RACE_DOG = 78318;
			//constexpr RE::FormID RACE_RABBIT = 449689;
			//constexpr RE::FormID RACE_HORSE = 910597;
			//constexpr RE::FormID RACE_DEER = 450201;
			//constexpr RE::FormID RACE_CRAB = 763205;
			//constexpr RE::FormID RACE_WOLF = 78346;
			//constexpr RE::FormID RACE_ICETROLL = 78342;
			//constexpr RE::FormID RACE_GIANT = 78329;

		default:
			return false;
		}
	}

	static void UpdateValues(RE::Actor* actor, const std::string debugMessage = "")
	{
		RaceData data;
		if (!TryGetBaseValues(actor->race, data)) {
			SKSE::log::warn("Unknown race 0x{:08X} - name: {}, race: {}", actor->race->formID, actor->GetDisplayFullName(),
				actor->race->GetFullName());
			return;
		}

		auto warriorStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_WARRIOR_STONE);
		auto mageStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_MAGE_STONE);
		auto thiefStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_THIEF_STONE);

		if (actor->HasMagicEffect(warriorStone)) {
			data.growth.hp += 1.0f;
		} else if (actor->HasMagicEffect(mageStone)) {
			data.growth.mp += 1.0f;
		} else if (actor->HasMagicEffect(thiefStone)) {
			data.growth.st += 1.0f;
		}

		auto level = actor->GetLevel();

		float damageResist = actor->GetActorValueMax(RE::ActorValue::kDamageResist);

		float hp = data.base.hp + (level - 1) * data.growth.hp + damageResist;
		float mp = data.base.mp + (level - 1) * data.growth.mp;
		float st = data.base.st + (level - 1) * data.growth.st;

		float oldHP = actor->GetBaseActorValue(RE::ActorValue::kHealth);
		float oldMP = actor->GetBaseActorValue(RE::ActorValue::kMagicka);
		float oldST = actor->GetBaseActorValue(RE::ActorValue::kStamina);

		actor->SetBaseActorValue(RE::ActorValue::kHealth, hp);
		actor->SetBaseActorValue(RE::ActorValue::kMagicka, mp);
		actor->SetBaseActorValue(RE::ActorValue::kStamina, st);

		if (!debugMessage.empty()) {
			SKSE::log::info("{}", debugMessage);
			SKSE::log::info("Name: {}, Race: {}, {} lvl", actor->GetDisplayFullName(), actor->race->GetFullName(), level);
			SKSE::log::info("PLAYER UPDATED HEALTH: {} => {}", oldHP, hp);
			SKSE::log::info("PLAYER UPDATED MAGICKA: {} => {}", oldMP, mp);
			SKSE::log::info("PLAYER UPDATED STAMINA: {} => {}", oldST, st);
		}
	}

	static void UpdateValues(const std::string debugMessage = "")
	{
		UpdateValues(RE::PlayerCharacter::GetSingleton(), debugMessage);
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

			RE::PlayerCharacter::GetSingleton()->ModActorValue(RE::ACTOR_VALUE_MODIFIER::kPermanent,
				RE::ActorValue::kCriticalChance, -bonus);

			SKSE::log::info("CritBonus: removed {} stacks (-{} CritChance)", g_currentStacks, bonus);

			g_currentStacks = 0;
			g_critTimer = 0.0f;
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

		static constexpr std::array<std::string_view, 4> kTransientHideTokens = { "photo", "screenshot", "capture",
			"freecamera" };

		// ---------------------------------------------------------
		// PRISMA UI
		// ---------------------------------------------------------

		static void Invoke(const std::string& a_script)
		{
			std::scoped_lock lock(g_viewMutex);

			if (!g_prismaUI || !g_viewReady || !g_view) {
				return;
			}

			g_prismaUI->Invoke(g_view, a_script.c_str());
		}

		static void SetCombatState(RE::ACTOR_COMBAT_STATE a_state)
		{
			const char* state = "idle";

			switch (a_state) {
			case RE::ACTOR_COMBAT_STATE::kCombat:
				state = "combat";
				break;

			case RE::ACTOR_COMBAT_STATE::kSearching:
				state = "searching";
				break;

			case RE::ACTOR_COMBAT_STATE::kNone:
			default:
				state = "idle";
				break;
			}

			Invoke("window.setCombatState('" + std::string(state) + "')");
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

		//static bool IsBlockingUIState(RE::UI* ui)
		//{
		//	if (!ui) {
		//		return false;
		//	}

		//	const bool genericBlockers =
		//		!ui->IsShowingMenus() || ui->GameIsPaused() || ui->IsModalMenuOpen() || ui->IsApplicationMenuOpen();

		//	const auto* camera = RE::PlayerCamera::GetSingleton();

		//	const bool freeCamera = camera && camera->IsInFreeCameraMode();

		//	return genericBlockers || freeCamera || IsAnyHiddenMenuOpen(ui) ||
		//	       g_transientHideMenuCount.load(std::memory_order_acquire) > 0;
		//}

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

		// ---------------------------------------------------------
		// COMBAT EVENT
		// ---------------------------------------------------------

		//class CombatEventSink : public RE::BSTEventSink<RE::TESCombatEvent>
		//{
		//public:
		//	RE::BSEventNotifyControl ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>*) override
		//	{
		//		if (!a_event) {
		//			return RE::BSEventNotifyControl::kContinue;
		//		}

		//		auto* player = RE::PlayerCharacter::GetSingleton();

		//		if (!player || !a_event->actor) {
		//			return RE::BSEventNotifyControl::kContinue;
		//		}

		//		auto* actor = a_event->actor.get();
		//		auto* target = a_event->targetActor.get();

		//		if (actor == player) {
		//			return RE::BSEventNotifyControl::kContinue;
		//		}

		//		const auto actorID = actor->GetFormID();
		//		const auto newState = a_event->newState.get();

		//		bool changed = false;
		//		RE::ACTOR_COMBAT_STATE playerState = RE::ACTOR_COMBAT_STATE::kNone;

		//		{
		//			std::scoped_lock lock(g_combatMutex);

		//			// Удаляем прежнее состояние этого участника.
		//			g_combatants.erase(actorID);

		//			// Учитываем только участников, целью которых является игрок.
		//			if (target == player && newState != RE::ACTOR_COMBAT_STATE::kNone) {
		//				g_combatants[actorID] = newState;
		//			}

		//			// Определяем общее состояние.
		//			bool hasCombat = false;
		//			bool hasSearching = false;

		//			for (const auto& [id, state] : g_combatants) {
		//				if (state == RE::ACTOR_COMBAT_STATE::kCombat) {
		//					hasCombat = true;
		//					break;
		//				}

		//				if (state == RE::ACTOR_COMBAT_STATE::kSearching) {
		//					hasSearching = true;
		//				}
		//			}

		//			if (hasCombat) {
		//				playerState = RE::ACTOR_COMBAT_STATE::kCombat;
		//			} else if (hasSearching) {
		//				playerState = RE::ACTOR_COMBAT_STATE::kSearching;
		//			} else {
		//				playerState = RE::ACTOR_COMBAT_STATE::kNone;
		//			}

		//			if (playerState != g_playerCombatState) {
		//				g_playerCombatState = playerState;
		//				changed = true;
		//			}
		//		}

		//		if (changed) {
		//			SetCombatState(playerState);
		//		}

		//		return RE::BSEventNotifyControl::kContinue;
		//	}
		//};

		//inline CombatEventSink g_combatEventSink;

		// ---------------------------------------------------------
		// PLAYER LEVEL / EXPERIENCE
		// ---------------------------------------------------------

		static void Update(float deltaTime)
		{
			g_visibilityTimer += deltaTime;

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

			if (level == g_lastLevel && currentXP == g_lastXP && requiredXP == g_lastRequiredXP) {
				return;
			}

			g_lastLevel = level;
			g_lastXP = currentXP;
			g_lastRequiredXP = requiredXP;

			const auto script = "window.setLevelProgress(" + std::to_string(level) + "," + std::to_string(currentXP) + "," +
			                    std::to_string(requiredXP) + ")";

			Invoke(script);
		}

		// ---------------------------------------------------------
		// PRISMA UI INITIALIZATION
		// ---------------------------------------------------------

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

		// ---------------------------------------------------------
		// INITIALIZATION
		// ---------------------------------------------------------

		//void Initialize()
		//{
		//	auto* messaging = SKSE::GetMessagingInterface();

		//	if (!messaging) {
		//		SKSE::log::error("PrismaUICode: Messaging unavailable");
		//		return;
		//	}

		//	messaging->RegisterListener("SKSE", [](SKSE::MessagingInterface::Message* a_message) {
		//		if (!a_message) {
		//			return;
		//		}

		//		if (a_message->type == SKSE::MessagingInterface::kPostLoad) {
		//			InitializePrismaUI();
		//		}

		//		if (a_message->type == SKSE::MessagingInterface::kDataLoaded) {
		//			if (auto* source = RE::ScriptEventSourceHolder::GetSingleton()) {
		//				source->AddEventSink(&g_combatEventSink);

		//				SKSE::log::info("PrismaUICode: TESCombatEvent registered");
		//			}
		//		}
		//	});
		//}
	}

	static RE::BSEventNotifyControl OnMenuOpenClose(const RE::MenuOpenCloseEvent* event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		if (!event) {
			return RE::BSEventNotifyControl::kContinue;
		}

		const std::string_view name = event->menuName;

		using namespace PrismaUICode;

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

			HideHUD();

			return RE::BSEventNotifyControl::kContinue;
		}

		if (event->opening && (IsKnownHiddenMenu(name) || IsTransientHideMenu(name))) {
			HideHUD();
		} else {
			UpdateHUDVisibility();
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnCellFullyLoaded(const RE::TESCellFullyLoadedEvent* e, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>*)
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

	static RE::BSEventNotifyControl OnFastTravelEnd(const RE::TESFastTravelEndEvent*, RE::BSTEventSource<RE::TESFastTravelEndEvent>*)
	{
		UpdateValues("FAST TRAVEL END");

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnSwitchRaceComplete(const RE::TESSwitchRaceCompleteEvent*, RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>*)
	{
		UpdateValues("SWITCH RACE COMPLETE");

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnActorAction(const SKSE::ActionEvent* e, RE::BSTEventSource<SKSE::ActionEvent>*)
	{
		if (!e->actor->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		if (e->type.any(SKSE::ActionEvent::Type::kBeginDraw)) {
			UpdateValues("ACTOR ACTION");
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnMagicEffectApply(const RE::TESMagicEffectApplyEvent* e, RE::BSTEventSource<RE::TESMagicEffectApplyEvent>*)
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

	static RE::BSEventNotifyControl OnContainerChanged(const RE::TESContainerChangedEvent* e, RE::BSTEventSource<RE::TESContainerChangedEvent>*)
	{
		if (!e || !e->baseObj)
			return RE::BSEventNotifyControl::kContinue;

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnEquip(const RE::TESEquipEvent* e, RE::BSTEventSource<RE::TESEquipEvent>*)
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

	static RE::BSEventNotifyControl OnHit(const RE::TESHitEvent* e, RE::BSTEventSource<RE::TESHitEvent>*)
	{
		if (!e || !e->cause || !e->cause->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* player = RE::PlayerCharacter::GetSingleton();
		if (!player) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* warriorStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_WARRIOR_STONE);

		if (!warriorStone || !player->HasMagicEffect(warriorStone)) {
			return RE::BSEventNotifyControl::kContinue;
		}

		++g_currentStacks;
		g_critTimer = kDuration;

		player->ModActorValue(RE::ACTOR_VALUE_MODIFIER::kPermanent, RE::ActorValue::kCriticalChance, kCritChancePerStack);

		SKSE::log::info("CritBonus: stack {} (+{} CritChance)", g_currentStacks, kCritChancePerStack);

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnCombat(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>*)
	{
		if (!a_event) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* player = RE::PlayerCharacter::GetSingleton();

		if (!player || !a_event->actor) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* actor = a_event->actor.get()->As<RE::Actor>();

		if (!actor || actor == player) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* eventTarget = a_event->targetActor ? a_event->targetActor.get()->As<RE::Actor>() : nullptr;

		auto currentTargetHandle = actor->currentCombatTarget.get();
		auto* currentTarget = currentTargetHandle.get();

		const auto actorID = actor->GetFormID();
		const auto newState = a_event->newState.get();

		const bool targetsPlayer = eventTarget == player || currentTarget == player;

		SKSE::log::info(
			"TESCombatEvent: actor=0x{:08X}, eventTarget=0x{:08X}, "
			"currentTarget=0x{:08X}, state={}",
			actorID, eventTarget ? eventTarget->GetFormID() : 0, currentTarget ? currentTarget->GetFormID() : 0,
			static_cast<std::uint32_t>(newState));

		bool changed = false;

		using namespace PrismaUICode;

		RE::ACTOR_COMBAT_STATE playerState = RE::ACTOR_COMBAT_STATE::kNone;

		{
			std::scoped_lock lock(g_combatMutex);

			auto it = g_combatants.find(actorID);

			if (newState == RE::ACTOR_COMBAT_STATE::kNone) {
				g_combatants.erase(actorID);

			} else if (targetsPlayer) {
				g_combatants[actorID] = newState;

			} else if (newState == RE::ACTOR_COMBAT_STATE::kSearching && !eventTarget && !currentTarget) {
				if (it != g_combatants.end()) {
					it->second = RE::ACTOR_COMBAT_STATE::kSearching;
				}

			} else {
				g_combatants.erase(actorID);
			}

			bool hasCombat = false;
			bool hasSearching = false;

			for (const auto& [id, state] : g_combatants) {
				if (state == RE::ACTOR_COMBAT_STATE::kCombat) {
					hasCombat = true;
					break;
				}

				if (state == RE::ACTOR_COMBAT_STATE::kSearching) {
					hasSearching = true;
				}
			}

			if (hasCombat) {
				playerState = RE::ACTOR_COMBAT_STATE::kCombat;
			} else if (hasSearching) {
				playerState = RE::ACTOR_COMBAT_STATE::kSearching;
			} else {
				playerState = RE::ACTOR_COMBAT_STATE::kNone;
			}

			if (playerState != g_playerCombatState) {
				g_playerCombatState = playerState;
				changed = true;
			}
		}

		if (changed) {
			SetCombatState(playerState);
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnDeath(const RE::TESDeathEvent* e, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		if (!e || !e->actorDying) {
			return RE::BSEventNotifyControl::kContinue;
		}

		const auto player = RE::PlayerCharacter::GetSingleton();

		if (e->actorKiller && e->actorKiller.get()->IsPlayerRef() && e->actorDying) {
			const RE::Actor* dyingActor = e->actorDying.get()->As<RE::Actor>();
			const RE::Actor* killerActor = e->actorKiller.get()->As<RE::Actor>();

			if (dyingActor != killerActor) {
				float playerlvl = player->GetLevel();
				float actorlvl = dyingActor->GetLevel();
				float exp = actorlvl / playerlvl * actorlvl + 1.0f + actorlvl;
				player->skills->data->xp += exp;

				std::string message = fmt::format("{} {} lvl, +{}xp", dyingActor->GetName(), dyingActor->GetLevel(), (int)exp);

				RE::DebugNotification(message.c_str());
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	static void Update(float deltaTime)
	{
		PrismaUICode::Update(deltaTime);

		if (g_currentStacks > 0) {
			auto* warriorStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_WARRIOR_STONE);

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

	static void Initialize()
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
			if (a->GetBaseActorValue(RE::ActorValue::kSpeedMult) < 80.0f) {
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
			auto* setting = GameSettings::iTrainingNumAllowedPerLevel;
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

	class UpdateAV
	{
		static void actor_regen(RE::Actor* a, float dt)
		{	
			const float fixVal = 0.01f;

			float hp = a->GetActorValueMax(RE::ActorValue::kHealth);
			float mp = a->GetActorValueMax(RE::ActorValue::kMagicka);
			float st = a->GetActorValueMax(RE::ActorValue::kStamina);

			float hpRate = a->GetActorValueMax(RE::ActorValue::kHealRateMult) * fixVal;
			float mpRate = a->GetActorValueMax(RE::ActorValue::kMagickaRateMult) * fixVal;
			float stRate = a->GetActorValueMax(RE::ActorValue::kStaminaRateMult) * fixVal;

			float hpRegen = (hp * 0.01f) * hpRate;
			float mpRegen = (mp * 0.025f) * mpRate;
			float stRegen = (st * 0.04f) * stRate;

			a->RestoreActorValue(RE::ActorValue::kHealth, hpRegen * dt);
			a->RestoreActorValue(RE::ActorValue::kMagicka, mpRegen * dt);
			a->RestoreActorValue(RE::ActorValue::kStamina, stRegen * dt);
		}

		static void thunk(RE::Actor* a, RE::ActorValue av, float dt)
		{
			actor_regen(a, dt);
			
			if (a->IsPlayerRef() && a->As<RE::PlayerCharacter>()->IsGodMode()) {
				func(a, av, dt);
				return;
			}

			if (a->actorState1.sprinting) {
				float stCost = (5 + a->equippedWeight * 0.2f);
				a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			} else if (a->actorState1.running) {
				float stCost = (1 + a->equippedWeight * 0.1f);
				a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			} else if (a->actorState1.swimming) {
				float stCost = (1 + a->equippedWeight * 0.25f);
				a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			}
			
			if (a->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kBowDrawn) {
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

	//ctd
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
			if (a->GetActorValue(RE::ActorValue::kStamina) < a->GetPermanentActorValue(RE::ActorValue::kStamina) * 0.18f) {
				// decrease attack speed
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
				a->DamageActorValue(RE::ActorValue::kStamina, 5.0f + a->equippedWeight * 0.25f);
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

namespace Rebalance
{
	int GetWeaponTier(RE::TESObjectWEAP* weap)
	{
		if (weap->HasKeywordByEditorID("WeapMaterialDaedric"))
			return 6;
		if (weap->HasKeywordByEditorID("WeapMaterialDragonbone"))
			return 5;
		if (weap->HasKeywordByEditorID("WeapMaterialEbony"))
			return 4;
		if (weap->HasKeywordByEditorID("WeapMaterialOrcish") || weap->HasKeywordByEditorID("WeapMaterialGlass"))
			return 3;
		if (weap->HasKeywordByEditorID("WeapMaterialDwarven") || weap->HasKeywordByEditorID("WeapMaterialElven"))
			return 2;
		if (weap->HasKeywordByEditorID("WeapMaterialSteel"))
			return 1;
		return 0;
	}

	int GetAmmoTier(RE::TESAmmo* ammo)
	{
		if (ammo->HasKeywordByEditorID("WeapMaterialDaedric"))
			return 6;
		if (ammo->HasKeywordByEditorID("WeapMaterialDragonbone"))
			return 5;
		if (ammo->HasKeywordByEditorID("WeapMaterialEbony"))
			return 4;
		if (ammo->HasKeywordByEditorID("WeapMaterialOrcish") || ammo->HasKeywordByEditorID("WeapMaterialGlass"))
			return 3;
		if (ammo->HasKeywordByEditorID("WeapMaterialDwarven") || ammo->HasKeywordByEditorID("WeapMaterialElven"))
			return 2;
		if (ammo->HasKeywordByEditorID("WeapMaterialSteel"))
			return 1;
		return 0;
	}

	std::uint16_t CalculateDamage(int baseDamage, int tier)
	{
		float multiplier = std::pow(1.15f, static_cast<float>(tier));
		return static_cast<std::uint16_t>(std::ceil(baseDamage * multiplier));
	}

	void weap_Install()
	{
		auto* dh = RE::TESDataHandler::GetSingleton();
		if (!dh)
			return;

		for (auto* form : dh->GetFormArray<RE::TESObjectWEAP>()) {
			if (!form)
				continue;

			int baseDmg = -1;

			// Определяем базовый урон по типу оружия
			if (form->HasKeywordByEditorID("WeapTypeDagger"))
				baseDmg = 25;
			else if (form->HasKeywordByEditorID("WeapTypeSword"))
				baseDmg = 35;
			else if (form->HasKeywordByEditorID("WeapTypeWarAxe"))
				baseDmg = 40;
			else if (form->HasKeywordByEditorID("WeapTypeMace"))
				baseDmg = 45;
			else if (form->HasKeywordByEditorID("WeapTypeGreatsword"))
				baseDmg = 55;
			else if (form->HasKeywordByEditorID("WeapTypeBattleaxe"))
				baseDmg = 65;
			else if (form->HasKeywordByEditorID("WeapTypeWarhammer"))
				baseDmg = 75;
			else if (form->HasKeywordByEditorID("WeapTypeBow"))
				baseDmg = 30;

			// Если это стандартное оружие, вычисляем урон
			if (baseDmg > 0) {
				int tier = GetWeaponTier(form);
				form->attackDamage = CalculateDamage(baseDmg, tier);
			}
		}
	}

	void ammo_Install()
	{
		auto* dh = RE::TESDataHandler::GetSingleton();
		if (!dh)
			return;

		for (auto* form : dh->GetFormArray<RE::TESAmmo>()) {
			if (!form)
				continue;

			// Болты (они не следуют обычной формуле, задаем вручную)
			if (form->HasKeywordByEditorID("WeapTypeBolt")) {
				if (form->HasKeywordByEditorID("WeapMaterialDwarven")) {
					form->data.damage = 20;
				} else {
					form->data.damage = 15;
				}
			}
			// Стрелы
			else if (form->HasKeywordByEditorID("WeapTypeArrow")) {
				int baseDmg = 10;
				int tier = GetAmmoTier(form);
				form->data.damage = CalculateDamage(baseDmg, tier);
			}
		}
	}

	void Initialize()
	{
		weap_Install();
		ammo_Install();
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
		break;

	case SKSE::MessagingInterface::kDataLoaded:
		GameSettings::Initialize();

		Event::PrismaUICode::InitializePrismaUI();

		Event::Initialize();
		VirtualHook::Initialize();
		Hook::Initialize();
		break;

	case SKSE::MessagingInterface::kPreLoadGame:
		Event::PrismaUICode::g_playerWorldReady.store(false);
		Event::PrismaUICode::HideHUD();
		break;

	case SKSE::MessagingInterface::kPostLoadGame:
		SKSE::log::info("PrismaUICode: Save loaded");

		Event::PrismaUICode::g_playerWorldReady.store(false);
		break;

	case SKSE::MessagingInterface::kNewGame:
		SKSE::log::info("PrismaUICode: New game");

		Event::PrismaUICode::g_playerWorldReady.store(false);
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
