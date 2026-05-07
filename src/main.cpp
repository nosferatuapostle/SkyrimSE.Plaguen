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

namespace FormID
{
	constexpr RE::FormID WarriorStone = 0x000E5F4B;
	constexpr RE::FormID MageStone = 0x000E5F48;
	constexpr RE::FormID ThiefStone = 0x000E5F44;

	constexpr RE::FormID Argonian = 79680;
	constexpr RE::FormID Breton = 79681;
	constexpr RE::FormID DarkElf = 79682;
	constexpr RE::FormID HighElf = 79683;
	constexpr RE::FormID Imperial = 79684;
	constexpr RE::FormID Khajiit = 79685;
	constexpr RE::FormID Nord = 79686;
	constexpr RE::FormID Orc = 79687;
	constexpr RE::FormID Redguard = 79688;
	constexpr RE::FormID WoodElf = 79689;
	constexpr RE::FormID ArgonianVampire = 559162;
	constexpr RE::FormID BretonVampire = 559164;
	constexpr RE::FormID DarkElfVampire = 559165;
	constexpr RE::FormID HighElfVampire = 559168;
	constexpr RE::FormID ImperialVampire = 559172;
	constexpr RE::FormID KhajiitVampire = 559173;
	constexpr RE::FormID NordVampire = 558996;
	constexpr RE::FormID OrcVampire = 688825;
	constexpr RE::FormID RedguardVampire = 559174;
	constexpr RE::FormID WoodElfVampire = 559236;
}

namespace SettingsData
{
	RE::GameSettingCollection* GSC = RE::GameSettingCollection::GetSingleton();
	RE::Setting* fArmorRatingScalingFactor = GSC->GetSetting("fArmorScalingFactor");
	RE::Setting* fMaxArmorRating = GSC->GetSetting("fMaxArmorRating");
	RE::Setting* fSprintStaminaDrainMult = GSC->GetSetting("fSprintStaminaDrainMult");

	void Initialize()
	{
		fMaxArmorRating->data.f = 100.0f;
		fSprintStaminaDrainMult->data.f = 0.0f;
	}
}

namespace ArmorRescaled
{
	constexpr float kHyperDenomShift = 0.2f;

	static inline float ConvertDamageResist(float vanillaResist)
	{
		float result;
		if (vanillaResist > 0.0f)
			result = vanillaResist / (vanillaResist + kHyperDenomShift);
		else
			result = vanillaResist / kHyperDenomShift;

		return result;
	}
}

namespace RaceLevelSystem
{
	struct RaceValuesData
	{
		float baseHP = 100.0f;
		float baseMP = 100.0f;
		float baseSP = 100.0f;
		float lvlHP = 1.0f;
		float lvlMP = 1.0f;
		float lvlST = 1.0f;
	};
	
	const std::unordered_map<RE::FormID, RaceValuesData> RACE_VALUES = {
		{ FormID::Argonian,        { 115.0f, 75.0f,  110.0f, 4.0f, 2.0f, 4.0f } },
		{ FormID::Breton,          { 90.0f,  115.0f, 95.0f,  2.0f, 5.0f, 3.0f } },
		{ FormID::DarkElf,         { 105.0f, 105.0f, 105.0f, 3.0f, 3.0f, 3.0f } },
		{ FormID::HighElf,         { 85.0f,  125.0f, 90.0f,  2.0f, 7.0f, 2.0f } },
		{ FormID::Imperial,        { 110.0f, 80.0f,  110.0f, 4.0f, 2.0f, 4.0f } },
		{ FormID::Khajiit,         { 100.0f, 75.0f,  125.0f, 3.0f, 2.0f, 6.0f } },
		{ FormID::Nord,            { 120.0f, 70.0f,  110.0f, 5.0f, 1.0f, 4.0f } },
		{ FormID::Orc,             { 125.0f, 60.0f,  115.0f, 6.0f, 1.0f, 4.0f } },
		{ FormID::Redguard,        { 115.0f, 65.0f,  120.0f, 4.0f, 1.0f, 5.0f } },
		{ FormID::WoodElf,         { 95.0f,  90.0f,  115.0f, 2.0f, 3.0f, 5.0f } },
		{ FormID::ArgonianVampire, { 115.0f, 75.0f,  110.0f, 5.0f, 3.0f, 5.0f } },
		{ FormID::BretonVampire,   { 90.0f,  115.0f, 95.0f,  3.0f, 6.0f, 4.0f } },
		{ FormID::DarkElfVampire,  { 105.0f, 105.0f, 105.0f, 4.0f, 4.0f, 4.0f } },
		{ FormID::HighElfVampire,  { 85.0f,  125.0f, 90.0f,  3.0f, 8.0f, 3.0f } },
		{ FormID::ImperialVampire, { 110.0f, 80.0f,  110.0f, 5.0f, 3.0f, 5.0f } },
		{ FormID::KhajiitVampire,  { 100.0f, 75.0f,  125.0f, 4.0f, 3.0f, 7.0f } },
		{ FormID::NordVampire,     { 120.0f, 70.0f,  110.0f, 6.0f, 2.0f, 5.0f } },
		{ FormID::OrcVampire,      { 125.0f, 60.0f,  115.0f, 7.0f, 2.0f, 5.0f } },
		{ FormID::RedguardVampire, { 115.0f, 65.0f,  120.0f, 5.0f, 2.0f, 6.0f } },
		{ FormID::WoodElfVampire,  { 95.0f,  90.0f,  115.0f, 3.0f, 5.0f, 6.0f } }
	};

	RaceValuesData GetValues(RE::TESRace* race)
	{
		RaceValuesData data;

		auto it = RACE_VALUES.find(race->GetFormID());
		if (it != RACE_VALUES.end()) {
			data = it->second;
		}

		return data;
	}

	static void UpdateValues(RE::Actor* a)
	{
		SKSE::GetTaskInterface()->AddTask([a]() {
			auto data = GetValues(a->race);

			RE::EffectSetting* warriorStone = RE::TESForm::LookupByID<RE::EffectSetting>(FormID::WarriorStone);
			RE::EffectSetting* mageStone = RE::TESForm::LookupByID<RE::EffectSetting>(FormID::MageStone);
			RE::EffectSetting* thiefStone = RE::TESForm::LookupByID<RE::EffectSetting>(FormID::ThiefStone);

			if (a->HasMagicEffect(warriorStone)) {
				data.lvlHP += 1.0f;
			}
			else if (a->HasMagicEffect(mageStone)) {
				data.lvlMP += 1.0f;
			}
			else if (a->HasMagicEffect(thiefStone)) {
				data.lvlST += 1.0f;
			}

			auto level = a->GetLevel();

			//base stats
			float hp = data.baseHP + (level - 1) * data.lvlHP;
			float mp = data.baseMP + (level - 1) * data.lvlMP;
			float st = data.baseSP + (level - 1) * data.lvlST;

			a->SetBaseActorValue(RE::ActorValue::kHealth, hp);
			a->SetBaseActorValue(RE::ActorValue::kMagicka, mp);
			a->SetBaseActorValue(RE::ActorValue::kStamina, st);

			//resists
			float vanillaResist = a->armorRating * SettingsData::fArmorRatingScalingFactor->data.f;
			float magicResist = ArmorRescaled::ConvertDamageResist(vanillaResist) * 50.0f;

			a->SetBaseActorValue(RE::ActorValue::kResistMagic, magicResist);
		});
	}

	void UpdateValues() { UpdateValues(RE::PlayerCharacter::GetSingleton()); }

	// todo:
	// update npc values in events (cellload, npc load ...)
	void UpdateAllActorValues()
	{
		auto process = RE::ProcessLists::GetSingleton();
		if (process) {
			for (auto& handle : process->highActorHandles) {
				if (auto actor = handle.get()) {
					if (actor && !actor->IsPlayerRef()) {
						UpdateValues(actor->GetTargetAsActor());
					}
				}
			}

			for (auto& handle : process->middleHighActorHandles) {
				if (auto actor = handle.get()) {
					if (actor && !actor->IsPlayerRef()) {
						UpdateValues(actor->GetTargetAsActor());
					}
				}
			}
		}
	}

	namespace Event
	{
		class FastTravelEnd : public RE::BSTEventSink<RE::TESFastTravelEndEvent>
		{
			using EventResult = RE::BSEventNotifyControl;

			virtual EventResult ProcessEvent(const RE::TESFastTravelEndEvent* a_event, RE::BSTEventSource<RE::TESFastTravelEndEvent>*)
			{
				if (!a_event) {
					return EventResult::kContinue;
				}

				UpdateValues();

				return EventResult::kContinue;
			}

			static FastTravelEnd* GetSingleton()
			{
				static FastTravelEnd instance;
				return &instance;
			}

		public:
			static void Register()
			{
				if (auto source = RE::ScriptEventSourceHolder::GetSingleton()) {
					source->AddEventSink(FastTravelEnd::GetSingleton());
				}
			}
		};

		class SwitchRaceComplete : public RE::BSTEventSink<RE::TESSwitchRaceCompleteEvent>
		{
			using EventResult = RE::BSEventNotifyControl;

			virtual EventResult ProcessEvent(const RE::TESSwitchRaceCompleteEvent* a_event,	RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>*)
			{
				if (!a_event || !a_event->subject) {
					return EventResult::kContinue;
				}

				UpdateValues();

				return EventResult::kContinue;
			}

			static SwitchRaceComplete* GetSingleton()
			{
				static SwitchRaceComplete instance;
				return &instance;
			}

		public:

			static void Register()
			{
				if (auto source = RE::ScriptEventSourceHolder::GetSingleton()) {
					source->AddEventSink(SwitchRaceComplete::GetSingleton());
				}
			}
		};

		class MagicEffectApply : public RE::BSTEventSink<RE::TESMagicEffectApplyEvent>
		{
			using EventResult = RE::BSEventNotifyControl;

			virtual EventResult ProcessEvent(const RE::TESMagicEffectApplyEvent* a_event, RE::BSTEventSource<RE::TESMagicEffectApplyEvent>*)
			{
				if (!a_event || !a_event->target.get()->IsPlayerRef())
				{
					return EventResult::kContinue;
				}
				UpdateValues();

				//auto* mgef = RE::TESForm::LookupByID<RE::EffectSetting>(a_event->magicEffect);
				//if (!mgef) {
				//	return EventResult::kContinue;
				//}

				//const char* name = mgef->GetFullName() ? mgef->GetFullName() : "None";
				//const char* editorID = mgef->GetFormEditorID() ? mgef->GetFormEditorID() : "None";
				//auto formID = mgef->GetFormID();

				//SKSE::log::info("MGEF -> Name: {} | EditorID: {} | FormID: 0x{:08X}", name, editorID, formID);

				return EventResult::kContinue;
			}

			static MagicEffectApply* GetSingleton()
			{
				static MagicEffectApply instance;
				return &instance;
			}

		public:
			static void Register()
			{
				if (auto source = RE::ScriptEventSourceHolder::GetSingleton()) {
					source->AddEventSink(MagicEffectApply::GetSingleton());
				}
			}
		};

		class Death : public RE::BSTEventSink<RE::TESDeathEvent>
		{
			using EventResult = RE::BSEventNotifyControl;

			virtual EventResult ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>*)
			{
				if (!a_event || !a_event->actorDying) {
					return EventResult::kContinue;
				}

				auto player = RE::PlayerCharacter::GetSingleton();

				float exp = 0.0f;

				if (a_event->actorKiller && a_event->actorKiller.get()->IsPlayerRef() && a_event->actorDying) {
					RE::Actor* dyingActor = a_event->actorDying.get()->As<RE::Actor>();
					RE::Actor* killerActor = a_event->actorKiller.get()->As<RE::Actor>();

					if (dyingActor != killerActor) {

						float playerlvl = player->GetLevel();
						float actorlvl = dyingActor->GetLevel();
						exp = actorlvl / playerlvl * actorlvl + 1.0f + actorlvl;
						player->skills->data->xp += exp;

						std::string message = fmt::format("{} {} lvl, +{} xp", dyingActor->GetName(), dyingActor->GetLevel(), (int)exp);

						RE::DebugNotification(message.c_str());
					}
				}

				return EventResult::kContinue;
			}

			static Death* GetSingleton()
			{
				static Death instance;
				return &instance;
			}

		public:

			static void Register()
			{
				if (auto source = RE::ScriptEventSourceHolder::GetSingleton()) {
					source->AddEventSink(Death::GetSingleton());
				}
			}
		};
	}

	namespace Hook
	{

		// todo:
		// change global variables (xp formula)
		class LevelUpMenu
		{
			static void thunk()
			{
				auto player = RE::PlayerCharacter::GetSingleton();
				player->skills->AdvanceLevel(false);
				RaceLevelSystem::UpdateValues();
			}

		public:
			static void Install() { SKSE::GetTrampoline().write_call<5>(REL::ID(51638).address() + 0xf8e, &thunk); }
		};
	}
	
	void OnPostGameLoad()
	{
		UpdateValues();
		UpdateAllActorValues();
	}

	void Initialize()
	{
		Event::FastTravelEnd::Register();
		Event::SwitchRaceComplete::Register();
		Event::MagicEffectApply::Register();
		Event::Death::Register();

		Hook::LevelUpMenu::Install();
	}
};

namespace Event
{
	class ActorAction : public RE::BSTEventSink<SKSE::ActionEvent>
	{
		virtual RE::BSEventNotifyControl ProcessEvent(const SKSE::ActionEvent* a_event,
			RE::BSTEventSource<SKSE::ActionEvent>*) override
		{
			if (!a_event || !a_event->actor) {
				return RE::BSEventNotifyControl::kContinue;
			}

			//if (a_event->type.any(SKSE::ActionEvent::Type::kSpellCast))
			//{
			//	RE::ConsoleLog::GetSingleton()->Print("proc SpellCast");
			//}

			return RE::BSEventNotifyControl::kContinue;
		}

		static ActorAction* GetSingleton()
		{
			static ActorAction instance;
			return &instance;
		}

	public:
		static void Register()
		{
			if (auto source = SKSE::GetActionEventSource()) {
				source->AddEventSink(ActorAction::GetSingleton());
			}
		}
	};

	void Initialize() { ActorAction::Register(); }
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

	void Initialize()
	{
		//ArrowGravity::Install();
		//PlayerUpdate::Install();
	}
}

namespace Hook
{
	class RestoreAV
	{
		static void ActorRegeneration(RE::Actor* a, float dt)
		{
			const float fixVal = 0.01f;

			float hp = a->GetBaseActorValue(RE::ActorValue::kHealth);
			float mp = a->GetBaseActorValue(RE::ActorValue::kMagicka);
			float st = a->GetBaseActorValue(RE::ActorValue::kStamina);

			float hpRate = a->GetBaseActorValue(RE::ActorValue::kHealRateMult) * fixVal;
			float mpRate = a->GetBaseActorValue(RE::ActorValue::kMagickaRateMult) * fixVal;
			float stRate = a->GetBaseActorValue(RE::ActorValue::kStaminaRateMult) * fixVal;

			float hpRegen = (hp * 0.01f) * hpRate;
			float mpRegen = (mp * 0.025f) * mpRate;
			float stRegen = (st * 0.04f) * stRate;

			a->RestoreActorValue(RE::ActorValue::kHealth, hpRegen * dt);
			a->RestoreActorValue(RE::ActorValue::kMagicka, mpRegen * dt);
			a->RestoreActorValue(RE::ActorValue::kStamina, stRegen * dt);
		}

		static void RestoreActorValue(RE::Actor* a, RE::ActorValue av, float dt)
		{
			ActorRegeneration(a, dt);

			if (a->actorState1.sprinting)
			{				
				a->DamageActorValue(RE::ActorValue::kStamina, (8 + a->equippedWeight * 0.2f) * dt);
			}
			else if (a->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kBowDrawn) {
				a->DamageActorValue(RE::ActorValue::kStamina, 15 * dt);
			}
			else if (a->actorState1.swimming) {
				a->DamageActorValue(RE::ActorValue::kStamina, (10 + a->equippedWeight * 0.25f) * dt);
			}

			_RestoreActorValue(a, av, dt);
		}

		static inline REL::Relocation<decltype(RestoreActorValue)> _RestoreActorValue;

	public:
		static void Install()
		{
			_RestoreActorValue = SKSE::GetTrampoline().write_call<5>(REL::ID(37510).address() + 0x1b, RestoreActorValue);
		}
	};

	class UpdateAV
	{
		static bool update_RegenDelay(RE::Actor* a, RE::ActorValue av, float) { return _update_RegenDelay(a, av, 0.0f); }

		static inline REL::Relocation<decltype(update_RegenDelay)> _update_RegenDelay;

	public:
		static void Install()
		{
			_update_RegenDelay = SKSE::GetTrampoline().write_call<5>(REL::ID(37510).address() + 0x176, update_RegenDelay);
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
			if (a->GetActorValue(RE::ActorValue::kStamina) < 32.0f) {
				return _get_damage(_weap, a, DamageMult, isbow) * 0.75f;
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
			//if (he->GetActorValue(RE::ActorValue::kStamina) < he->GetPermanentActorValue(RE::ActorValue::kStamina) * 0.2f)
			//{
			//	return 0.5f;
			//}
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
			a->DamageActorValue(RE::ActorValue::kStamina, 12.0f + a->equippedWeight * 0.25f);
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

	void Initialize()
	{
		RestoreAV::Install();
		UpdateAV::Install();
		GetDamage::Install();
		GetThisAttackChance::Install();
		PlayerJump::Install();
		AttackCostStamina::Install();
	}
}

#include <xbyak/xbyak.h>

namespace ArmorRescaled
{
	constexpr REL::Offset kInject1Offset(0x00743617);
	constexpr REL::Offset kInject2Offset(0x00624FA2);

	constexpr uint8_t kInject1Bytes[] = { 0xF3, 0x0F, 0x10, 0x4D, 0x77, 0xF3, 0x0F, 0x58, 0xC8, 0xF3, 0x0F, 0x11, 0x4D, 0x77 };

	constexpr uint8_t kInject2Bytes[] = { 0xF3, 0x0F, 0x10, 0x0D, 0x22, 0xD0, 0xF1, 0x00, 0xF3, 0x44, 0x0F, 0x58, 0xC0, 0x44, 0x0F, 0x2F, 0xC1, 0x72, 0x04, 0x44, 0x0F, 0x28, 0xC1 };

	bool TestBytes(uintptr_t a_addr, const uint8_t* a_expected, size_t a_size)
	{
		for (size_t i = 0; i < a_size; i++) {
			if (*reinterpret_cast<uint8_t*>(a_addr + i) != a_expected[i]) {
				SKSE::log::error("Byte mismatch at 0x{:X}+{}: got 0x{:02X}, expected 0x{:02X}", a_addr, i, *reinterpret_cast<uint8_t*>(a_addr + i), a_expected[i]);
				return false;
			}
		}
		return true;
	}

	void Initialize()
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

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kPostLoadGame:

		RaceLevelSystem::UpdateValues();
		RaceLevelSystem::UpdateAllActorValues();

		break;
	
	case SKSE::MessagingInterface::kDataLoaded:

		SettingsData::Initialize();

		ArmorRescaled::Initialize();

		RaceLevelSystem::Initialize();

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
