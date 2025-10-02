#pragma once
#include <TeamClass.h>

#include <Helpers/Enumerators.h>
#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>
#include <Phobos.h>
#include <set>

class TeamExt
{
public:
	using base_type = TeamClass;

	class ExtData final : public Extension<TeamClass>
	{
	public:
		int WaitNoTargetAttempts;
		double NextSuccessWeightAward;
		int IdxSelectedObjectFromAIList;
		double CloseEnough;
		int Countdown_RegroupAtLeader;
		int MoveMissionEndMode;
		int WaitNoTargetCounter;
		TimerStruct WaitNoTargetTimer;
		TimerStruct ForceJump_Countdown;
		int ForceJump_InitialCountdown;
		bool ForceJump_RepeatMode;
		FootClass* TeamLeader;
		std::vector<ScriptClass*> PreviousScriptList;
		int GenericStatus;
		DynamicVectorClass<FootClass*> AllPassengers;
		int AttackTargetRank;
		DynamicVectorClass<TechnoClass*> IndividualTargets;
		TechnoClass* CaptureTarget;
		DynamicVectorClass<FootClass*> AllyPassengers;
		bool SelectNeural;
		int CurrentLine;
		std::map<TechnoClass*, int> SeekEnemyRepeatedCount;
		std::set<TechnoClass*> ForbiddenTargets;

		ExtData(TeamClass* OwnerObject) : Extension<TeamClass>(OwnerObject)
			, WaitNoTargetAttempts { 0 }
			, NextSuccessWeightAward { 0 }
			, IdxSelectedObjectFromAIList { -1 }
			, CloseEnough { -1 }
			, Countdown_RegroupAtLeader { -1 }
			, MoveMissionEndMode { 0 }
			, WaitNoTargetCounter { 0 }
			, WaitNoTargetTimer { 0 }
			, ForceJump_Countdown { -1 }
			, ForceJump_InitialCountdown { -1 }
			, ForceJump_RepeatMode { false }
			, TeamLeader { nullptr }
			, PreviousScriptList { }
			, SeekEnemyRepeatedCount { }
			, ForbiddenTargets { }
			, GenericStatus { 0 }
			, AllPassengers { }
			, AttackTargetRank { -1 }
			, IndividualTargets { }
			, CaptureTarget { nullptr }
			, AllyPassengers { }
			, SelectNeural { true }
			, CurrentLine { 0 }
		{ }

		virtual ~ExtData() = default;

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override
		{
			AnnounceInvalidPointer(TeamLeader, ptr);
		}

		virtual void LoadFromStream(PhobosStreamReader& Stm) override;
		virtual void SaveToStream(PhobosStreamWriter& Stm) override;

	private:
		template <typename T>
		void Serialize(T& Stm);
	};

	class ExtContainer final : public Container<TeamExt>
	{
	public:
		ExtContainer();
		~ExtContainer();

		virtual bool InvalidateExtDataIgnorable(void* const ptr) const override
		{
			auto const abs = static_cast<AbstractClass*>(ptr)->WhatAmI();
			switch (abs)
			{
			case AbstractType::Infantry:
			case AbstractType::Unit:
			case AbstractType::Aircraft:
				return false;
			default:
				return true;
			}
		}
	};

	static ExtContainer ExtMap;

};
