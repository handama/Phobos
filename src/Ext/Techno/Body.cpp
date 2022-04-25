#include "Body.h"

#include <BuildingClass.h>
#include <HouseClass.h>
#include <BulletClass.h>
#include <BulletTypeClass.h>
#include <ScenarioClass.h>
#include <SpawnManagerClass.h>
#include <InfantryClass.h>
#include <TacticalClass.h>
#include <Unsorted.h>

#include <PhobosHelper/Helper.h>

#include <Ext/BulletType/Body.h>
#include <Ext/WeaponType/Body.h>
#include <New/Type/DigitalDisplayTypeClass.h>

template<> const DWORD Extension<TechnoClass>::Canary = 0x55555555;
TechnoExt::ExtContainer TechnoExt::ExtMap;

bool TechnoExt::IsActive(TechnoClass* pThis)
{
	return
		pThis &&
		!pThis->TemporalTargetingMe &&
		!pThis->BeingWarpedOut &&
		!pThis->IsUnderEMP() &&
		pThis->IsAlive &&
		pThis->Health > 0 &&
		!pThis->InLimbo;
}

void TechnoExt::ObjectKilledBy(TechnoClass* pVictim, TechnoClass* pKiller)
{
	if (auto pVictimTechno = static_cast<TechnoClass*>(pVictim))
	{
		auto pVictimTechnoData = TechnoExt::ExtMap.Find(pVictim);

		if (pVictimTechnoData && pKiller)
		{
			TechnoClass* pObjectKiller;

			if ((pKiller->GetTechnoType()->Spawned || pKiller->GetTechnoType()->MissileSpawn) && pKiller->SpawnOwner)
				pObjectKiller = pKiller->SpawnOwner;
			else
				pObjectKiller = pKiller;

			if (pObjectKiller && pObjectKiller->BelongsToATeam())
			{
				auto pKillerTechnoData = TechnoExt::ExtMap.Find(pObjectKiller);
				auto const pFootKiller = abstract_cast<FootClass*>(pObjectKiller);
				auto const pFocus = abstract_cast<TechnoClass*>(pFootKiller->Team->Focus);
				/*
				Debug::Log("DEBUG: pObjectKiller -> [%s] [%s] registered a kill of the type [%s]\n",
					pFootKiller->Team->Type->ID, pObjectKiller->get_ID(), pVictim->get_ID());
				*/
				pKillerTechnoData->LastKillWasTeamTarget = false;
				if (pFocus == pVictim)
					pKillerTechnoData->LastKillWasTeamTarget = true;
			}
		}
	}
}

void TechnoExt::ApplyMindControlRangeLimit(TechnoClass* pThis)
{
	if (auto pCapturer = pThis->MindControlledBy)
	{
		auto pCapturerExt = TechnoTypeExt::ExtMap.Find(pCapturer->GetTechnoType());
		if (pCapturerExt && pCapturerExt->MindControlRangeLimit.Get() > 0 &&
			pThis->DistanceFrom(pCapturer) > pCapturerExt->MindControlRangeLimit.Get())
		{
			pCapturer->CaptureManager->FreeUnit(pThis);
		}
	}
}

void TechnoExt::ApplyInterceptor(TechnoClass* pThis)
{
	auto pData = TechnoExt::ExtMap.Find(pThis);
	auto const pTypeData = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());

	if (pData && pTypeData && pTypeData->Interceptor && !pThis->Target &&
		!(pThis->WhatAmI() == AbstractType::Aircraft && pThis->GetHeight() <= 0))
	{
		for (auto const& pBullet : *BulletClass::Array)
		{
			if (auto pBulletTypeData = BulletTypeExt::ExtMap.Find(pBullet->Type))
			{
				if (!pBulletTypeData->Interceptable)
					continue;
			}

			const auto guardRange = pThis->Veterancy.IsElite() ?
				pTypeData->Interceptor_EliteGuardRange :
				pTypeData->Interceptor_GuardRange;
			const auto minguardRange = pThis->Veterancy.IsElite() ?
				pTypeData->Interceptor_EliteMinimumGuardRange :
				pTypeData->Interceptor_MinimumGuardRange;

			auto distance = pBullet->Location.DistanceFrom(pThis->Location);
			if (distance > guardRange.Get() || distance < minguardRange.Get())
				continue;

			/*
			if (pBullet->Location.DistanceFrom(pBullet->TargetCoords) >
				double(ScenarioClass::Instance->Random.RandomRanged(128, (int)guardRange / 10)) * 10)
			{
				continue;
			}
			*/

			if (!pThis->Owner->IsAlliedWith(pBullet->Owner))
			{
				pThis->SetTarget(pBullet);
				pData->InterceptedBullet = pBullet;
				break;
			}
		}
	}
}

void TechnoExt::ApplyPowered_KillSpawns(TechnoClass* pThis)
{
	auto const pTypeData = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());
	if (pTypeData && pThis->WhatAmI() == AbstractType::Building)
	{
		auto const pBuilding = abstract_cast<BuildingClass*>(pThis);
		if (pTypeData->Powered_KillSpawns && pBuilding->Type->Powered && !pBuilding->IsPowerOnline())
		{
			if (auto pManager = pBuilding->SpawnManager)
			{
				pManager->ResetTarget();
				for (auto pItem : pManager->SpawnedNodes)
				{
					if (pItem->Status == SpawnNodeStatus::Attacking || pItem->Status == SpawnNodeStatus::Returning)
					{
						pItem->Unit->ReceiveDamage(&pItem->Unit->Health, 0,
							RulesClass::Instance()->C4Warhead, nullptr, false, false, nullptr);
					}
				}
			}
		}
	}
}

void TechnoExt::ApplySpawn_LimitRange(TechnoClass* pThis)
{
	auto const pTypeData = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());
	if (pTypeData && pTypeData->Spawn_LimitedRange)
	{
		if (auto const pManager = pThis->SpawnManager)
		{
			auto pTechnoType = pThis->GetTechnoType();
			int weaponRange = 0;
			int weaponRangeExtra = pTypeData->Spawn_LimitedExtraRange * 256;

			auto setWeaponRange = [&weaponRange](WeaponTypeClass* pWeaponType)
			{
				if (pWeaponType && pWeaponType->Spawner && pWeaponType->Range > weaponRange)
					weaponRange = pWeaponType->Range;
			};

			setWeaponRange(pTechnoType->Weapon[0].WeaponType);
			setWeaponRange(pTechnoType->Weapon[1].WeaponType);
			setWeaponRange(pTechnoType->EliteWeapon[0].WeaponType);
			setWeaponRange(pTechnoType->EliteWeapon[1].WeaponType);

			weaponRange += weaponRangeExtra;

			if (pManager->Target && (pThis->DistanceFrom(pManager->Target) > weaponRange))
				pManager->ResetTarget();
		}
	}
}

bool TechnoExt::IsHarvesting(TechnoClass* pThis)
{
	if (!TechnoExt::IsActive(pThis))
		return false;

	auto slave = pThis->SlaveManager;
	if (slave && slave->State != SlaveManagerStatus::Ready)
		return true;

	if (pThis->WhatAmI() == AbstractType::Building && pThis->IsPowerOnline())
		return true;

	auto mission = pThis->GetCurrentMission();
	if ((mission == Mission::Harvest || mission == Mission::Unload || mission == Mission::Enter)
		&& TechnoExt::HasAvailableDock(pThis))
	{
		return true;
	}

	return false;
}

bool TechnoExt::HasAvailableDock(TechnoClass* pThis)
{
	for (auto pBld : pThis->GetTechnoType()->Dock)
	{
		if (pThis->Owner->CountOwnedAndPresent(pBld))
			return true;
	}

	return false;
}

void TechnoExt::InitializeLaserTrails(TechnoClass* pThis)
{
	auto pExt = TechnoExt::ExtMap.Find(pThis);

	if (pExt->LaserTrails.size())
		return;

	if (auto pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType()))
	{
		for (auto const& entry : pTypeExt->LaserTrailData)
		{
			if (auto const pLaserType = LaserTrailTypeClass::Array[entry.idxType].get())
			{
				pExt->LaserTrails.push_back(std::make_unique<LaserTrailClass>(
					pLaserType, pThis->Owner, entry.FLH, entry.IsOnTurret));
			}
		}
	}
}

void TechnoExt::InitializeShield(TechnoClass* pThis)
{
	auto pExt = TechnoExt::ExtMap.Find(pThis);

	if (auto pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType()))
		pExt->CurrentShieldType = pTypeExt->ShieldType;
}

void TechnoExt::FireWeaponAtSelf(TechnoClass* pThis, WeaponTypeClass* pWeaponType)
{
	WeaponTypeExt::DetonateAt(pWeaponType, pThis, pThis);
}

// reversed from 6F3D60
CoordStruct TechnoExt::GetFLHAbsoluteCoords(TechnoClass* pThis, CoordStruct pCoord, bool isOnTurret)
{
	auto const pType = pThis->GetTechnoType();
	auto const pFoot = abstract_cast<FootClass*>(pThis);
	Matrix3D mtx;

	// Step 1: get body transform matrix
	if (pFoot && pFoot->Locomotor)
		mtx = pFoot->Locomotor->Draw_Matrix(nullptr);
	else // no locomotor means no rotation or transform of any kind (f.ex. buildings) - Kerbiter
		mtx.MakeIdentity();

	// Steps 2-3: turret offset and rotation
	if (isOnTurret && pThis->HasTurret())
	{
		TechnoTypeExt::ApplyTurretOffset(pType, &mtx);

		double turretRad = (pThis->TurretFacing().value32() - 8) * -(Math::Pi / 16);
		double bodyRad = (pThis->PrimaryFacing.current().value32() - 8) * -(Math::Pi / 16);
		float angle = (float)(turretRad - bodyRad);

		mtx.RotateZ(angle);
	}

	// Step 4: apply FLH offset
	mtx.Translate((float)pCoord.X, (float)pCoord.Y, (float)pCoord.Z);

	Vector3D<float> result = Matrix3D::MatrixMultiply(mtx, Vector3D<float>::Empty);

	// Resulting coords are mirrored along X axis, so we mirror it back
	result.Y *= -1;

	// Step 5: apply as an offset to global object coords
	CoordStruct location = pThis->GetCoords();
	location += { (int)result.X, (int)result.Y, (int)result.Z };

	return location;
}

CoordStruct TechnoExt::GetBurstFLH(TechnoClass* pThis, int weaponIndex, bool& FLHFound)
{
	FLHFound = false;
	CoordStruct FLH = CoordStruct::Empty;

	if (!pThis || weaponIndex < 0)
		return FLH;

	auto const pExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());

	if (pThis->Veterancy.IsElite())
	{
		if (pExt->EliteWeaponBurstFLHs[weaponIndex].Count > pThis->CurrentBurstIndex)
		{
			FLHFound = true;
			FLH = pExt->EliteWeaponBurstFLHs[weaponIndex][pThis->CurrentBurstIndex];
		}
	}
	else
	{
		if (pExt->WeaponBurstFLHs[weaponIndex].Count > pThis->CurrentBurstIndex)
		{
			FLHFound = true;
			FLH = pExt->WeaponBurstFLHs[weaponIndex][pThis->CurrentBurstIndex];
		}
	}

	return FLH;
}

void TechnoExt::EatPassengers(TechnoClass* pThis)
{
	if (!TechnoExt::IsActive(pThis))
		return;

	auto const pData = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());

	if (pData && pData->PassengerDeletion_Rate > 0)
	{
		auto pExt = TechnoExt::ExtMap.Find(pThis);

		if (pThis->Passengers.NumPassengers > 0)
		{
			FootClass* pPassenger = pThis->Passengers.GetFirstPassenger();

			if (pExt->PassengerDeletionCountDown < 0)
			{
				// Setting & start countdown. Bigger units needs more time
				int passengerSize = pData->PassengerDeletion_Rate;
				if (pData->PassengerDeletion_Rate_SizeMultiply && pPassenger->GetTechnoType()->Size > 1.0)
					passengerSize *= (int)(pPassenger->GetTechnoType()->Size + 0.5);

				pExt->PassengerDeletionCountDown = passengerSize;
				pExt->PassengerDeletionTimer.Start(passengerSize);
			}
			else
			{
				if (pExt->PassengerDeletionTimer.Completed())
				{
					ObjectClass* pLastPassenger = nullptr;

					// Passengers are designed as a FIFO queue but being implemented as a list
					while (pPassenger->NextObject)
					{
						pLastPassenger = pPassenger;
						pPassenger = static_cast<FootClass*>(pPassenger->NextObject);
					}

					if (pLastPassenger)
						pLastPassenger->NextObject = nullptr;
					else
						pThis->Passengers.FirstPassenger = nullptr;

					--pThis->Passengers.NumPassengers;

					if (pPassenger)
					{
						if (auto const pPassengerType = pPassenger->GetTechnoType())
						{
							VocClass::PlayAt(pData->PassengerDeletion_ReportSound, pThis->GetCoords(), nullptr);

							if (pData->PassengerDeletion_Anim.isset())
							{
								const auto pAnimType = pData->PassengerDeletion_Anim.Get();
								if (auto const pAnim = GameCreate<AnimClass>(pAnimType, pThis->Location))
								{
									pAnim->SetOwnerObject(pThis);
									pAnim->Owner = pThis->Owner;
								}
							}

							// Check if there is money refund
							if (pData->PassengerDeletion_Soylent)
							{
								int nMoneyToGive = 0;

								// Refund money to the Attacker
								if (pPassengerType && pPassengerType->Soylent > 0)
									nMoneyToGive = pPassengerType->Soylent;

								// Is allowed the refund of friendly units?
								if (!pData->PassengerDeletion_SoylentFriendlies && pPassenger->Owner->IsAlliedWith(pThis))
									nMoneyToGive = 0;

								if (nMoneyToGive > 0)
									pThis->Owner->GiveMoney(nMoneyToGive);
							}
						}

						pPassenger->UnInit();
					}

					pExt->PassengerDeletionTimer.Stop();
					pExt->PassengerDeletionCountDown = -1;
				}
			}
		}
		else
		{
			pExt->PassengerDeletionTimer.Stop();
			pExt->PassengerDeletionCountDown = -1;
		}
	}
}

bool TechnoExt::CanFireNoAmmoWeapon(TechnoClass* pThis, int weaponIndex)
{
	if (pThis->GetTechnoType()->Ammo > 0)
	{
		if (const auto pExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType()))
		{
			if (pThis->Ammo <= pExt->NoAmmoAmount && (pExt->NoAmmoWeapon = weaponIndex || pExt->NoAmmoWeapon == -1))
				return true;
		}
	}

	return false;
}

// Feature: Kill Object Automatically
void TechnoExt::CheckDeathConditions(TechnoClass* pThis)
{
	auto pTypeThis = pThis->GetTechnoType();
	auto pTypeData = TechnoTypeExt::ExtMap.Find(pTypeThis);
	auto pData = TechnoExt::ExtMap.Find(pThis);

	const bool peacefulDeath = pTypeData->Death_Peaceful.Get();
	// Death if no ammo
	if (pTypeThis && pTypeData && pTypeData->Death_NoAmmo)
	{
		if (pTypeThis->Ammo > 0 && pThis->Ammo <= 0)
		{
			if (peacefulDeath)
			{
				pThis->Limbo();
				pThis->UnInit();
			}
			else
			{
				pThis->ReceiveDamage(&pThis->Health, 0, RulesClass::Instance()->C4Warhead, nullptr, true, false, pThis->Owner);
			}
		}
	}

	// Death if countdown ends
	if (pTypeThis && pData && pTypeData && pTypeData->Death_Countdown > 0)
	{
		if (pData->Death_Countdown >= 0)
		{
			if (pData->Death_Countdown > 0)
			{
				pData->Death_Countdown--; // Update countdown
			}
			else
			{
				// Countdown ended. Kill the unit
				pData->Death_Countdown = -1;
				if (peacefulDeath)
				{
					pThis->Limbo();
					pThis->UnInit();
				}
				else
				{
					pThis->ReceiveDamage(&pThis->Health, 0, RulesClass::Instance()->C4Warhead, nullptr, true, false, pThis->Owner);
				}
			}
		}
		else
		{
			pData->Death_Countdown = pTypeData->Death_Countdown; // Start countdown
		}
	}
}

void TechnoExt::UpdateSharedAmmo(TechnoClass* pThis)
{
	if (!pThis)
		return;

	if (const auto pType = pThis->GetTechnoType())
	{
		if (pType->OpenTopped && pThis->Passengers.NumPassengers > 0)
		{
			if (const auto pExt = TechnoTypeExt::ExtMap.Find(pType))
			{
				if (pExt->Ammo_Shared && pType->Ammo > 0)
				{
					auto passenger = pThis->Passengers.FirstPassenger;
					TechnoTypeClass* passengerType;

					do
					{
						passengerType = passenger->GetTechnoType();
						auto pPassengerExt = TechnoTypeExt::ExtMap.Find(passengerType);

						if (pPassengerExt && pPassengerExt->Ammo_Shared)
						{
							if (pExt->Ammo_Shared_Group < 0 || pExt->Ammo_Shared_Group == pPassengerExt->Ammo_Shared_Group)
							{
								if (pThis->Ammo > 0 && (passenger->Ammo < passengerType->Ammo))
								{
									pThis->Ammo--;
									passenger->Ammo++;
								}
							}
						}

						passenger = static_cast<FootClass*>(passenger->NextObject);
					}
					while (passenger);
				}
			}
		}
	}
}

double TechnoExt::GetCurrentSpeedMultiplier(FootClass* pThis)
{
	double houseMultiplier = 1.0;

	if (pThis->WhatAmI() == AbstractType::Aircraft)
		houseMultiplier = pThis->Owner->Type->SpeedAircraftMult;
	else if (pThis->WhatAmI() == AbstractType::Infantry)
		houseMultiplier = pThis->Owner->Type->SpeedInfantryMult;
	else
		houseMultiplier = pThis->Owner->Type->SpeedUnitsMult;

	return pThis->SpeedMultiplier * houseMultiplier *
		(pThis->HasAbility(Ability::Faster) ? RulesClass::Instance->VeteranSpeed : 1.0);
}

void TechnoExt::UpdateMindControlAnim(TechnoClass* pThis)
{
	if (const auto pExt = TechnoExt::ExtMap.Find(pThis))
	{
		if (pThis->IsMindControlled())
		{
			if (pThis->MindControlRingAnim && !pExt->MindControlRingAnimType)
			{
				pExt->MindControlRingAnimType = pThis->MindControlRingAnim->Type;
			}
			else if (!pThis->MindControlRingAnim && pExt->MindControlRingAnimType &&
				pThis->CloakState == CloakState::Uncloaked && !pThis->InLimbo && pThis->IsAlive)
			{
				auto coords = CoordStruct::Empty;
				coords = *pThis->GetCoords(&coords);
				int offset = 0;

				if (const auto pBuilding = specific_cast<BuildingClass*>(pThis))
					offset = Unsorted::LevelHeight * pBuilding->Type->Height;
				else
					offset = pThis->GetTechnoType()->MindControlRingOffset;

				coords.Z += offset;
				auto anim = GameCreate<AnimClass>(pExt->MindControlRingAnimType, coords, 0, 1);

				if (anim)
				{
					pThis->MindControlRingAnim = anim;
					pThis->MindControlRingAnim->SetOwnerObject(pThis);

					if (pThis->WhatAmI() == AbstractType::Building)
						pThis->MindControlRingAnim->ZAdjust = -1024;
				}
			}
		}
		else if (pExt->MindControlRingAnimType)
		{
			pExt->MindControlRingAnimType = nullptr;
		}
	}
}

void TechnoExt::DigitalDisplayHealth(TechnoClass* pThis, Point2D* pLocation)
{//pos use for reference ShieldClass::DrawShieldBar_Building

	TechnoTypeClass* pType = pThis->GetTechnoType();
	auto pTypeExt = TechnoTypeExt::ExtMap.Find(pType);
	auto pExt = TechnoExt::ExtMap.Find(pThis);
	int iLength = pThis->WhatAmI() == AbstractType::Infantry ? 8 : 17;
	Point2D Loc = *pLocation;

	if (pThis->WhatAmI() == AbstractType::Building)
	{
		Loc.X -= 5;
		Loc.Y -= 3;
	}
	
	Loc.Y -= 5;

	DigitalDisplayTypeClass* pDisplayType = nullptr;
	AbstractType ThisAbstractType = pThis->WhatAmI();

	switch (ThisAbstractType)
	{
	case AbstractType::Building:
		pDisplayType = pTypeExt->DigitalDisplayType.Get(RulesExt::Global()->Buildings_DefaultDigitalDisplayTypeHP.Get());
		break;
	case AbstractType::Infantry:
		pDisplayType = pTypeExt->DigitalDisplayType.Get(RulesExt::Global()->Infantrys_DefaultDigitalDisplayTypeHP.Get());
		break;
	case AbstractType::Unit:
		pDisplayType = pTypeExt->DigitalDisplayType.Get(RulesExt::Global()->Units_DefaultDigitalDisplayTypeHP.Get());
		break;
	case AbstractType::Aircraft:
		pDisplayType = pTypeExt->DigitalDisplayType.Get(RulesExt::Global()->Aircrafts_DefaultDigitalDisplayTypeHP.Get());
		break;
	default:
		break;
	}

	if (pDisplayType == nullptr)
		return;

	Point2D PosH = { 0, 0 };

	if (pThis->WhatAmI() == AbstractType::Building)
	{
		CoordStruct Coords = { 0, 0, 0 };
		pThis->GetTechnoType()->Dimension2(&Coords);

		Point2D Pos2 = { 0, 0 };
		CoordStruct Coords2 = { -Coords.X / 2, Coords.Y / 2, Coords.Z };
		TacticalClass::Instance->CoordsToScreen(&Pos2, &Coords2);

		PosH.X = Pos2.X + Loc.X + 4 * 17 - 110;
		PosH.Y = Pos2.Y + Loc.Y - 2 * 17 + 40;
	}
	else
	{
		if (iLength == 8)
		{
			PosH.X = Loc.X - 15;
			PosH.Y = Loc.Y - 50;
		}
		else
		{
			PosH.X = Loc.X - 20;
			PosH.Y = Loc.Y - 45;
		}
		PosH.Y += pType->PixelSelectionBracketDelta;
		//Building's PixelSelectionBracketDetla is ineffctive

	}

	if (pExt->Shield == nullptr ||
		pExt->Shield->IsBrokenAndNonRespawning())
	{
		PosH.X += pDisplayType->Offset_WithoutShield.Get(pDisplayType->Offset.Get()).X;
		PosH.Y += pDisplayType->Offset_WithoutShield.Get(pDisplayType->Offset.Get()).Y;
	}
	else
	{
		PosH.X += pDisplayType->Offset.Get().X;
		PosH.Y += pDisplayType->Offset.Get().Y;
	}

	bool UseSHPShow = pDisplayType->UseSHP;

	if (UseSHPShow)
	{
		if (pThis->WhatAmI() == AbstractType::Building)
		{
			PosH.X += 10;
			PosH.Y -= 27;
		}
		else
		{
			PosH.X -= 8;
			PosH.Y -= 15;
		}
		DigitalDisplaySHPHealth(pThis, pDisplayType, PosH);
	}
	else
	{
		if (pThis->WhatAmI() == AbstractType::Building)
		{
			PosH.X += 35;
			PosH.Y += 5;
		}
		else
		{
			PosH.X += 18;

			if (iLength != 8) 
				PosH.X += 4;
		}
		DigitalDisplayTextHealth(pThis, pDisplayType, PosH);
	}
}

void TechnoExt::DigitalDisplayTextHealth(TechnoClass* pThis, DigitalDisplayTypeClass* pDisplayType, Point2D Pos)
{
	wchar_t Healthpoint[0x20];
	swprintf_s(Healthpoint, L"%d/%d", pThis->Health, pThis->GetTechnoType()->Strength);

	COLORREF HPColor;

	if (pThis->IsGreenHP())
		HPColor = Drawing::RGB2DWORD(pDisplayType->Text_ColorHigh.Get());
	else if (pThis->IsYellowHP())
		HPColor = Drawing::RGB2DWORD(pDisplayType->Text_ColorMid.Get());
	else
		HPColor = Drawing::RGB2DWORD(pDisplayType->Text_ColorLow.Get());

	bool ShowBackground = pDisplayType->Text_Background;
	RectangleStruct rect = { 0,0,0,0 };
	DSurface::Temp->GetRect(&rect);
	COLORREF BackColor = 0;
	TextPrintType PrintType;

	switch (pDisplayType->Alignment)
	{
	case DigitalDisplayTypeClass::AlignType::Left:
		PrintType = TextPrintType::NoShadow;
		break;
	case DigitalDisplayTypeClass::AlignType::Right:
		PrintType = TextPrintType::Right;
		break;
	case DigitalDisplayTypeClass::AlignType::Center:
		PrintType = TextPrintType::Center;
	default:
		if (pThis->WhatAmI() == AbstractType::Building)
			PrintType = TextPrintType::Right;
		else
			PrintType = TextPrintType::Center;
		break;
	}

	//0x400 is TextPrintType::Background pr#563 YRpp
	PrintType = TextPrintType(int(PrintType) + (ShowBackground ? 0x400 : 0));

	//DSurface::Temp->DrawText(Healthpoint, vPosH.X, vPosH.Y, ShowHPColor);
	DSurface::Temp->DrawTextA(Healthpoint, &rect, &Pos, HPColor, BackColor, PrintType);
}

void TechnoExt::DigitalDisplaySHPHealth(TechnoClass* pThis, DigitalDisplayTypeClass* pDisplayType, Point2D Pos)
{
	const int Strength = pThis->GetTechnoType()->Strength;
	const int Health = pThis->Health;
	const DynamicVectorClass<char>vStrength = IntToVector(Strength);
	const DynamicVectorClass<char>vHealth = IntToVector(Health);
	const int Length = vStrength.Count + vHealth.Count + 1;
	const Vector2D<int> Interval = (pThis->WhatAmI() == AbstractType::Building ? pDisplayType->SHP_Interval_Building.Get() : pDisplayType->SHP_Interval.Get());
	SHPStruct* SHPFile = pDisplayType->SHPFile;
	ConvertClass* PALFile = pDisplayType->PALFile;

	if (SHPFile == nullptr ||
		PALFile == nullptr)
		return;
	
	bool LeftToRight = true;

	switch (pDisplayType->Alignment)
	{
	case DigitalDisplayTypeClass::AlignType::Left:
		break;
	case DigitalDisplayTypeClass::AlignType::Right:
		LeftToRight = false;
		break;
	case DigitalDisplayTypeClass::AlignType::Center:
		Pos.X -= ((vHealth.Count * Interval.X + vStrength.Count * Interval.X + Interval.X) / 2);
		break;
	default:
		if (pThis->WhatAmI() != AbstractType::Building)
			Pos.X -= ((vHealth.Count * Interval.X + vStrength.Count * Interval.X + Interval.X) / 2);
		break;
	}

	int base = 0;
	
	if (pThis->IsYellowHP())
		base = 10;
	else if (pThis->IsRedHP())
		base = 20;

	for (int i = vHealth.Count - 1; i >= 0; i--)
	{
		int num = base + vHealth.GetItem(i);

		if (LeftToRight)
			Pos.X += Interval.X;
		else
			Pos.X -= Interval.X;

		Pos.Y += Interval.Y;
		
		DSurface::Composite->DrawSHP(PALFile, SHPFile, num, &Pos, &DSurface::ViewBounds,
			BlitterFlags::None, 0, 0, ZGradient::Ground, 1000, 0, nullptr, 0, 0, 0);
	}
	
	if (LeftToRight)
		Pos.X += Interval.X;
	else
		Pos.X -= Interval.X;

	Pos.Y += Interval.Y;
	
	int frame = 30;
	
	if (base == 10)
		frame = 31;
	else if (base == 20)
		frame = 32;
	
	DSurface::Composite->DrawSHP(PALFile, SHPFile, frame, &Pos, &DSurface::ViewBounds,
			BlitterFlags::None, 0, 0, ZGradient::Ground, 1000, 0, nullptr, 0, 0, 0);

	for (int i = vStrength.Count - 1; i >= 0; i--)
	{
		int num = base + vStrength.GetItem(i);
		
		if (LeftToRight)
			Pos.X += Interval.X;
		else
			Pos.X -= Interval.X;

		Pos.Y += Interval.Y;
		
		DSurface::Composite->DrawSHP(PALFile, SHPFile, num, &Pos, &DSurface::ViewBounds,
			BlitterFlags::None, 0, 0, ZGradient::Ground, 1000, 0, nullptr, 0, 0, 0);
	}
}


// =============================
// load / save

template <typename T>
void TechnoExt::ExtData::Serialize(T& Stm)
{
	Stm
		.Process(this->InterceptedBullet)
		.Process(this->Shield)
		.Process(this->LaserTrails)
		.Process(this->ReceiveDamage)
		.Process(this->PassengerDeletionTimer)
		.Process(this->PassengerDeletionCountDown)
		.Process(this->CurrentShieldType)
		.Process(this->LastWarpDistance)
		.Process(this->Death_Countdown)
		.Process(this->MindControlRingAnimType)
		;
}

void TechnoExt::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{
	Extension<TechnoClass>::LoadFromStream(Stm);
	this->Serialize(Stm);
}

void TechnoExt::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{
	Extension<TechnoClass>::SaveToStream(Stm);
	this->Serialize(Stm);
}

bool TechnoExt::LoadGlobals(PhobosStreamReader& Stm)
{
	return Stm
		.Success();
}

bool TechnoExt::SaveGlobals(PhobosStreamWriter& Stm)
{
	return Stm
		.Success();
}

// =============================
// container

TechnoExt::ExtContainer::ExtContainer() : Container("TechnoClass") { }

TechnoExt::ExtContainer::~ExtContainer() = default;

void TechnoExt::ExtContainer::InvalidatePointer(void* ptr, bool bRemoved) { }

// =============================
// container hooks

DEFINE_HOOK(0x6F3260, TechnoClass_CTOR, 0x5)
{
	GET(TechnoClass*, pItem, ESI);

	TechnoExt::ExtMap.FindOrAllocate(pItem);

	return 0;
}

DEFINE_HOOK(0x6F4500, TechnoClass_DTOR, 0x5)
{
	GET(TechnoClass*, pItem, ECX);

	TechnoExt::ExtMap.Remove(pItem);

	return 0;
}

DEFINE_HOOK_AGAIN(0x70C250, TechnoClass_SaveLoad_Prefix, 0x8)
DEFINE_HOOK(0x70BF50, TechnoClass_SaveLoad_Prefix, 0x5)
{
	GET_STACK(TechnoClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	TechnoExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(0x70C249, TechnoClass_Load_Suffix, 0x5)
{
	TechnoExt::ExtMap.LoadStatic();

	return 0;
}

DEFINE_HOOK(0x70C264, TechnoClass_Save_Suffix, 0x5)
{
	TechnoExt::ExtMap.SaveStatic();

	return 0;
}
