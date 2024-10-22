#include <YRPP.h>
#include <Helpers/Macro.h>
#include <TagTypeClass.h>
#include "command.h"
#include "GeneralUtils.h"

bool bObjectInfo = false;

class ObjectInfoClass : public CommandClass
{
public:
	//CommandClass
	virtual const char* GetName() const override
	{
		return "Display Object Info";
	}
	virtual const wchar_t* GetUIName() const override
	{
		return GeneralUtils::LoadStringUnlessMissing("TXT_DISPLAY_OBJECT_INFO", L"Display Object Info");
	}

	virtual const wchar_t* GetUICategory() const override
	{
		return GeneralUtils::LoadStringUnlessMissing("TXT_DEVELOPMENT", L"Development");
	}

	virtual const wchar_t* GetUIDescription() const override
	{
		return GeneralUtils::LoadStringUnlessMissing("TXT_DISPLAY_OBJECT_INFO_DESC", L"Display objects' information along with them.");
	}

	virtual void Execute(DWORD dwUnk) const override
	{
		bObjectInfo = !bObjectInfo;
	}
};

wchar_t* char2wchar(const char* cchar)
{
	wchar_t* m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

DEFINE_HOOK(533066, CommandClassCallback_Register, 6)
{
	// Load it after Ares'

	MakeCommand<ObjectInfoClass>();

	return 0;
}

DEFINE_HOOK(4F4583, GScreenClass_DrawOnTop_TheDarkSideOfTheMoon, 6)
{
	if (!bObjectInfo)
		return 0;

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

	for (auto pTechno : *TechnoClass::Array)
	{
		if (pTechno->IsMouseHovering || pTechno->IsSelected)
		{
			Point2D position;
			TacticalClass::Instance->CoordsToClient(&pTechno->GetCoords(), &position);

			int offsetX = position.X - 20;
			int offsetY = position.Y + 30;
			bool opposite = false;
			bool draw = true;

			if (DSurface::Composite->GetHeight() - position.Y < 150)
			{
				opposite = true;
				offsetY = position.Y - 30;
			}
			if ((position.X < -80 || position.X > DSurface::Composite->GetWidth() + 80)
				|| (position.Y < -80 || position.Y > DSurface::Composite->GetHeight() + 80))
				draw = false;

			char displayBuffer[0x100] = { 0 };

			auto DrawText = [&opposite, &draw](const wchar_t* string, int& offsetX, int& offsetY, int color) {
				if (draw)
				{
					auto h = DSurface::Composite->GetHeight();
					auto w = DSurface::Composite->GetWidth();

					auto wanted = Drawing::GetTextDimensions(string);
					wanted.Height += 2;

					int exceedX = w - offsetX - wanted.Width;
					if (exceedX >= 0)
						exceedX = 0;

					int exceedY = h - offsetY - wanted.Height;
					if (exceedY >= 0)
						exceedY = 0;

					RectangleStruct rect = { offsetX + exceedX, offsetY - wanted.Height, wanted.Width, wanted.Height };

					DSurface::Composite->FillRect(&rect, COLOR_BLACK);
					DSurface::Composite->DrawTextA(string, rect.X, rect.Y, color);

					if (opposite)
						offsetY -= wanted.Height;
					else
						offsetY += wanted.Height;
				}
			};
			
			auto append = [&displayBuffer](const char* pFormat, ...)
			{
				char buffer[0x100];
				va_list args;
				va_start(args, pFormat);
				vsprintf(buffer, pFormat, args);
				va_end(args);
				strcat_s(displayBuffer, buffer);
			};
			auto display = [&displayBuffer, &DrawText, &offsetX, &offsetY]()
			{
				wchar_t bufferW[0x100] = { 0 };
				mbstowcs(bufferW, displayBuffer, strlen(displayBuffer));
				DrawText(bufferW, offsetX, offsetY, COLOR_WHITE);

				displayBuffer[0] = 0;
			};
			auto displayWide = [&DrawText, &offsetX, &offsetY](const wchar_t* pFormat, ...)
			{
				wchar_t buffer[0x100] = { 0 };
				va_list args;
				va_start(args, pFormat);
				vswprintf_s(buffer, 0x100, pFormat, args);
				va_end(args);

				DrawText(buffer, offsetX, offsetY, COLOR_WHITE);
			};


			auto printFoots = [&append, &display, &displayWide, &getMissionName](FootClass* pFoot)
			{
				auto pType = pFoot->GetTechnoType();
				//char2wchar(pFoot->GetTechnoType()->ID)
				append("ID = %s, ", pType->ID);
				append("Owner = %s (%s), ", pFoot->Owner->get_ID(), pFoot->Owner->PlainName);
				append("Location = (%d, %d)", (int)pFoot->GetMapCoords().X, (int)pFoot->GetMapCoords().Y);
				display();

				append("Mission = %d (%s), ", pFoot->CurrentMission, getMissionName((int)pFoot->CurrentMission));
				append("Group = %d, RecruitA = %d, RecruitB = %d", pFoot->Group, (int)pFoot->RecruitableA, (int)pFoot->RecruitableB);
				
				if (pType->Trainable)
				{
					char veterancy[10] = { 0 };
					if (pFoot->Veterancy.IsRookie())
						sprintf(veterancy, "%s", "Rookie");
					else if (pFoot->Veterancy.IsVeteran())
						sprintf(veterancy, "%s", "Veteran");
					else if (pFoot->Veterancy.IsElite())
						sprintf(veterancy, "%s", "Elite");
					else
						sprintf(veterancy, "%s", "N/A");
					append(", Veterancy = %s (%.2f)", veterancy, pFoot->Veterancy.Veterancy);
				}

				display();

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
					if (found)
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
					if (pTeam->CurrentScript->idxCurrentLine >= 0)
						append("Current Script [Line = Action, Argument]: %d = %d, %d", pTeam->CurrentScript->idxCurrentLine, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->idxCurrentLine].Action, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->idxCurrentLine].Argument);
					else if (!missingUnit)
						append("Current Script [Line = Action, Argument]: %d", pTeam->CurrentScript->idxCurrentLine);
					else
					{
						append("Missing: ", pTeam->CurrentScript->idxCurrentLine);
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
					display();
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

					append("Target = %s, Distance = %d, Location = (%d, %d)", ID, (pFoot->DistanceFrom(pFoot->Target) / LeptonsPerCell), mapCoords.X, mapCoords.Y);
					display();
				}

				auto pDestination = abstract_cast<TechnoClass*>(pFoot->Destination);

				if (pDestination)
				{
					append("Destination = %s, Distance = %d, Location = (%d, %d)", pDestination->GetTechnoType()->ID, (pDestination->DistanceFrom(pFoot) / 256), pDestination->GetMapCoords().X, pDestination->GetMapCoords().Y);
					display();
				}
				else
				{
					if (pFoot->Destination)
					{
						auto destCell = CellClass::Coord2Cell(pFoot->Destination->GetCoords());
						append("Destination = (%d, %d)", destCell.X, destCell.Y);
						display();
					}
				}


				auto pFocus = abstract_cast<TechnoClass*>(pFoot->Focus);

				if (pFocus)
				{
					append("Focus = %s, Distance = %d, Location = (%d, %d)", pFocus->GetTechnoType()->ID, (pFocus->DistanceFrom(pFoot) / 256), pFocus->GetMapCoords().X, pFocus->GetMapCoords().Y);
					display();
				}

				append("HP = (%d / %d)", pFoot->Health, pType->Strength);

				if (pType->Ammo > 0)
					append(", Ammo = (%d / %d)", pFoot->Ammo, pType->Ammo);

				if (pFoot->AttachedTag)
				{
					append(", Tag = %s, InstanceCount = %d", pFoot->AttachedTag->Type->get_ID(), pFoot->AttachedTag->InstanceCount);
				}
				append(", UID = %d", (int)pFoot->UniqueID); 
				display();

			};
			auto printBuilding = [&append, &display, &displayWide, &getMissionName](BuildingClass* pBuilding)
			{
				auto pType = pBuilding->GetTechnoType();
				append("ID = %s, ", pType->ID);
				append("Owner = %s (%s), ", pBuilding->Owner->get_ID(), pBuilding->Owner->PlainName);
				append("Location = (%d, %d)", pBuilding->GetMapCoords().X, pBuilding->GetMapCoords().Y);
				display();

				append("Mission = %d (%s), ", pBuilding->CurrentMission, getMissionName((int)pBuilding->CurrentMission));
				append("Group = %d", pBuilding->Group);

				if (pType->Trainable)
				{
					char veterancy[10] = { 0 };
					if (pBuilding->Veterancy.IsRookie())
						sprintf(veterancy, "%s", "Rookie");
					else if (pBuilding->Veterancy.IsVeteran())
						sprintf(veterancy, "%s", "Veteran");
					else if (pBuilding->Veterancy.IsElite())
						sprintf(veterancy, "%s", "Elite");
					else
						sprintf(veterancy, "%s", "N/A");
					append(", Veterancy = %s (%.2f)", veterancy, pBuilding->Veterancy.Veterancy);
				}

				display();


				if (pBuilding->IsOnMap && pBuilding->HasPower)
				{
					if (auto const pBldType = abstract_cast<BuildingTypeClass*>(pType))
					{
						int power = pBldType->PowerBonus - pBldType->PowerDrain;
						if (power > 0)
						{
							power *= ((double)pBuilding->Health / (double)pBuilding->Type->Strength);
							append("Power: %d", power);
							
							int totalPowerBonus = 0;
							int totalPowerDrain = 0;
							append(", Total Power Output: %d, Total Power Drain: %d", pBuilding->Owner->PowerOutput, pBuilding->Owner->PowerDrain);
							display();
						}
					}
				}

				if (pBuilding->Factory && pBuilding->Factory->Object)
				{
					append("Production: %s (%d%%)", pBuilding->Factory->Object->GetTechnoType()->ID, (pBuilding->Factory->GetProgress() * 100 / 54));
					display();
				}

				if (pBuilding->Type->Refinery || pBuilding->Type->ResourceGatherer)
				{
					append("Money: %d", pBuilding->Owner->Available_Money());
					display();
				}

				if (pBuilding->Occupants.Count > 0)
				{
					append("%d Occupants: %s", pBuilding->Occupants.Count, pBuilding->Occupants.GetItem(0)->Type->ID);
					for (int i = 1; i < pBuilding->Occupants.Count; i++)
					{
						append(", %s", pBuilding->Occupants.GetItem(i)->Type->ID);
					}
					display();
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
							&& !pNode.House->IsAlliedWith(pHouse)
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
						append("Enemy = %s (%s),  AngerLevel = %d", pEnemyHouse->get_ID(), pEnemyHouse->PlainName, angerLevel);
						display();
					}
				}

				if (pBuilding->Type->Upgrades)
				{
					append("Upgrades (%d / %d): ", pBuilding->UpgradeLevel, pBuilding->Type->Upgrades);
					for (int i = 0; i < 3; i++)
					{
						if (i != 0)
							append(", ");

						append("Slot %d = %s", i + 1, pBuilding->Upgrades[i] ? pBuilding->Upgrades[i]->get_ID() : "<none>");
					}
					display();
				}

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

					append("Target = %s, Distance = %d, Location = (%d, %d)", ID, (pBuilding->DistanceFrom(pBuilding->Target) / LeptonsPerCell), mapCoords.X, mapCoords.Y);
					display();
				}
				

				append("HP = (%d / %d)", pBuilding->Health, pBuilding->Type->Strength);

				if (pBuilding->Type->Ammo > 0)
					append(", Ammo = (%d / %d)", pBuilding->Ammo, pBuilding->Type->Ammo);

				if (pBuilding->AttachedTag)
				{
					append(", Tag = (%s), InstanceCount = (%d)", pBuilding->AttachedTag->Type->get_ID(), pBuilding->AttachedTag->InstanceCount);
				}
				append(", UID = %d", (int)pBuilding->UniqueID);

				display();

			};


			switch (pTechno->WhatAmI())
			{
				case AbstractType::Infantry:
				case AbstractType::Unit:
				case AbstractType::Aircraft:
					printFoots(abstract_cast<FootClass*>(pTechno));
					break;
				case AbstractType::Building:
					printBuilding(abstract_cast<BuildingClass*>(pTechno));
					break;
				default:
					append("INVALID ITEM!");
					display();
					break;
			}


		}
	}

	
	return 0;
}



