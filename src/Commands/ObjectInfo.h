#pragma once
#include "Commands.h"

#include <Utilities/GeneralUtils.h>
#include <BuildingClass.h>
#include <InfantryClass.h>
#include <FootClass.h>
#include <TeamClass.h>
#include <HouseClass.h>
#include <ScriptClass.h>
#include <AITriggerTypeClass.h>
#include <Helpers/Enumerators.h>

#include <Ext/TechnoType/Body.h>
#include <Ext/Techno/Body.h>

// #53 New debug feature for AI scripts
class ObjectInfoCommandClass : public PhobosCommandClass
{
public:

	virtual const char* GetName() const override
	{
		return "Dump ObjectInfo";
	}

	virtual const wchar_t* GetUIName() const override
	{
		return GeneralUtils::LoadStringUnlessMissing("TXT_DUMP_OBJECT_INFO", L"Dump Object Info");
	}

	virtual const wchar_t* GetUICategory() const override
	{
		return GeneralUtils::LoadStringUnlessMissing("TXT_DEVELOPMENT", L"Development");
	}

	virtual const wchar_t* GetUIDescription() const override
	{
		return GeneralUtils::LoadStringUnlessMissing("TXT_DUMP_OBJECT_INFO_DESC", L"Dump ObjectInfo to log file and display it.");
	}

	// Just place these two functions here... for now.
	// Will be moved to other places later.
	static size_t __cdecl mbstowcs(wchar_t* lpWideCharStr, const char* lpMultiByteStr, size_t a3)
	{
		JMP_STD(0x7CC2AC);
	}
	static int __cdecl vsprintf(char* a1, const char* a2, va_list a3)
	{
		JMP_STD(0x7CB7BA);
	}

	virtual void Execute(WWKey eInput) const override
	{
		if (this->CheckDebugDeactivated())
			return;

		char buffer[0x800] = { 0 };

		auto append = [&buffer](const char* pFormat, ...)
		{
			va_list args;
			va_start(args, pFormat);
			vsprintf(Phobos::readBuffer, pFormat, args);
			va_end(args);
			strcat_s(buffer, Phobos::readBuffer);
		};

		auto getMissionName = [](int mID)
		{
			switch (mID)
			{
			case -1:
				return "None";
			case 0:
				return "Sleep";
			case 1:
				return "Attack";
			case 2:
				return "Move";
			case 3:
				return "QMove";
			case 4:
				return "Retreat";
			case 5:
				return "Guard";
			case 6:
				return "Sticky";
			case 7:
				return "Enter";
			case 8:
				return "Capture";
			case 9:
				return "Eaten";
			case 10:
				return "Harvest";
			case 11:
				return "Area_Guard";
			case 12:
				return "Return";
			case 13:
				return "Stop";
			case 14:
				return "Ambush";
			case 15:
				return "Hunt";
			case 16:
				return "Unload";
			case 17:
				return "Sabotage";
			case 18:
				return "Construction";
			case 19:
				return "Selling";
			case 20:
				return "Repair";
			case 21:
				return "Rescue";
			case 22:
				return "Missile";
			case 23:
				return "Harmless";
			case 24:
				return "Open";
			case 25:
				return "Patrol";
			case 26:
				return "ParadropApproach";
			case 27:
				return "ParadropOverfly";
			case 28:
				return "Wait";
			case 29:
				return "AttackMove";
			case 30:
				return "SpyplaneApproach";
			case 31:
				return "SpyplaneOverfly";
			default:
				return "INVALID_MISSION";
			}
		};

		auto display = [&buffer]()
		{
			memset(Phobos::wideBuffer, 0, sizeof Phobos::wideBuffer);
			mbstowcs(Phobos::wideBuffer, buffer, strlen(buffer));
			MessageListClass::Instance->PrintMessage(Phobos::wideBuffer, 600, 5, true);
			Debug::Log("%s\n", buffer);
			buffer[0] = 0;
		};

		auto printFoots = [&append, &display, &getMissionName](FootClass* pFoot)
		{
			append("[Phobos] Dump ObjectInfo runs.\n");
			auto pType = pFoot->GetTechnoType();
			append("ID = %s, ", pType->ID);
			append("Owner = %s (%s), ", pFoot->Owner->get_ID(), pFoot->Owner->PlainName);
			append("Location = (%d, %d), ", pFoot->GetMapCoords().X, pFoot->GetMapCoords().Y);
			append("Mission = %d (%s), ", pFoot->CurrentMission, getMissionName((int)pFoot->CurrentMission));




			append("Group = %d\n", pFoot->Group);

			

			if (pFoot->BelongsToATeam())
			{
				auto pTeam = pFoot->Team;

				auto pTeamType = pFoot->Team->Type;
				bool found = false;
				for (int i = 0; i < AITriggerTypeClass::Array->Count && !found; i++)
				{
					auto pTriggerTeam1Type = AITriggerTypeClass::Array->GetItem(i)->Team1;
					auto pTriggerTeam2Type = AITriggerTypeClass::Array->GetItem(i)->Team2;

					if (pTeamType && ((pTriggerTeam1Type && pTriggerTeam1Type == pTeamType) || (pTriggerTeam2Type && pTriggerTeam2Type == pTeamType)))
					{
						found = true;
						auto pTriggerType = AITriggerTypeClass::Array->GetItem(i);
						append("Trigger ID = %s, weights [Current, Min, Max]: %f, %f, %f", pTriggerType->ID, pTriggerType->Weight_Current, pTriggerType->Weight_Minimum, pTriggerType->Weight_Maximum);
					}
				}
				display();

				append("Team ID = %s, Script ID = %s, Taskforce ID = %s",
					pTeam->Type->ID, pTeam->CurrentScript->Type->get_ID(), pTeam->Type->TaskForce->ID);
				display();

				bool missingUnit = false;
				for (int i = 0; i < 6; ++i)
				{
					auto pEntry = pTeam->Type->TaskForce->Entries[i];
					if (pEntry.Type && pEntry.Amount > 0)
					{
						int missing = pEntry.Amount;

						for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
						{
							auto pUnitType = pUnit->GetTechnoType();

							if (pUnitType
								&& pUnit->IsAlive
								&& pUnit->Health > 0
								&& !pUnit->InLimbo)
							{
								if (pEntry.Type->ID == pUnitType->ID)
								{
									missing--;
								}
							}
						}

						if (missing > 0)
						{
							missingUnit = true;
						}
					}
				}
				if (pTeam->CurrentScript->CurrentMission >= 0)
					append("Current Script [Line = Action, Argument]: %d = %d,%d", pTeam->CurrentScript->CurrentMission, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Action, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument);
				else if (!missingUnit)
					append("Current Script [Line = Action, Argument]: %d", pTeam->CurrentScript->CurrentMission);
				else
				{
					append("Missing: ", pTeam->CurrentScript->CurrentMission);
					for (int i = 0; i < 6; ++i)
					{
						auto pEntry = pTeam->Type->TaskForce->Entries[i];
						if (pEntry.Type && pEntry.Amount > 0)
						{
							int missing = pEntry.Amount;

							for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
							{
								auto pUnitType = pUnit->GetTechnoType();

								if (pUnitType
									&& pUnit->IsAlive
									&& pUnit->Health > 0
									&& !pUnit->InLimbo)
								{
									if (pEntry.Type->ID == pUnitType->ID)
									{
										missing--;
									}
								}
							}
							
							if (missing > 0)
							{
								append("(%d, %s) ", missing, pEntry.Type->ID);
							}
						}
					}
				}
					

				display();


			}

			if (pFoot->Passengers.NumPassengers > 0)
			{
				append("Passengers: %s", pFoot->Passengers.FirstPassenger->GetTechnoType()->ID);
				for (NextObject j(pFoot->Passengers.FirstPassenger->NextObject); j && abstract_cast<FootClass*>(*j); ++j)
				{
					auto passenger = static_cast<FootClass*>(*j);
					append(", %s", passenger->GetTechnoType()->ID);
				}
				append("\n");
			}

			auto pTarget = abstract_cast<TechnoClass*>(pFoot->Target);
			
			if (pTarget)
			{
				append("Target = %s, Distance = %d, Location = (%d, %d)\n", pTarget->GetTechnoType()->ID, (pTarget->DistanceFrom(pFoot) / 256), pTarget->GetMapCoords().X, pTarget->GetMapCoords().Y);
			}

			auto pDestination = abstract_cast<TechnoClass*>(pFoot->Destination);

			if (pDestination)
			{
				append("Destination = %s, Distance = %d, Location = (%d, %d)\n", pDestination->GetTechnoType()->ID, (pDestination->DistanceFrom(pFoot) / 256), pDestination->GetMapCoords().X, pDestination->GetMapCoords().Y);
			}
			else
			{
				if (pFoot->Destination)
				{
					auto destCell = CellClass::Coord2Cell(pFoot->Destination->GetCoords());
					append("Destination = (%d, %d)\n", destCell.X, destCell.Y);
				}
			}

			auto pFocus = abstract_cast<TechnoClass*>(pFoot->Focus);

			if (pFocus)
			{
				append("Focus = %s, Distance = %d, Location = (%d, %d)\n", pFocus->GetTechnoType()->ID, (pFocus->DistanceFrom(pFoot) / 256), pFocus->GetMapCoords().X, pFocus->GetMapCoords().Y);
			}

			append("Current HP = (%d / %d)", pFoot->Health, pType->Strength);

			auto pTechnoExt = TechnoExt::ExtMap.Find(pFoot);
			if (auto pShieldData = pTechnoExt->Shield.get())
			{
				auto pTypeShieldData = TechnoTypeExt::ExtMap.Find(pFoot->GetTechnoType());

				append(", Current Shield HP = (%d / %d)", pShieldData->GetHP(), pTypeShieldData->ShieldType->Strength);
			}

			if (pType->Ammo > 0)
				append(", Ammo = (%d / %d)", pFoot->Ammo, pType->Ammo);

			if (pFoot->AttachedTag)
			{
				append(", Tag = (%s), InstanceCount = (%d)", pFoot->AttachedTag->Type->get_ID(), pFoot->AttachedTag->InstanceCount);
			}

			append(", RecA = %d, RecB = %d, UID = %d", pFoot->RecruitableA, pFoot->RecruitableB, pFoot->UniqueID);

			append("\n");
			display();
		};

		auto printBuilding = [&append, &display](BuildingClass* pBuilding)
		{
			append("[Phobos] Dump ObjectInfo runs.\n");
			auto pType = pBuilding->GetTechnoType();
			append("ID = %s, ", pType->ID);
			append("Owner = %s (%s), ", pBuilding->Owner->get_ID(), pBuilding->Owner->PlainName);

		

			append("Location = (%d, %d)\n", pBuilding->GetMapCoords().X, pBuilding->GetMapCoords().Y);

			if (pBuilding->Factory && pBuilding->Factory->Object)
			{
				append("Production: %s (%d%%)\n", pBuilding->Factory->Object->GetTechnoType()->ID, (pBuilding->Factory->GetProgress() * 100 / 54));
			}

			if (pBuilding->Type->Refinery || pBuilding->Type->ResourceGatherer)
			{
				append("Money: %d\n", pBuilding->Owner->Available_Money());
			}

			if (pBuilding->Occupants.Count > 0)
			{
				append("Occupants: %s", pBuilding->Occupants.GetItem(0)->Type->ID);
				for (int i = 1; i < pBuilding->Occupants.Count; i++)
				{
					append(", %s", pBuilding->Occupants.GetItem(i)->Type->ID);
				}
				append("\n");
			}

			HouseClass* pEnemyHouse = nullptr;

			if (auto pHouse = pBuilding->Owner)
			{
				int angerLevel = -1;
				bool isHumanHouse = false;

				for (auto pNode : pHouse->AngerNodes)
				{
					if (!pNode.House->Type->MultiplayPassive
						&& !pNode.House->Defeated
						&& !pNode.House->IsObserver()
						&& ((pNode.AngerLevel > angerLevel
							)
							|| angerLevel < 0))
					{
						angerLevel = pNode.AngerLevel;
						pEnemyHouse = pNode.House;
					}
				}

				if (pEnemyHouse)
				{
					append("Enemy = %s (%s),  AngerLevel = %d, UID = %d\n", pEnemyHouse->get_ID(), pEnemyHouse->PlainName, angerLevel, pBuilding->UniqueID);
				}
			}

		/*	if (pBuilding->Owner->EnemyHouseIndex > 0)
			{
				auto pHouse = pBuilding->Owner->FindByIndex(pBuilding->Owner->EnemyHouseIndex);
				if (pHouse)
				{
					append("Enemy = (%s)\n", pHouse->get_ID());
				}
			}*/


			if (pBuilding->Type->Ammo > 0)
				append("Ammo = (%d / %d)\n", pBuilding->Ammo, pBuilding->Type->Ammo);

			auto pTarget = abstract_cast<TechnoClass*>(pBuilding->Target);
			if (pTarget)
			{
				append("Target = %s, Distance = %d, Location = (%d, %d)\n", pTarget->GetTechnoType()->ID, (pTarget->DistanceFrom(pBuilding) / 256), pTarget->GetMapCoords().X, pTarget->GetMapCoords().Y);
			}

			append("Current HP = (%d / %d)", pBuilding->Health, pBuilding->Type->Strength);

			auto pTechnoExt = TechnoExt::ExtMap.Find(pBuilding);
			if (auto pShieldData = pTechnoExt->Shield.get())
			{
				auto pTypeShieldData = TechnoTypeExt::ExtMap.Find(pBuilding->GetTechnoType());

				append(", Current Shield HP = (%d / %d)", pShieldData->GetHP(), pTypeShieldData->ShieldType->Strength);
			}

			if (pBuilding->AttachedTag)
			{
				append(", Tag = (%s), InstanceCount = (%d)", pBuilding->AttachedTag->Type->get_ID(), pBuilding->AttachedTag->InstanceCount);
			}

			append("\n");
			display();
		};

		bool dumped = false;
		auto dumpInfo = [&printFoots, &printBuilding, &append, &display, &dumped](ObjectClass* pObject)
		{
			switch (pObject->WhatAmI())
			{
			case AbstractType::Infantry:
			case AbstractType::Unit:
			case AbstractType::Aircraft:
				printFoots(abstract_cast<FootClass*>(pObject));
				break;
			case AbstractType::Building:
				printBuilding(abstract_cast<BuildingClass*>(pObject));
				break;
			default:
				append("INVALID ITEM!");
				display();
				break;
			}
			dumped = true;
		};

		for (auto pTechno : *TechnoClass::Array)
		{
			if (dumped) break;
			if (pTechno->IsMouseHovering)
				dumpInfo(pTechno);
		}
		if (!dumped)
			if (ObjectClass::CurrentObjects->Count > 0)
			{
				if (ObjectClass::CurrentObjects->Count != 1)
					MessageListClass::Instance->PrintMessage(L"This command will only dump one of these selected object", 600, 5, true);
				dumpInfo(ObjectClass::CurrentObjects->GetItem(ObjectClass::CurrentObjects->Count - 1));
			}
	}
};