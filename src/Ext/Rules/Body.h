#pragma once

#include <CCINIClass.h>
#include <RulesClass.h>

#include <Utilities/Container.h>
#include <Utilities/Constructs.h>
#include <Utilities/Template.h>

#include <Utilities/Debug.h>


class AnimTypeClass;
class MouseCursor;
class TechnoTypeClass;
class VocClass;
class WarheadTypeClass;
class DigitalDisplayTypeClass;

class RulesExt
{
public:
	using base_type = RulesClass;

	class ExtData final : public Extension<RulesClass>
	{
	public:
		DynamicVectorClass<DynamicVectorClass<TechnoTypeClass*>> AITargetTypesLists;
		DynamicVectorClass<DynamicVectorClass<ScriptTypeClass*>> AIScriptsLists;

		Valueable<int> Storage_TiberiumIndex;
		Valueable<int> RadApplicationDelay_Building;
		Valueable<double> JumpjetCrash;
		Valueable<bool> JumpjetNoWobbles;
		Valueable<bool> JumpjetAllowLayerDeviation;
		Valueable<bool> JumpjetTurnToTarget;
		PhobosFixedString<32u> MissingCameo;
		Valueable<int> PlacementGrid_TranslucentLevel;
		Valueable<int> BuildingPlacementPreview_TranslucentLevel;
		Valueable<Vector3D<int>> Pips_Shield;
		PhobosFixedString<32u> Pips_Shield_Background_Filename;
		SHPStruct* Pips_Shield_Background_SHP;
		Valueable<Vector3D<int>> Pips_Shield_Building;
		Nullable<int> Pips_Shield_Building_Empty;

		Valueable<bool> DigitalDisplay_Enable;
		Valueable<DigitalDisplayTypeClass*> Buildings_DefaultDigitalDisplayTypeHP;
		Valueable<DigitalDisplayTypeClass*> Buildings_DefaultDigitalDisplayTypeSP;
		Valueable<DigitalDisplayTypeClass*> Infantrys_DefaultDigitalDisplayTypeHP;
		Valueable<DigitalDisplayTypeClass*> Infantrys_DefaultDigitalDisplayTypeSP;
		Valueable<DigitalDisplayTypeClass*> Units_DefaultDigitalDisplayTypeHP;
		Valueable<DigitalDisplayTypeClass*> Units_DefaultDigitalDisplayTypeSP;
		Valueable<DigitalDisplayTypeClass*> Aircrafts_DefaultDigitalDisplayTypeHP;
		Valueable<DigitalDisplayTypeClass*> Aircrafts_DefaultDigitalDisplayTypeSP;

		ExtData(RulesClass* OwnerObject) : Extension<RulesClass>(OwnerObject)
			, Storage_TiberiumIndex { -1 }
			, RadApplicationDelay_Building { 0 }
			, JumpjetCrash { 5.0 }
			, JumpjetNoWobbles { false }
			, JumpjetAllowLayerDeviation { true }
			, JumpjetTurnToTarget { false }
			, MissingCameo { "xxicon.shp" }
			, PlacementGrid_TranslucentLevel{ 0 }
			, BuildingPlacementPreview_TranslucentLevel { 3 }
			, Pips_Shield_Background_Filename {}
			, Pips_Shield_Background_SHP {}
			, Pips_Shield_Building { { -1,-1,-1 } }
			, Pips_Shield_Building_Empty {}
			, JumpjetTurnToTarget { false }
			, DigitalDisplay_Enable { false }
			, Buildings_DefaultDigitalDisplayTypeHP { nullptr }
			, Buildings_DefaultDigitalDisplayTypeSP { nullptr }
			, Infantrys_DefaultDigitalDisplayTypeHP { nullptr }
			, Infantrys_DefaultDigitalDisplayTypeSP { nullptr }
			, Units_DefaultDigitalDisplayTypeHP { nullptr }
			, Units_DefaultDigitalDisplayTypeSP { nullptr }
			, Aircrafts_DefaultDigitalDisplayTypeHP { nullptr }
			, Aircrafts_DefaultDigitalDisplayTypeSP { nullptr }
		{ }

		virtual ~ExtData() = default;

		virtual void LoadFromINIFile(CCINIClass* pINI) override;
		virtual void LoadBeforeTypeData(RulesClass* pThis, CCINIClass* pINI);
		virtual void LoadAfterTypeData(RulesClass* pThis, CCINIClass* pINI);
		virtual void InitializeConstants() override;
		void InitializeAfterTypeData(RulesClass* pThis);

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { }

		virtual void LoadFromStream(PhobosStreamReader& Stm) override;
		virtual void SaveToStream(PhobosStreamWriter& Stm) override;

	private:
		template <typename T>
		void Serialize(T& Stm);
	};

private:
	static std::unique_ptr<ExtData> Data;

public:
	static IStream* g_pStm;

	static void Allocate(RulesClass* pThis);
	static void Remove(RulesClass* pThis);

	static void LoadFromINIFile(RulesClass* pThis, CCINIClass* pINI);
	static void LoadBeforeTypeData(RulesClass* pThis, CCINIClass* pINI);
	static void LoadAfterTypeData(RulesClass* pThis, CCINIClass* pINI);

	static ExtData* Global()
	{
		return Data.get();
	}

	static void Clear()
	{
		Allocate(RulesClass::Instance);
	}

	static void PointerGotInvalid(void* ptr, bool removed)
	{
		Global()->InvalidatePointer(ptr, removed);
	}

	static bool LoadGlobals(PhobosStreamReader& Stm);
	static bool SaveGlobals(PhobosStreamWriter& Stm);

	static bool DetailsCurrentlyEnabled();
	static bool DetailsCurrentlyEnabled(int minDetailLevel);
};
