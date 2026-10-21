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

	void Initialize()
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
		if (auto* source = GetEventSource<EventType>()) {
			source->AddEventSink(&instance);
		}
	}

private:
	template <typename EventType>
	static RE::BSTEventSource<EventType>* GetEventSource()
	{
		if constexpr (std::is_same_v<EventType, SKSE::ActionEvent>) {
			return SKSE::GetActionEventSource();
		} else {
			return RE::ScriptEventSourceHolder::GetSingleton();
		}
	}
};

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

	const RE::FormID id = race->GetFormID() & 0x00FFFFFF;

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

	default:
		return false;
	}
}

static void UpdateValues(const std::string debugMessage = "")
{
	auto player = RE::PlayerCharacter::GetSingleton();

	RaceData data;
	if (!TryGetBaseValues(player->race, data)) {
		SKSE::log::warn("Unknown race 0x{:08X}, skipping stat update.", player->race->formID);
		return;
	}

	auto warriorStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_WARRIOR_STONE);
	auto mageStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_MAGE_STONE);
	auto thiefStone = RE::TESForm::LookupByID<RE::EffectSetting>(MGEF_THIEF_STONE);

	if (player->HasMagicEffect(warriorStone)) {
		data.growth.hp += 1.0f;
	} else if (player->HasMagicEffect(mageStone)) {
		data.growth.mp += 1.0f;
	} else if (player->HasMagicEffect(thiefStone)) {
		data.growth.st += 1.0f;
	}

	auto level = player->GetLevel();

	float damageResist = player->GetActorValueMax(RE::ActorValue::kDamageResist);

	float hp = data.base.hp + (level - 1) * data.growth.hp + damageResist;
	float mp = data.base.mp + (level - 1) * data.growth.mp;
	float st = data.base.st + (level - 1) * data.growth.st;

	float oldHP = player->GetBaseActorValue(RE::ActorValue::kHealth);
	float oldMP = player->GetBaseActorValue(RE::ActorValue::kMagicka);
	float oldST = player->GetBaseActorValue(RE::ActorValue::kStamina);

	player->ModBaseActorValue(RE::ActorValue::kHealth, (level - 1) * data.growth.hp);

	player->SetBaseActorValue(RE::ActorValue::kHealth, hp);
	player->SetBaseActorValue(RE::ActorValue::kMagicka, mp);
	player->SetBaseActorValue(RE::ActorValue::kStamina, st);

	if (!debugMessage.empty())
	{
		/*auto message = fmt::format(fmt::runtime(debugMessage));
		SKSE::log::info(message);*/
		SKSE::log::info("{}", debugMessage);
		SKSE::log::info("PLAYER UPDATED HEALTH: {} => {}", oldHP, hp);
		SKSE::log::info("PLAYER UPDATED MAGICKA: {} => {}", oldMP, mp);
		SKSE::log::info("PLAYER UPDATED STAMINA: {} => {}", oldST, st);
	}
}

namespace Event
{
	static RE::BSEventNotifyControl OnCellAttachDetach(const RE::TESCellAttachDetachEvent* e,
		RE::BSTEventSource<RE::TESCellAttachDetachEvent>*)
	{
		if (!e || !e->attached) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* reference = e->reference.get();

		if (!reference) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto* actor = reference->As<RE::Actor>();

		if (!actor || actor->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		//auto& referenceToProcess = e->reference;

		//if (auto* taskInterface = SKSE::GetTaskInterface()) {
		//	taskInterface->AddTask([referenceToProcess]() {
		//		auto* ref = referenceToProcess.get();

		//		if (!ref) {
		//			return;
		//		}

		//		if (auto* attachedActor = ref->As<RE::Actor>()) {
		//			auto maxHealth = attachedActor->GetBaseActorValue(RE::ActorValue::kHealth);
		//			attachedActor->SetBaseActorValue(RE::ActorValue::kHealth, maxHealth * 5);
		//		}
		//	});
		//}

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnActorAction(const SKSE::ActionEvent* e, RE::BSTEventSource<SKSE::ActionEvent>*)
	{
		if (!e->actor->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		if (e->type.any(SKSE::ActionEvent::Type::kBeginDraw, SKSE::ActionEvent::Type::kEndDraw)) {
			UpdateValues("ACTOR ACTION");
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnFastTravelEnd(const RE::TESFastTravelEndEvent*, RE::BSTEventSource<RE::TESFastTravelEndEvent>*)
	{
		UpdateValues("FAST TRAVEL END");

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnSwitchRaceComplete(const RE::TESSwitchRaceCompleteEvent*,
		RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>*)
	{
		UpdateValues("SWITCH RACE COMPLETE");

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnMagicEffectApply(const RE::TESMagicEffectApplyEvent* e,
		RE::BSTEventSource<RE::TESMagicEffectApplyEvent>*)
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

	static RE::BSEventNotifyControl OnEquip(const RE::TESEquipEvent* e, RE::BSTEventSource<RE::TESEquipEvent>*)
	{
		if (!e || !e->actor || !e->actor->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		SKSE::GetTaskInterface()->AddTask([]() {
			UpdateValues("EQUIP");
		});

		return RE::BSEventNotifyControl::kContinue;
	}

	static RE::BSEventNotifyControl OnDeath(const RE::TESDeathEvent* e, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		if (!e || !e->actorDying) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto player = RE::PlayerCharacter::GetSingleton();

		if (e->actorKiller && e->actorKiller.get()->IsPlayerRef() && e->actorDying) {
			RE::Actor* dyingActor = e->actorDying.get()->As<RE::Actor>();
			RE::Actor* killerActor = e->actorKiller.get()->As<RE::Actor>();

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

	static void Initialize()
	{
		EventSink::Register(OnCellAttachDetach);
		EventSink::Register(OnActorAction);
		EventSink::Register(OnFastTravelEnd);
		EventSink::Register(OnSwitchRaceComplete);
		EventSink::Register(OnMagicEffectApply);
		EventSink::Register(OnEquip);
		EventSink::Register(OnDeath);
	}
}

namespace VirtualHook
{
	class ArrowGravity
	{
		static float GetGravity(RE::Projectile* projectile)
		{
			if (auto shooter = projectile->shooter.get(); shooter && shooter->As<RE::Actor>())
			{
				if (shooter->As<RE::Actor>()->HasPerk(RE::TESForm::LookupByID<RE::BGSPerk>(0x00079354)))
				{
					return 0;
				}
			}

			return _GetGravity(projectile);
		}

		static inline REL::Relocation<decltype(GetGravity)> _GetGravity;

	public:

		static void Install()
		{
			_GetGravity = REL::Relocation<uintptr_t>(RE::VTABLE_ArrowProjectile[0]).write_vfunc(0xb5, GetGravity);
		}
	};

	class PlayerUpdate
	{
		static void Update(RE::PlayerCharacter* a, float dt)
		{
			_Update(a, dt);
			if (a->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kBowDrawn) {
				a->DamageActorValue(RE::ActorValue::kStamina, 12 * dt);
			}
		}

		static inline REL::Relocation<decltype(Update)> _Update;

	public:
		static void Install() { _Update = REL::Relocation<uintptr_t>(RE::VTABLE_PlayerCharacter[0]).write_vfunc(0xAD, Update); }
	};

	static void Initialize()
	{
		//ArmorHealth::Install();

		//ArrowGravity::Install();
		//PlayerUpdate::Install();
	}
}

namespace Hook
{
	class LevelUpMenu
	{
		static void thunk()
		{
			auto player = RE::PlayerCharacter::GetSingleton();
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
			//auto* settings = RE::GameSettingCollection::GetSingleton();
			//auto* setting = settings ? settings->GetSetting("iTrainingNumAllowedPerLevel") : nullptr;
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
		static void ActorRegeneration(RE::Actor* a, float dt)
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

		static void update_RegenDelay(RE::Actor* a, RE::ActorValue av, float dt)
		{
			ActorRegeneration(a, dt);
			
			if (a->IsPlayerRef() && a->As<RE::PlayerCharacter>()->IsGodMode()) {
				_update_RegenDelay(a, av, dt);
				return;
			}

			float stCost = 0;

			if (a->actorState1.sprinting) {
				stCost = (4 + a->equippedWeight * 0.2f);
				a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			} else if (a->actorState1.running) {
				stCost = (2 + a->equippedWeight * 0.1f);
				a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			} else if (a->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kBowDrawn) {
				stCost = 20;

				auto overdrawPerk = RE::TESForm::LookupByID<RE::BGSPerk>(0x000BABED);
				if (overdrawPerk && a->HasPerk(overdrawPerk)) {
					stCost = 10;
				}

				a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			} else if (a->actorState1.swimming) {
				stCost = (1 + a->equippedWeight * 0.25f);
				a->DamageActorValue(RE::ActorValue::kStamina, stCost * dt);
			}

			_update_RegenDelay(a, av, dt);
		}

		static inline REL::Relocation<decltype(update_RegenDelay)> _update_RegenDelay;

	public:
		static void Install()
		{
			_update_RegenDelay = SKSE::GetTrampoline().write_call<5>(REL::ID(37510).address() + 0x1b, update_RegenDelay);
		}
	};

	class RestoreAV
	{
		static bool RestoreActorValue(RE::Actor* a, RE::ActorValue av, float) { return _RestoreActorValue(a, av, 0.0f); }

		static inline REL::Relocation<decltype(RestoreActorValue)> _RestoreActorValue;

	public:
		static void Install()
		{
			_RestoreActorValue = SKSE::GetTrampoline().write_call<5>(REL::ID(37510).address() + 0x176, RestoreActorValue);
		}
	};

	class GetBlockCost
	{
		static float get_block_cost(RE::HitData* hitdata) { return _get_block_cost(hitdata); }

		static inline REL::Relocation<decltype(get_block_cost)> _get_block_cost;

	private:
		static void Install()
		{
			_get_block_cost = SKSE::GetTrampoline().write_call<5>(REL::ID(37633).address() + 0x8d4, get_block_cost);
		}
	};

	class GetDamage
	{
		static float get_damage(void* _weap, RE::ActorValueOwner* a, float DamageMult, char isbow)
		{
			if (a->GetActorValue(RE::ActorValue::kStamina) < 18.0f) {
				return _get_damage(_weap, a, DamageMult, isbow) * 0.4f;
			}
			return _get_damage(_weap, a, DamageMult, isbow);
		}

		static inline REL::Relocation<decltype(get_damage)> _get_damage;

	public:
		static void Install()
		{
			_get_damage = SKSE::GetTrampoline().write_call<5>(REL::ID(42832).address() + 0x1a5, get_damage);
		}
	};

	class GetThisAttackChance
	{
		static float get_thisattack_chance(RE::Actor* me, RE::Actor* he, RE::BGSAttackData* my_attackData)
		{
			if (he->GetActorValue(RE::ActorValue::kStamina) <= 0.0f) {
				return 1.0f;
			}

			return _get_thisattack_chance(me, he, my_attackData);
		}

		static inline REL::Relocation<decltype(get_thisattack_chance)> _get_thisattack_chance;

	public:
		static void Install()
		{
			_get_thisattack_chance = SKSE::GetTrampoline().write_call<5>(REL::ID(48139).address() + 0x2ae, get_thisattack_chance);
		}
	};

	class PlayerJump
	{
		static void Jump(RE::Actor* a)
		{
			_Jump(a);
			a->DamageActorValue(RE::ActorValue::kStamina, 5.0f + a->equippedWeight * 0.25f);
		}

		static inline REL::Relocation<decltype(Jump)> _Jump;

	public:
		static void Install()
		{
			_Jump = SKSE::GetTrampoline().write_branch<5>(REL::ID(41349).address() + 0x114,	Jump);
		}
	};

	class AttackCostStamina
	{
		static float CalcAttackStaminaDrain(RE::ActorValueOwner* avo, RE::BGSAttackData* attack, float)
		{
			auto a = skyrim_cast<RE::Actor*>(avo);

			float equipCost = a->equippedWeight * 0.4f;
			float regularCost = 4.0f + equipCost * 0.4f;

			if (attack->data.flags.any(RE::AttackData::AttackFlag::kBashAttack, RE::AttackData::AttackFlag::kPowerAttack)) {
				return 18.0f + equipCost;
			}
			else if (auto currentSt = a->GetActorValue(RE::ActorValue::kStamina); currentSt < regularCost) {
				return currentSt;
			}

			return regularCost;
		}

		static float CalcAttackStaminaDrain1(RE::ActorValueOwner* a, RE::BGSAttackData* attack)
		{
			return CalcAttackStaminaDrain(a, attack, _CalcAttackStaminaDrain1(a, attack));
		}

		static float CalcAttackStaminaDrain2(RE::ActorValueOwner* a, RE::BGSAttackData* attack)
		{
			return CalcAttackStaminaDrain(a, attack, _CalcAttackStaminaDrain2(a, attack));
		}

		static float CalcAttackStaminaDrain3(RE::ActorValueOwner* a, RE::BGSAttackData* attack)
		{
			return CalcAttackStaminaDrain(a, attack, _CalcAttackStaminaDrain3(a, attack));
		}

		static float CalcAttackStaminaDrain4(RE::ActorValueOwner* a, RE::BGSAttackData* attack)
		{
			return CalcAttackStaminaDrain(a, attack, _CalcAttackStaminaDrain4(a, attack));
		}

		static inline REL::Relocation<decltype(CalcAttackStaminaDrain1)> _CalcAttackStaminaDrain1;
		static inline REL::Relocation<decltype(CalcAttackStaminaDrain2)> _CalcAttackStaminaDrain2;
		static inline REL::Relocation<decltype(CalcAttackStaminaDrain3)> _CalcAttackStaminaDrain3;
		static inline REL::Relocation<decltype(CalcAttackStaminaDrain4)> _CalcAttackStaminaDrain4;

	public:

		static void Install()
		{
			_CalcAttackStaminaDrain1 =	SKSE::GetTrampoline().write_call<5>(REL::ID(48139).address() + 0x29b, CalcAttackStaminaDrain1);
			_CalcAttackStaminaDrain2 =	SKSE::GetTrampoline().write_call<5>(REL::ID(38047).address() + 0xbb, CalcAttackStaminaDrain2);
			_CalcAttackStaminaDrain3 =	SKSE::GetTrampoline().write_call<5>(REL::ID(37650).address() + 0x16e, CalcAttackStaminaDrain3);
			_CalcAttackStaminaDrain4 =	SKSE::GetTrampoline().write_call<5>(REL::ID(28629).address() + 0x1561, CalcAttackStaminaDrain4);
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
		GetDamage::Install();
		GetThisAttackChance::Install();
		PlayerJump::Install();
		AttackCostStamina::Install();
		ArmorRescaled::Install();
	}
}

namespace Rebalance
{
	void weap_Install()
	{
		RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler)
			return;

		for (auto form : dataHandler->GetFormArray(RE::FormType::Weapon)) {
			form->As<RE::TESObjectWEAP>()->attackDamage *= 4;
		}
	}

	void Initialize()
	{
		weap_Install();
	}
}

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {

	case SKSE::MessagingInterface::kPostLoad:

		SKSE::GetTaskInterface()->AddTask([] { UpdateValues("POST LOAD"); });
		
		break;
	
	case SKSE::MessagingInterface::kDataLoaded:

		Rebalance::Initialize();

		GameSettings::Initialize();

		Event::Initialize();

		VirtualHook::Initialize();

		Hook::Initialize();

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
