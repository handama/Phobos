#include "ObjectInfo.h"

#include <Utilities/GeneralUtils.h>
#include <BuildingClass.h>
#include <InfantryClass.h>
#include <FootClass.h>
#include <TeamClass.h>
#include <HouseClass.h>
#include <ScriptClass.h>
#include <AITriggerTypeClass.h>
#include <Helpers/Enumerators.h>
#include <CRT.h>

#include <Ext/TechnoType/Body.h>
#include <Ext/Techno/Body.h>

const char* ObjectInfoCommandClass::GetName() const
{
	return "Dump ObjectInfo";
}

const wchar_t* ObjectInfoCommandClass::GetUIName() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_DUMP_OBJECT_INFO", L"Dump Object Info");
}

const wchar_t* ObjectInfoCommandClass::GetUICategory() const
{
	return CATEGORY_DEVELOPMENT;
}

const wchar_t* ObjectInfoCommandClass::GetUIDescription() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_DUMP_OBJECT_INFO_DESC", L"Dump ObjectInfo to log file and display it.");
}

void ObjectInfoCommandClass::Execute(WWKey eInput) const
{
	char buffer[0x800] = { 0 };

	auto append = [&buffer](const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		vsprintf_s(Phobos::readBuffer, pFormat, args);
		va_end(args);
		strcat_s(buffer, Phobos::readBuffer);
	};

	auto display = [&buffer]()
	{
		memset(Phobos::wideBuffer, 0, sizeof Phobos::wideBuffer);
		CRT::mbstowcs(Phobos::wideBuffer, buffer, strlen(buffer));
		MessageListClass::Instance->PrintMessage(Phobos::wideBuffer, 600, 5, true);
		Debug::Log("%s\n", buffer);
		buffer[0] = 0;
	};

	auto printFoots = [&append, &display](FootClass* pFoot)
	{
		append("[Phobos] Dump ObjectInfo runs.\n");
		auto pType = pFoot->GetTechnoType();
		append("ID = %s, ", pType->ID);
		append("Owner = %s (%s), ", pFoot->Owner->get_ID(), pFoot->Owner->PlainName);
		append("Location = (%d, %d), ", pFoot->GetMapCoords().X, pFoot->GetMapCoords().Y);
		append("Mission = %d (%s), ", pFoot->CurrentMission, MissionControlClass::FindName(pFoot->CurrentMission));
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
			FootClass* pCurrent = pFoot->Passengers.FirstPassenger;
			append("%d Passengers: %s", pFoot->Passengers.NumPassengers, pCurrent->GetTechnoType()->ID);
			for (pCurrent = abstract_cast<FootClass*>(pCurrent->NextObject); pCurrent; pCurrent = abstract_cast<FootClass*>(pCurrent->NextObject))
				append(", %s", pCurrent->GetTechnoType()->ID);
			append("\n");
		}

		if (pFoot->Target)
		{
			auto mapCoords = CellStruct::Empty;
			auto ID = "N/A";

			if (auto const pObject = abstract_cast<ObjectClass*>(pFoot->Target))
			{
				mapCoords = pObject->GetMapCoords();
				ID = pObject->GetType()->get_ID();
			}
			else if (auto const pCell = abstract_cast<CellClass*>(pFoot->Target))
			{
				mapCoords = pCell->MapCoords;
				ID = "Cell";
			}

			append("Target = %s, Distance = %d, Location = (%d, %d)\n", ID, (pFoot->DistanceFrom(pFoot->Target) / Unsorted::LeptonsPerCell), mapCoords.X, mapCoords.Y);
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

		append("HP = (%d / %d)", pFoot->Health, pType->Strength);

		auto pTechnoExt = TechnoExt::ExtMap.Find(pFoot);
		auto pShieldData = pTechnoExt->Shield.get();

		if (pTechnoExt->CurrentShieldType && pShieldData)
			append(", Shield HP = (%d / %d)", pShieldData->GetHP(), pTechnoExt->CurrentShieldType->Strength);

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
		auto pType = pBuilding->Type;
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

		if (pBuilding->Type->Upgrades)
		{
			append("Upgrades (%d/%d): ", pBuilding->UpgradeLevel, pBuilding->Type->Upgrades);
			for (int i = 0; i < 3; i++)
			{
				if (i != 0)
					append(", ");

				append("Slot %d = %s", i+1, pBuilding->Upgrades[i] ? pBuilding->Upgrades[i]->get_ID() : "<none>");
			}
			append("\n");
		}

		if (pBuilding->Type->Ammo > 0)
			append("Ammo = (%d / %d)\n", pBuilding->Ammo, pBuilding->Type->Ammo);

		if (pBuilding->Target)
		{
			auto mapCoords = CellStruct::Empty;
			auto ID = "N/A";

			if (auto const pObject = abstract_cast<ObjectClass*>(pBuilding->Target))
			{
				mapCoords = pObject->GetMapCoords();
				ID = pObject->GetType()->get_ID();
			}
			else if (auto const pCell = abstract_cast<CellClass*>(pBuilding->Target))
			{
				mapCoords = pCell->MapCoords;
				ID = "Cell";
			}

			append("Target = %s, Distance = %d, Location = (%d, %d)\n", ID, (pBuilding->DistanceFrom(pBuilding->Target) / Unsorted::LeptonsPerCell), mapCoords.X, mapCoords.Y);
		}

		append("HP = (%d / %d)", pBuilding->Health, pBuilding->Type->Strength);

		auto pTechnoExt = TechnoExt::ExtMap.Find(pBuilding);
		auto pShieldData = pTechnoExt->Shield.get();

		if (pTechnoExt->CurrentShieldType && pShieldData)
			append(", Shield HP = (%d / %d)", pShieldData->GetHP(), pTechnoExt->CurrentShieldType->Strength);

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
			printFoots(static_cast<FootClass*>(pObject));
			break;
		case AbstractType::Building:
			printBuilding(static_cast<BuildingClass*>(pObject));
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
