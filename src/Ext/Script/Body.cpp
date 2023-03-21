#include "Body.h"

#include <Ext/Techno/Body.h>
#include <Ext/Scenario/Body.h>

#define TECHNO_IS_ALIVE(tech) (!tech->InLimbo && tech->Health > 0)

template<> const DWORD Extension<ScriptClass>::Canary = 0x3B3B3B3B;
ScriptExt::ExtContainer ScriptExt::ExtMap;

// =============================
// load / save

void ScriptExt::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{
	// Nothing yet
}

void ScriptExt::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{
	// Nothing yet
}

// =============================
// container

ScriptExt::ExtContainer::ExtContainer() : Container("ScriptClass")
{
}

ScriptExt::ExtContainer::~ExtContainer() = default;

void ScriptExt::ProcessAction(TeamClass* pTeam)
{
	const int action = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Action;
	const int argument = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	switch (static_cast<PhobosScripts>(action))
	{
	case PhobosScripts::TimedAreaGuard:
		ScriptExt::ExecuteTimedAreaGuardAction(pTeam);
		break;
	case PhobosScripts::LoadIntoTransports:
		ScriptExt::LoadIntoTransports(pTeam);
		break;
	case PhobosScripts::WaitUntilFullAmmo:
		ScriptExt::WaitUntilFullAmmoAction(pTeam);
		break;
	case PhobosScripts::RepeatAttackCloserThreat:
		// Threats that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack(pTeam, true, 0, -1, -1);
		break;
	case PhobosScripts::RepeatAttackFartherThreat:
		// Threats that are far have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack(pTeam, true, 1, -1, -1);
		break;
	case PhobosScripts::RepeatAttackCloser:
		// Closer targets from Team Leader have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack(pTeam, true, 2, -1, -1);
		break;
	case PhobosScripts::RepeatAttackFarther:
		// Farther targets from Team Leader have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack(pTeam, true, 3, -1, -1);
		break;
	case PhobosScripts::SingleAttackCloserThreat:
		// Threats that are close have more priority. 1 kill only (good for xx=49,0 combos)
		ScriptExt::Mission_Attack(pTeam, false, 0, -1, -1);
		break;
	case PhobosScripts::SingleAttackFartherThreat:
		// Threats that are far have more priority. 1 kill only (good for xx=49,0 combos)
		ScriptExt::Mission_Attack(pTeam, false, 1, -1, -1);
		break;
	case PhobosScripts::SingleAttackCloser:
		// Closer targets from Team Leader have more priority. 1 kill only (good for xx=49,0 combos)
		ScriptExt::Mission_Attack(pTeam, false, 2, -1, -1);
		break;
	case PhobosScripts::SingleAttackFarther:
		// Farther targets from Team Leader have more priority. 1 kill only (good for xx=49,0 combos)
		ScriptExt::Mission_Attack(pTeam, false, 3, -1, -1);
		break;
	case PhobosScripts::DecreaseCurrentAITriggerWeight:
		ScriptExt::DecreaseCurrentTriggerWeight(pTeam, true, 0);
		break;
	case PhobosScripts::IncreaseCurrentAITriggerWeight:
		ScriptExt::IncreaseCurrentTriggerWeight(pTeam, true, 0);
		break;
	case PhobosScripts::RepeatAttackTypeCloserThreat:
		// Threats specific targets that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_List(pTeam, true, 0, -1);
		break;
	case PhobosScripts::RepeatAttackTypeFartherThreat:
		// Threats specific targets that are far have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_List(pTeam, true, 1, -1);
		break;
	case PhobosScripts::RepeatAttackTypeCloser:
		// Closer specific targets targets from Team Leader have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_List(pTeam, true, 2, -1);
		break;
	case PhobosScripts::RepeatAttackTypeFarther:
		// Farther specific targets targets from Team Leader have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_List(pTeam, true, 3, -1);
		break;
	case PhobosScripts::SingleAttackTypeCloserThreat:
		// Threats specific targets that are close have more priority. 1 kill only (good for xx=49,0 combos)
		ScriptExt::Mission_Attack_List(pTeam, false, 0, -1);
		break;
	case PhobosScripts::SingleAttackTypeFartherThreat:
		// Threats specific targets that are far have more priority. 1 kill only (good for xx=49,0 combos)
		ScriptExt::Mission_Attack_List(pTeam, false, 1, -1);
		break;
	case PhobosScripts::SingleAttackTypeCloser:
		// Closer specific targets from Team Leader have more priority. 1 kill only (good for xx=49,0 combos)
		ScriptExt::Mission_Attack_List(pTeam, false, 2, -1);
		break;
	case PhobosScripts::SingleAttackTypeFarther:
		// Farther specific targets from Team Leader have more priority. 1 kill only (good for xx=49,0 combos)
		ScriptExt::Mission_Attack_List(pTeam, false, 3, -1);
		break;
	case PhobosScripts::WaitIfNoTarget:
		ScriptExt::WaitIfNoTarget(pTeam, -1);
		break;
	case PhobosScripts::TeamWeightReward:
		ScriptExt::TeamWeightReward(pTeam, 0);
		break;
	case PhobosScripts::PickRandomScript:
		ScriptExt::PickRandomScript(pTeam, -1);
		break;
	case PhobosScripts::MoveToEnemyCloser:
		// Move to the closest enemy target
		ScriptExt::Mission_Move(pTeam, 2, false, -1, -1);
		break;
	case PhobosScripts::MoveToEnemyFarther:
		// Move to the farther enemy target
		ScriptExt::Mission_Move(pTeam, 3, false, -1, -1);
		break;
	case PhobosScripts::MoveToFriendlyCloser:
		// Move to the closest friendly target
		ScriptExt::Mission_Move(pTeam, 2, true, -1, -1);
		break;
	case PhobosScripts::MoveToFriendlyFarther:
		// Move to the farther friendly target
		ScriptExt::Mission_Move(pTeam, 3, true, -1, -1);
		break;
	case PhobosScripts::MoveToTypeEnemyCloser:
		// Move to the closest specific enemy target
		ScriptExt::Mission_Move_List(pTeam, 2, false, -1);
		break;
	case PhobosScripts::MoveToTypeEnemyFarther:
		// Move to the farther specific enemy target
		ScriptExt::Mission_Move_List(pTeam, 3, false, -1);
		break;
	case PhobosScripts::MoveToTypeFriendlyCloser:
		// Move to the closest specific friendly target
		ScriptExt::Mission_Move_List(pTeam, 2, true, -1);
		break;
	case PhobosScripts::MoveToTypeFriendlyFarther:
		// Move to the farther specific friendly target
		ScriptExt::Mission_Move_List(pTeam, 3, true, -1);
		break;
	case PhobosScripts::ModifyTargetDistance:
		// AISafeDistance equivalent for Mission_Move()
		ScriptExt::SetCloseEnoughDistance(pTeam, -1);
		break;
	case PhobosScripts::RandomAttackTypeCloser:
		// Pick 1 closer random objective from specific list for attacking it
		ScriptExt::Mission_Attack_List1Random(pTeam, true, 2, -1);
		break;
	case PhobosScripts::RandomAttackTypeFarther:
		// Pick 1 farther random objective from specific list for attacking it
		ScriptExt::Mission_Attack_List1Random(pTeam, true, 3, -1);
		break;
	case PhobosScripts::RandomMoveToTypeEnemyCloser:
		// Pick 1 closer enemy random objective from specific list for moving to it
		ScriptExt::Mission_Move_List1Random(pTeam, 2, false, -1, -1);
		break;
	case PhobosScripts::RandomMoveToTypeEnemyFarther:
		// Pick 1 farther enemy random objective from specific list for moving to it
		ScriptExt::Mission_Move_List1Random(pTeam, 3, false, -1, -1);
		break;
	case PhobosScripts::RandomMoveToTypeFriendlyCloser:
		// Pick 1 closer friendly random objective from specific list for moving to it
		ScriptExt::Mission_Move_List1Random(pTeam, 2, true, -1, -1);
		break;
	case PhobosScripts::RandomMoveToTypeFriendlyFarther:
		// Pick 1 farther friendly random objective from specific list for moving to it
		ScriptExt::Mission_Move_List1Random(pTeam, 3, true, -1, -1);
		break;
	case PhobosScripts::SetMoveMissionEndMode:
		// Set the condition for ending the Mission_Move Actions.
		ScriptExt::SetMoveMissionEndMode(pTeam, -1);
		break;
	case PhobosScripts::UnregisterGreatSuccess:
		// Un-register success for AITrigger weight adjustment (this is the opposite of 49,0)
		ScriptExt::UnregisterGreatSuccess(pTeam);
		break;
	case PhobosScripts::GatherAroundLeader:
		ScriptExt::Mission_Gather_NearTheLeader(pTeam, -1);
		break;
	case PhobosScripts::RandomSkipNextAction:
		ScriptExt::SkipNextAction(pTeam, -1);
		break;
	case PhobosScripts::ChangeTeamGroup:
		ScriptExt::TeamMemberSetGroup(pTeam, argument); 
		break;
	case PhobosScripts::DistributedLoading:
		ScriptExt::DistributedLoadOntoTransport(pTeam, argument); 
		break;
	case PhobosScripts::FollowFriendlyByGroup:
		ScriptExt::FollowFriendlyByGroup(pTeam, argument); 
		break;
	case PhobosScripts::RallyUnitWithSameGroup:
		ScriptExt::RallyUnitInMap(pTeam, argument);
		break;
	case PhobosScripts::RallyUnitInMapWithLeader:
		ScriptExt::RallyUnitInMapWithLeader(pTeam, argument);
		break;
	case PhobosScripts::StopForceJumpCountdown:
		// Stop Timed Jump
		ScriptExt::Stop_ForceJump_Countdown(pTeam);
		break;
	case PhobosScripts::NextLineForceJumpCountdown:
		// Start Timed Jump that jumps to the next line when the countdown finish (in frames)
		ScriptExt::Set_ForceJump_Countdown(pTeam, false, -1);
		break;
	case PhobosScripts::SameLineForceJumpCountdown:
		// Start Timed Jump that jumps to the same line when the countdown finish (in frames)
		ScriptExt::Set_ForceJump_Countdown(pTeam, true, -1);
		break;
	case PhobosScripts::UnloadFromTransports:
		ScriptExt::UnloadFromTransports(pTeam);
		break;
	case PhobosScripts::JumpBackToPreviousScript:
		ScriptExt::JumpBackToPreviousScript(pTeam);
		break;
	case PhobosScripts::DistributedLoading2:
		ScriptExt::DistributedLoadOntoTransport2(pTeam, argument);
		break;
	case PhobosScripts::RepeatAttackIndividuallyCloserThreat:
		// Threats that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_Individually(pTeam, 1, true, 0, -1, -1);
		break;
	case PhobosScripts::RepeatAttackTypeIndividuallyCloserThreat:
		// Threats specific targets that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_List_Individually(pTeam, 1, true, 0, -1);
		break;
	case PhobosScripts::RepeatAttackIndividuallyCloserThreat2:
		// Threats that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_Individually(pTeam, 2, true, 0, -1, -1);
		break;
	case PhobosScripts::RepeatAttackTypeIndividuallyCloserThreat2:
		// Threats specific targets that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_List_Individually(pTeam, 2, true, 0, -1);
		break;
	case PhobosScripts::SetAttackTargetRank:
		ScriptExt::SetAttackTargetRank(pTeam);
		break;
	case PhobosScripts::StopIfHumanOrAI:
		ScriptExt::StopIfHumanOrAI(pTeam);
		break;
	case PhobosScripts::RepeatAttackIndividuallyCloser:
		// Threats that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_Individually(pTeam, 1, true, 2, -1, -1);
		break;
	case PhobosScripts::RepeatAttackTypeIndividuallyCloser:
		// Threats specific targets that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_List_Individually(pTeam, 1, true, 2, -1);
		break;
	case PhobosScripts::RepeatAttackIndividuallyCloser2:
		// Threats that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_Individually(pTeam, 2, true, 2, -1, -1);
		break;
	case PhobosScripts::RepeatAttackTypeIndividuallyCloser2:
		// Threats specific targets that are close have more priority. Kill until no more targets.
		ScriptExt::Mission_Attack_List_Individually(pTeam, 2, true, 2, -1);
		break;
	case PhobosScripts::CaptureOccupiableBuildings:
		// Threats specific targets that are close have more priority. Kill until no more targets.
		ScriptExt::CaptureOccupiableBuildings(pTeam);
		break;
	case PhobosScripts::MindControlledUnitsGoToGrinder:
		// Threats specific targets that are close have more priority. Kill until no more targets.
		ScriptExt::MindControlledUnitsGoToGrinder(pTeam);
		break;
	case PhobosScripts::AllyUnitEnterTransport:
		// Threats specific targets that are close have more priority. Kill until no more targets.
		ScriptExt::AllyUnitEnterTransport(pTeam);
		break;
	case PhobosScripts::SetPickNeutral:
		ScriptExt::SetPickNeutral(pTeam);
		break;

		
	default:
		// Do nothing because or it is a wrong Action number or it is an Ares/YR action...
		if (action > 70 && !IsExtVariableAction(action))
		{
			// Unknown new action. This action finished
			pTeam->StepCompleted = true;
			ScriptExt::Log("AI Scripts - ProcessAction: [%s] [%s] (line %d): Unknown Script Action: %d\n", pTeam->Type->ID, pTeam->CurrentScript->Type->ID, pTeam->CurrentScript->CurrentMission, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Action);
		}
		break;
	}

	if (IsExtVariableAction(action))
		VariablesHandler(pTeam, static_cast<PhobosScripts>(action), argument);
}

void ScriptExt::ExecuteTimedAreaGuardAction(TeamClass* pTeam)
{
	auto const pScript = pTeam->CurrentScript;
	auto const pScriptType = pScript->Type;

	if (pTeam->GuardAreaTimer.TimeLeft == 0 && !pTeam->GuardAreaTimer.InProgress())
	{
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
			pUnit->QueueMission(Mission::Area_Guard, true);

		pTeam->GuardAreaTimer.Start(15 * pScriptType->ScriptActions[pScript->CurrentMission].Argument);
	}

	if (pTeam->GuardAreaTimer.Completed())
	{
		pTeam->GuardAreaTimer.Stop(); // Needed
		pTeam->StepCompleted = true;
	}
}

void ScriptExt::LoadIntoTransports(TeamClass* pTeam)
{
	std::vector<FootClass*> transports;

	// Collect available transports
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		auto const pType = pUnit->GetTechnoType();

		if (pType->Passengers > 0
			&& pUnit->Passengers.NumPassengers < pType->Passengers
			&& pUnit->Passengers.GetTotalSize() < pType->Passengers)
		{
			transports.emplace_back(pUnit);
		}
	}

	if (transports.size() == 0)
	{
		// This action finished
		pTeam->StepCompleted = true;
		return;
	}

	// Now load units into transports
	for (auto pTransport : transports)
	{
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			auto const pTransportType = pTransport->GetTechnoType();
			auto const pUnitType = pUnit->GetTechnoType();

			if (pTransport != pUnit
				&& pUnitType->WhatAmI() != AbstractType::AircraftType
				&& !pUnit->InLimbo && !pUnitType->ConsideredAircraft
				&& pUnit->Health > 0)
			{
				if (pUnit->GetTechnoType()->Size > 0
					&& pUnitType->Size <= pTransportType->SizeLimit
					&& pUnitType->Size <= pTransportType->Passengers - pTransport->Passengers.GetTotalSize())
				{
					// If is still flying wait a bit more
					if (pTransport->IsInAir())
						return;

					// All fine
					if (pUnit->GetCurrentMission() != Mission::Enter)
					{
						pUnit->QueueMission(Mission::Enter, false);
						pUnit->SetTarget(nullptr);
						pUnit->SetDestination(pTransport, true);

						return;
					}
				}
			}
		}
	}

	// Is loading
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		if (pUnit->GetCurrentMission() == Mission::Enter)
			return;
	}

	auto const pExt = TeamExt::ExtMap.Find(pTeam);

	if (pExt)
	{
		FootClass* pLeaderUnit = FindTheTeamLeader(pTeam);
		pExt->TeamLeader = pLeaderUnit;
	}

	// This action finished
	pTeam->StepCompleted = true;
}

void ScriptExt::WaitUntilFullAmmoAction(TeamClass* pTeam)
{
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		if (!pUnit->InLimbo && pUnit->Health > 0)
		{
			if (pUnit->GetTechnoType()->Ammo > 0 && pUnit->Ammo < pUnit->GetTechnoType()->Ammo)
			{
				// If an aircraft object have AirportBound it must be evaluated
				if (auto const pAircraft = abstract_cast<AircraftClass*>(pUnit))
				{
					if (pAircraft->Type->AirportBound)
					{
						// Reset last target, at long term battles this prevented the aircraft to pick a new target (rare vanilla YR bug)
						pUnit->SetTarget(nullptr);
						pUnit->LastTarget = nullptr;
						// Fix YR bug (when returns from the last attack the aircraft switch in loop between Mission::Enter & Mission::Guard, making it impossible to land in the dock)
						if (pUnit->IsInAir() && pUnit->CurrentMission != Mission::Enter)
							pUnit->QueueMission(Mission::Enter, true);

						return;
					}
				}
				else if (pUnit->GetTechnoType()->Reload != 0) // Don't skip units that can reload themselves
					return;
			}
		}
	}

	pTeam->StepCompleted = true;
}

void ScriptExt::Mission_Gather_NearTheLeader(TeamClass* pTeam, int countdown = -1)
{
	FootClass* pLeaderUnit = nullptr;
	int initialCountdown = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;
	bool gatherUnits = false;
	auto const pExt = TeamExt::ExtMap.Find(pTeam);

	// This team has no units! END
	if (!pTeam)
	{
		// This action finished
		pTeam->StepCompleted = true;
		return;
	}

	// Load countdown
	if (pExt->Countdown_RegroupAtLeader >= 0)
		countdown = pExt->Countdown_RegroupAtLeader;

	// Gather permanently until all the team members are near of the Leader
	if (initialCountdown == 0)
		gatherUnits = true;

	// Countdown updater
	if (initialCountdown > 0)
	{
		if (countdown > 0)
		{
			countdown--; // Update countdown
			gatherUnits = true;
		}
		else if (countdown == 0) // Countdown ended
			countdown = -1;
		else // Start countdown.
		{
			countdown = initialCountdown * 15;
			gatherUnits = true;
		}

		// Save counter
		pExt->Countdown_RegroupAtLeader = countdown;
	}

	if (!gatherUnits)
	{
		// This action finished
		pTeam->StepCompleted = true;
		return;
	}
	else
	{
		// Move all around the leader, the leader always in "Guard Area" Mission or simply in Guard Mission
		int nTogether = 0;
		int nUnits = -1; // Leader counts here
		double closeEnough;

		// Find the Leader
		pLeaderUnit = pExt->TeamLeader;

		if (!IsUnitAvailable(pLeaderUnit, true, true))
		{
			pLeaderUnit = FindTheTeamLeader(pTeam);
			pExt->TeamLeader = pLeaderUnit;
		}

		if (!pLeaderUnit)
		{
			pExt->Countdown_RegroupAtLeader = -1;
			// This action finished
			pTeam->StepCompleted = true;

			return;
		}

		// Leader's area radius where the Team members are considered "near" to the Leader
		if (pExt->CloseEnough > 0)
		{
			closeEnough = pExt->CloseEnough;
			pExt->CloseEnough = -1; // This a one-time-use value
		}
		else
		{
			closeEnough = RulesClass::Instance->CloseEnough / 256.0;
		}

		// The leader should stay calm & be the group's center
		if (pLeaderUnit->Locomotor->Is_Moving_Now())
			pLeaderUnit->SetDestination(nullptr, false);

		pLeaderUnit->QueueMission(Mission::Guard, false);

		// Check if units are around the leader
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			if (!IsUnitAvailable(pUnit, true, true))
			{
				auto pTypeUnit = pUnit->GetTechnoType();

				if (!pTypeUnit)
					continue;

				if (pUnit == pLeaderUnit)
				{
					nUnits++;
					continue;
				}

				// Aircraft case
				if (pTypeUnit->WhatAmI() == AbstractType::AircraftType && pUnit->Ammo <= 0 && pTypeUnit->Ammo > 0)
				{
					auto pAircraft = static_cast<AircraftTypeClass*>(pUnit->GetTechnoType());

					if (pAircraft->AirportBound)
					{
						// This aircraft won't count for the script action
						pUnit->EnterIdleMode(false, true);

						continue;
					}
				}

				nUnits++;

				if ((pUnit->DistanceFrom(pLeaderUnit) / 256.0) > closeEnough)
				{
					// Leader's location is too far from me. Regroup
					if (pUnit->Destination != pLeaderUnit)
					{
						pUnit->SetDestination(pLeaderUnit, false);
						pUnit->QueueMission(Mission::Move, false);
					}
				}
				else
				{
					// Is near of the leader, then protect the area
					if (pUnit->GetCurrentMission() != Mission::Area_Guard || pUnit->GetCurrentMission() != Mission::Attack)
						pUnit->QueueMission(Mission::Area_Guard, true);

					nTogether++;
				}
			}
		}


		if (nUnits >= 0
			&& nUnits == nTogether
			&& (initialCountdown == 0
				|| (initialCountdown > 0
					&& countdown <= 0)))
		{
			pExt->Countdown_RegroupAtLeader = -1;
			// This action finished
			pTeam->StepCompleted = true;

			return;
		}
	}
}

void ScriptExt::DecreaseCurrentTriggerWeight(TeamClass* pTeam, bool forceJumpLine = true, double modifier = 0)
{
	if (modifier <= 0)
		modifier = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	if (modifier <= 0)
		modifier = RulesClass::Instance->AITriggerFailureWeightDelta;
	else
		modifier = modifier * (-1);

	ModifyCurrentTriggerWeight(pTeam, forceJumpLine, modifier);

	// This action finished
	if (forceJumpLine)
		pTeam->StepCompleted = true;

	return;
}

void ScriptExt::IncreaseCurrentTriggerWeight(TeamClass* pTeam, bool forceJumpLine = true, double modifier = 0)
{
	if (modifier <= 0)
		modifier = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	if (modifier <= 0)
		modifier = abs(RulesClass::Instance->AITriggerSuccessWeightDelta);

	ScriptExt::ModifyCurrentTriggerWeight(pTeam, forceJumpLine, modifier);

	// This action finished
	if (forceJumpLine)
		pTeam->StepCompleted = true;

	return;
}

void ScriptExt::ModifyCurrentTriggerWeight(TeamClass* pTeam, bool forceJumpLine = true, double modifier = 0)
{
	AITriggerTypeClass* pTriggerType = nullptr;
	auto pTeamType = pTeam->Type;
	bool found = false;

	for (int i = 0; i < AITriggerTypeClass::Array->Count && !found; i++)
	{
		auto pTriggerTeam1Type = AITriggerTypeClass::Array->GetItem(i)->Team1;
		auto pTriggerTeam2Type = AITriggerTypeClass::Array->GetItem(i)->Team2;

		if (pTeamType
			&& ((pTriggerTeam1Type && pTriggerTeam1Type == pTeamType)
				|| (pTriggerTeam2Type && pTriggerTeam2Type == pTeamType)))
		{
			found = true;
			pTriggerType = AITriggerTypeClass::Array->GetItem(i);
		}
	}

	if (found)
	{
		pTriggerType->Weight_Current += modifier;

		if (pTriggerType->Weight_Current > pTriggerType->Weight_Maximum)
		{
			pTriggerType->Weight_Current = pTriggerType->Weight_Maximum;
		}
		else
		{
			if (pTriggerType->Weight_Current < pTriggerType->Weight_Minimum)
				pTriggerType->Weight_Current = pTriggerType->Weight_Minimum;
		}
	}
}

void ScriptExt::WaitIfNoTarget(TeamClass* pTeam, int attempts = 0)
{
	// This method modifies the new attack actions preventing Team's Trigger to jump to next script action
	// attempts == number of times the Team will wait if Mission_Attack(...) can't find a new target.
	if (attempts < 0)
		attempts = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	if (pTeamData)
	{
		if (attempts <= 0)
			pTeamData->WaitNoTargetAttempts = -1; // Infinite waits if no target
		else
			pTeamData->WaitNoTargetAttempts = attempts;
	}

	// This action finished
	pTeam->StepCompleted = true;

	return;
}

void ScriptExt::TeamWeightReward(TeamClass* pTeam, double award = 0)
{
	if (award <= 0)
		award = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	if (pTeamData)
	{
		if (award > 0)
			pTeamData->NextSuccessWeightAward = award;
	}

	// This action finished
	pTeam->StepCompleted = true;

	return;
}

void ScriptExt::PickRandomScript(TeamClass* pTeam, int idxScriptsList = -1)
{
	if (idxScriptsList <= 0)
		idxScriptsList = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	bool changeFailed = true;

	if (idxScriptsList >= 0)
	{
		if ((size_t)idxScriptsList < RulesExt::Global()->AIScriptsLists.size())
		{
			auto& objectsList = RulesExt::Global()->AIScriptsLists[idxScriptsList];

			if (objectsList.size() > 0)
			{
				int IdxSelectedObject = ScenarioClass::Instance->Random.RandomRanged(0, objectsList.size() - 1);

				ScriptTypeClass* pNewScript = objectsList[IdxSelectedObject];
				if (pNewScript->ActionsCount > 0)
				{
					changeFailed = false;
					TeamExt::ExtMap.Find(pTeam)->PreviousScriptList.push_back(pTeam->CurrentScript);
					pTeam->CurrentScript = nullptr;
					pTeam->CurrentScript = GameCreate<ScriptClass>(pNewScript);

					// Ready for jumping to the first line of the new script
					pTeam->CurrentScript->CurrentMission = -1;
					pTeam->StepCompleted = true;

					return;
				}
				else
				{
					pTeam->StepCompleted = true;
					ScriptExt::Log("AI Scripts - PickRandomScript: [%s] Aborting Script change because [%s] has 0 Action scripts!\n", pTeam->Type->ID, pNewScript->ID);

					return;
				}
			}
		}
	}

	// This action finished
	if (changeFailed)
	{
		pTeam->StepCompleted = true;
		ScriptExt::Log("AI Scripts - PickRandomScript: [%s] [%s] Failed to change the Team Script with a random one!\n", pTeam->Type->ID, pTeam->CurrentScript->Type->ID);
	}
}

void ScriptExt::SetCloseEnoughDistance(TeamClass* pTeam, double distance = -1)
{
	// This passive method replaces the CloseEnough value from rulesmd.ini by a custom one. Used by Mission_Move()
	if (distance <= 0)
		distance = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	auto const pTeamData = TeamExt::ExtMap.Find(pTeam);

	if (pTeamData)
	{
		if (distance > 0)
			pTeamData->CloseEnough = distance;
	}

	if (distance <= 0)
		pTeamData->CloseEnough = RulesClass::Instance->CloseEnough / 256.0;

	// This action finished
	pTeam->StepCompleted = true;

	return;
}

void ScriptExt::UnregisterGreatSuccess(TeamClass* pTeam)
{
	pTeam->AchievedGreatSuccess = false;
	pTeam->StepCompleted = true;
}

void ScriptExt::SetMoveMissionEndMode(TeamClass* pTeam, int mode = 0)
{
	// This passive method replaces the CloseEnough value from rulesmd.ini by a custom one. Used by Mission_Move()
	if (mode < 0 || mode > 2)
		mode = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	auto const pTeamData = TeamExt::ExtMap.Find(pTeam);

	if (pTeamData)
	{
		if (mode >= 0 && mode <= 2)
			pTeamData->MoveMissionEndMode = mode;
	}

	// This action finished
	pTeam->StepCompleted = true;

	return;
}

bool ScriptExt::MoveMissionEndStatus(TeamClass* pTeam, TechnoClass* pFocus, FootClass* pLeader = nullptr, int mode = 0)
{
	if (!pTeam || !pFocus || mode < 0)
		return false;

	if (mode != 2 && mode != 1 && !pLeader)
		return false;

	double closeEnough = RulesClass::Instance->CloseEnough / 256.0;

	auto const pTeamData = TeamExt::ExtMap.Find(pTeam);

	if (pTeamData && pTeamData->CloseEnough > 0)
		closeEnough = pTeamData->CloseEnough;

	bool bForceNextAction = false;

	if (mode == 2)
		bForceNextAction = true;

	// Team already have a focused target
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		if (IsUnitAvailable(pUnit, true, true)
			&& !pUnit->TemporalTargetingMe
			&& !pUnit->BeingWarpedOut)
		{
			if (mode == 2)
			{
				// Default mode: all members in range
				if ((pUnit->DistanceFrom(pFocus->GetCell()) / 256.0) > closeEnough)
				{
					bForceNextAction = false;

					if (pUnit->WhatAmI() == AbstractType::Aircraft && pUnit->Ammo > 0)
						pUnit->QueueMission(Mission::Move, false);

					continue;
				}
				else
				{
					if (pUnit->WhatAmI() == AbstractType::Aircraft && pUnit->Ammo <= 0)
					{
						pUnit->EnterIdleMode(false, true);

						continue;
					}
				}
			}
			else
			{
				if (mode == 1)
				{
					// Any member in range
					if ((pUnit->DistanceFrom(pFocus->GetCell()) / 256.0) > closeEnough)
					{
						if (pUnit->WhatAmI() == AbstractType::Aircraft && pUnit->Ammo > 0)
							pUnit->QueueMission(Mission::Move, false);

						continue;
					}
					else
					{
						bForceNextAction = true;

						if (pUnit->WhatAmI() == AbstractType::Aircraft && pUnit->Ammo <= 0)
						{
							pUnit->EnterIdleMode(false, true);

							continue;
						}
					}
				}
				else
				{
					// All other cases: Team Leader mode in range
					if (pLeader)
					{
						if ((pUnit->DistanceFrom(pFocus->GetCell()) / 256.0) > closeEnough)
						{
							if (pUnit->WhatAmI() == AbstractType::Aircraft && pUnit->Ammo > 0)
								pUnit->QueueMission(Mission::Move, false);

							continue;
						}
						else
						{
							if (pUnit->IsTeamLeader)
								bForceNextAction = true;

							if (pUnit->WhatAmI() == AbstractType::Aircraft && pUnit->Ammo <= 0)
							{
								pUnit->EnterIdleMode(false, true);

								continue;
							}
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	return bForceNextAction;
}

void ScriptExt::SkipNextAction(TeamClass* pTeam, int successPercentage = 0)
{
	// This team has no units! END
	if (!pTeam)
	{
		// This action finished
		pTeam->StepCompleted = true;
		ScriptExt::Log("AI Scripts - SkipNextAction: [%s] [%s] (line: %d) Jump to next line: %d = %d,%d -> (No team members alive)\n",
			pTeam->Type->ID, pTeam->CurrentScript->Type->ID, pTeam->CurrentScript->CurrentMission, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Action, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument,
			pTeam->CurrentScript->CurrentMission + 1, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission + 1].Action,
			pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission + 1].Argument);

		return;
	}

	if (successPercentage < 0 || successPercentage > 100)
		successPercentage = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	if (successPercentage < 0)
		successPercentage = 0;

	if (successPercentage > 100)
		successPercentage = 100;

	int percentage = ScenarioClass::Instance->Random.RandomRanged(1, 100);

	if (percentage <= successPercentage)
	{
		ScriptExt::Log("AI Scripts - SkipNextAction: [%s] [%s] (line: %d = %d,%d) Next script line skipped successfuly. Next line will be: %d = %d,%d\n",
			pTeam->Type->ID, pTeam->CurrentScript->Type->ID, pTeam->CurrentScript->CurrentMission, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Action, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument, pTeam->CurrentScript->CurrentMission + 2,
			pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission + 2].Action, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission + 2].Argument);

		pTeam->CurrentScript->CurrentMission++;
	}

	// This action finished
	pTeam->StepCompleted = true;
}

void ScriptExt::TeamMemberSetGroup(TeamClass* pTeam, int group)
{
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		pUnit->Group = group;
		pTeam->AddMember(pUnit, false);
	}
	pTeam->StepCompleted = true;
}

bool ScriptExt::StopTeamMemberMoving(TeamClass* pTeam)
{
	bool stillMoving = false;
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		if (pUnit->CurrentMission == Mission::Move || (pUnit->Locomotor->Is_Moving() && !pUnit->GetTechnoType()->JumpJet))
		{
			pUnit->ForceMission(Mission::Wait);
			pUnit->CurrentTargets.Clear();
			pUnit->SetTarget(nullptr);
			pUnit->SetFocus(nullptr);
			pUnit->SetDestination(nullptr, true);
			stillMoving = true;
		}
	}
	return stillMoving;
}

void ScriptExt::DistributedLoadOntoTransport(TeamClass* pTeam, int nArg)
{
	const int T_NO_GATHER = 0, T_COUNTDOWN = 1, T_AVGPOS = 2;
	/*
	type 0: stop member from gathering and begin load now
	type 1: don't stop, maintain previous action, countdown LOWORD seconds then begin load
	type 2: don't stop, gether around first member's position range LOWORD then begin load
	*/

	// can proceed to load calculate logic
	const int R_CAN_PROCEED = 1;
	// type 1 waiting stage, start timer and wait timer stop, when stop, proceed to loading
	const int R_WAITING = 2;
	// type 2 waiting stage, start timer and wait timer stop, when stop, check average position
	const int R_WAIT_POS = 3;

	int nType = HIWORD(nArg);
	int nNum = LOWORD(nArg);
	auto pExt = TeamExt::ExtMap.Find(pTeam);
	auto remainingSize = [](FootClass* src)
	{
		auto type = src->GetTechnoType();
		return type->Passengers - src->Passengers.GetTotalSize();
	};

	auto pFoot = pTeam->FirstUnit;
	auto timer = pFoot->BlockagePathTimer;
	// Wait for timer stop
	if (pTeam->GuardAreaTimer.TimeLeft > 0)
	{
		pTeam->GuardAreaTimer.TimeLeft--;
		return;
	}
	// type 1 times up
	else if (pExt->GenericStatus == R_WAITING)
	{
		bool stillMoving = StopTeamMemberMoving(pTeam);
		if (!stillMoving)
		{
			pExt->GenericStatus = R_CAN_PROCEED;
			goto beginLoad;
		}
	}
	// type 2 times up, check distance
	else if (pExt->GenericStatus == R_WAIT_POS)
	{
		bool canProceed = true;
		auto pCell = pFoot->GetCell();

		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			// no blockage, just keep moving
			if (pUnit->Locomotor->Is_Moving_Now())
			{
				canProceed = false;
				continue;
			}

			if (pUnit->DistanceFrom(pFoot) / 256 <= nNum)
			{
				pUnit->StopMoving();
				pUnit->CurrentTargets.Clear();
				pUnit->SetTarget(nullptr);
				pUnit->SetFocus(nullptr);
				pUnit->SetDestination(nullptr, true);
			}
			else
			{
				CoordStruct coords = pCell->GetCoords();
				pUnit->MoveTo(&coords);
				canProceed = false;
			}
		}
		if (canProceed)
		{
			pExt->GenericStatus = R_CAN_PROCEED;
			return;
		}
		else
		{
			pTeam->GuardAreaTimer.Start(5);
			return;
		}
	}

	// Check if this script can skip this frame
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		// If anyone is entering transport, means this script is now proceeding
		if (pUnit->GetCurrentMission() == Mission::Enter)
			return;
	}

	// Status jump
	if (pExt->GenericStatus == R_CAN_PROCEED)
		goto beginLoad;

	// switch mode
	// 0: stop member from gathering
	if (nType == T_NO_GATHER)
	{
		// If anyone is moving, stop now, and add timer
		// why team member will converge upon team creation
		// fuck you westwood
		pTeam->Focus = nullptr;
		pTeam->QueuedFocus = nullptr;
		bool stillMoving = StopTeamMemberMoving(pTeam);
		if (stillMoving)
		{
			pTeam->GuardAreaTimer.Start(45);
			return;
		}
	}
	// 1: don't stop, maintain previous action
	// default: gather near team center position (upon team creation, it's auto), countdown timer LOWORD seconds
	else if (nType == T_COUNTDOWN)
	{
		if (pExt->GenericStatus == R_WAITING)
			return;
		pTeam->GuardAreaTimer.Start(nNum * 15);
		pExt->GenericStatus = R_WAITING;
		return;
	}
	// 2: don't stop, manually gather around first member's position
	else if (nType == T_AVGPOS)
	{
		auto pCell = pFoot->GetCell();
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			CoordStruct coords = pCell->GetCoords();
			pUnit->MoveTo(&coords);
		}
		pTeam->GuardAreaTimer.Start(5);
		pExt->GenericStatus = R_WAIT_POS;
		return;
	}

beginLoad:
	// Now we're talking
	DynamicVectorClass<FootClass*> transports, passengers;
	std::unordered_map<FootClass*, double> transportSpaces;
	// Find max SizeLimit to determine which type is considered as transport
	double maxSizeLimit = 0;
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		auto pType = pUnit->GetTechnoType();
		maxSizeLimit = std::max(maxSizeLimit, pType->SizeLimit);
	}
	// No transports remaining
	if (maxSizeLimit == 0)
	{
		pTeam->StepCompleted = true;
		pExt->GenericStatus = 0;
		return;
	}
	// All member share this SizeLimit will consider as transport
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		auto pType = pUnit->GetTechnoType();
		if (pType->SizeLimit == maxSizeLimit)
		{
			int space = remainingSize(pUnit);
			transports.AddItem(pUnit);
			transportSpaces[pUnit] = space;
		}
		else
			passengers.AddItem(pUnit);
	}
	// If there are no passengers
	// then this script is done
	if (passengers.Count == 0)
	{
		pTeam->StepCompleted = true;
		pExt->GenericStatus = 0;
		return;
	}
	// If transport is on building, scatter, and discard this frame
	for (auto pUnit : transports)
	{
		if (pUnit->GetCell()->GetBuilding())
		{
			pUnit->Scatter(pUnit->GetCoords(), true, false);
			return;
		}
	}

	// Load logic
	// range prioritize
	bool passengerLoading = false;
	// larger size first
	auto sizeSort = [](FootClass* a, FootClass* b)
	{
		return a->GetTechnoType()->Size > b->GetTechnoType()->Size;
	};
	std::sort(passengers.begin(), passengers.end(), sizeSort);
	for (auto pPassenger : passengers)
	{
		auto pPassengerType = pPassenger->GetTechnoType();
		// Is legal loadable unit ?
		if (pPassengerType->WhatAmI() != AbstractType::AircraftType &&
				!pPassengerType->ConsideredAircraft &&
				TECHNO_IS_ALIVE(pPassenger))
		{
			FootClass* targetTransport = nullptr;
			double distance = INFINITY;
			for (auto pTransport : transports)
			{
				auto pTransportType = pTransport->GetTechnoType();
				double currentSpace = transportSpaces[pTransport];
				// Can unit load onto this car ?
				if (currentSpace > 0 &&
					pPassengerType->Size > 0 &&
					pPassengerType->Size <= pTransportType->SizeLimit &&
					pPassengerType->Size <= currentSpace)
				{
					double d = pPassenger->DistanceFrom(pTransport);
					if (d < distance)
					{
						targetTransport = pTransport;
						distance = d;
					}
				}
			}
			// This is nearest available transport
			if (targetTransport)
			{
				// Get on the car
				if (pPassenger->GetCurrentMission() != Mission::Enter)
				{
					pPassenger->QueueMission(Mission::Enter, true);
					pPassenger->SetTarget(nullptr);
					pPassenger->SetDestination(targetTransport, false);
					transportSpaces[targetTransport] -= pPassengerType->Size;
					passengerLoading = true;
				}
			}
		}
	}
	// If no one is loading, this script is done
	if (!passengerLoading)
	{
		pTeam->StepCompleted = true;
		pExt->GenericStatus = 0;
	}
	// Load logic
	// speed prioritize
	//for (auto pTransport : transports)
	//{
	//	DynamicVectorClass<FootClass*> loadedUnits;
	//	auto pTransportType = pTransport->GetTechnoType();
	//	double currentSpace = pTransportType->Passengers - pTransport->Passengers.GetTotalSize();
	//	if (currentSpace == 0) continue;
	//	for (auto pUnit : passengers)
	//	{
	//		auto pUnitType = pUnit->GetTechnoType();
	//		// Is legal loadable unit ?
	//		if (pUnitType->WhatAmI() != AbstractType::AircraftType &&
	//			!pUnit->InLimbo &&
	//			!pUnitType->ConsideredAircraft &&
	//			pUnit->Health > 0 &&
	//			pUnit != pTransport)
	//		{
	//			// Can unit load onto this car ?
	//			if (pUnitType->Size > 0
	//				&& pUnitType->Size <= pTransportType->SizeLimit
	//				&& pUnitType->Size <= currentSpace)
	//			{
	//				// Get on the car
	//				if (pUnit->GetCurrentMission() != Mission::Enter)
	//				{
	//					transportsNoSpace = false;
	//					pUnit->QueueMission(Mission::Enter, true);
	//					pUnit->SetTarget(nullptr);
	//					pUnit->SetDestination(pTransport, false);
	//					currentSpace -= pUnitType->Size;
	//					loadedUnits.AddItem(pUnit);
	//				}
	//			}
	//		}
	//		if (currentSpace == 0) break;
	//	}
	//	for (auto pRemove : loadedUnits) passengers.Remove(pRemove);
	//	loadedUnits.Clear();
	//}
	//if (transportsNoSpace)
	//{
	//	pTeam->StepCompleted = true;
	//	return;
	//}
}

bool ScriptExt::IsValidFriendlyTarget(TeamClass* pTeam, int group, TechnoClass* target, bool isSelfNaval, bool isSelfAircraft, bool isFriendly)
{
	if (!target) return false;
	if (TECHNO_IS_ALIVE(target) && target->Group == group)
	{
		auto pType = target->GetTechnoType();
		// Friendly?
		if (isFriendly ^ pTeam->Owner->IsAlliedWith(target->Owner)) 
			return false;
		// Only aircraft team can follow friendly aircraft
		if (isSelfAircraft) 
			return true;
		else if (pType->ConsideredAircraft)
			return false;
		// If team is naval, only follow friendly naval
		if (isSelfNaval ^ pType->Naval) 
			return false;
		// No underground
		if (target->InWhichLayer() == Layer::Underground) 
			return false;

		return true;
	}
	return false;
}

void ScriptExt::FollowFriendlyByGroup(TeamClass* pTeam, int group)
{
	bool isSelfNaval = true, isSelfAircraft = true;
	double distMin = std::numeric_limits<double>::infinity();
	CellStruct* teamPosition = nullptr;
	TechnoClass* target = nullptr;
	// Use timer to reduce unnecessary cycle
	if (pTeam->GuardAreaTimer.TimeLeft <= 0)
	{
		// If all member is naval, will only follow friendly navals
		for (auto pMember = pTeam->FirstUnit; pMember; pMember = pMember->NextTeamMember)
		{
			if (!teamPosition) 
				teamPosition = &(pMember->GetCell()->MapCoords);
			auto pMemberType = pMember->GetTechnoType();
			isSelfNaval = isSelfNaval && pMemberType->Naval;
			isSelfAircraft = isSelfAircraft && pMemberType->ConsideredAircraft;
		}
		// If previous target is valid, skip this check
		auto dest = abstract_cast<TechnoClass*>(pTeam->Focus);
		if (IsValidFriendlyTarget(pTeam, group, dest, isSelfNaval, isSelfAircraft, true))
		{
			for (auto pMember = pTeam->FirstUnit; pMember; pMember = pMember->NextTeamMember)
			{
				double d = pMember->GetMapCoords().DistanceFrom(dest->GetCell()->MapCoords);
				if (d * 256 > RulesClass::Instance->CloseEnough)
				{
					if (isSelfAircraft)
					{
						pMember->SetDestination(dest, false);
						pMember->QueueMission(Mission::Move, true);
					}
					else
					{
						pMember->QueueMission(Mission::Area_Guard, true);
						pMember->SetTarget(nullptr);
						pMember->SetFocus(dest);
					}
				}
				else
				{
					if (!isSelfAircraft)
					{
						pMember->SetDestination(nullptr, false);
						pMember->CurrentMission = Mission::Area_Guard;
						pMember->Focus = nullptr;
					}
					else
						pMember->QueueMission(Mission::Area_Guard, true);
				}
			}
			pTeam->GuardAreaTimer.Start(30);
			return;
		}
		// Now looking for target
		for (int i = 0; i < TechnoClass::Array->Count; i++)
		{
			auto pTechno = (*TechnoClass::Array)[i];

			if (IsValidFriendlyTarget(pTeam, group, pTechno, isSelfNaval, isSelfAircraft, true))
			{
				// candidate
				auto coord = pTechno->GetCell();
				double distance = coord->MapCoords.DistanceFromSquared(*teamPosition);
				if (distance < distMin)
				{
					target = pTechno;
					distMin = distance;
				}
			}
		}
		if (target)
		{
			if (isSelfAircraft)
			{
				for (auto pMember = pTeam->FirstUnit; pMember; pMember = pMember->NextTeamMember)
				{
					//pMember->SetTarget(target);
					//pMember->SetFocus(target);
					pMember->SetDestination(target, false);
					pMember->QueueMission(Mission::Move, true);
				}
			}
			else
			{
				for (auto pMember = pTeam->FirstUnit; pMember; pMember = pMember->NextTeamMember)
				{
					pMember->QueueMission(Mission::Area_Guard, true);
					pMember->SetTarget(nullptr);
					pMember->SetFocus(target);
				}
			}
			pTeam->Focus = target;
			pTeam->GuardAreaTimer.Start(30);
		}
		// If there's no valid target, continue script
		else
			pTeam->StepCompleted = true;
	}
	else
		pTeam->GuardAreaTimer.TimeLeft--;
}

void ScriptExt::RallyUnitInMap(TeamClass* pTeam, int nArg)
{
	auto pType = pTeam->Type;
	for (int i = 0; i < FootClass::Array->Count; i++)
	{
		auto pFoot = (*FootClass::Array)[i];

		// Must be owner and with same group
		if (pFoot
			&& TECHNO_IS_ALIVE(pFoot)
			&& pFoot->Owner == pTeam->Owner
			&& pFoot->Group == pType->Group
			&& IsValidRallyTarget(pTeam, pFoot, nArg))
		{
			// This will bypass any recruiting restrictions, except group (this action's purpose)
			if (pType->Recruiter)
				pTeam->AddMember(pFoot, true);
			// Only rally unit's parent team set Recruitable = yes and Priority less than this team
			else if (pFoot->RecruitableB)
			{
				if (pFoot->Team)
				{
					if (pFoot->Team->Type->Priority < pType->Priority)
						pTeam->AddMember(pFoot, true);
				}
				else
					pTeam->AddMember(pFoot, true);
			}
		}
	}
	pTeam->StepCompleted = true;
}

void ScriptExt::RallyUnitInMapWithLeader(TeamClass* pTeam, int nArg)
{
	if (!pTeam)
		return;

	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	if (!pTeamData)
		return;

	// Find the Leader
	auto pLeaderUnit = pTeamData->TeamLeader;
	if (!pLeaderUnit
		|| !pLeaderUnit->IsAlive
		|| pLeaderUnit->Health <= 0
		|| pLeaderUnit->InLimbo
		|| !pLeaderUnit->IsOnMap
		|| pLeaderUnit->Absorbed)
	{
		pLeaderUnit = FindTheTeamLeader(pTeam);
		pTeamData->TeamLeader = pLeaderUnit;
	}

	if (!pLeaderUnit)
	{
		// This action finished
		pTeam->StepCompleted = true;
		return;
	}

	auto pType = pTeam->Type;
	for (int i = 0; i < FootClass::Array->Count; i++)
	{
		auto pFoot = (*FootClass::Array)[i];

		// Must be owner and with same group
		if (pFoot
			&& TECHNO_IS_ALIVE(pFoot)
			&& pFoot->Owner == pTeam->Owner
			&& pFoot->Group == pLeaderUnit->Group
			&& IsValidRallyTarget(pTeam, pFoot, nArg))
		{
			// This will bypass any recruiting restrictions, except group (this action's purpose)
			if (pType->Recruiter)
				pTeam->AddMember(pFoot, true);
			// Only rally unit's parent team set Recruitable = yes and Priority less than this team
			else if (pFoot->RecruitableB)
			{
				if (pFoot->Team)
				{
					if (pFoot->Team->Type->Priority < pType->Priority)
						pTeam->AddMember(pFoot, true);
				}
				else
					pTeam->AddMember(pFoot, true);
			}
		}
	}
	pTeam->StepCompleted = true;
}

bool ScriptExt::IsValidRallyTarget(TeamClass* pTeam, FootClass* pFoot, int nType)
{
	auto type = pFoot->WhatAmI();
	auto pTechnoType = pFoot->GetTechnoType();
	auto pAircraftType = abstract_cast<AircraftTypeClass*>(pTechnoType);
	auto pInfantryType = abstract_cast<InfantryTypeClass*>(pTechnoType);
	TaskForceClass* pTaskforce = pTeam->Type->TaskForce;
	if (!pTechnoType)
		return false;

	switch (nType)
	{
	case 0: // Anything
		return true;
	case 1: // Infantry
		return type == AbstractType::Infantry;
	case 2: // Vehicles
		return type == AbstractType::Unit;
	case 3: // Air Units
		return pTechnoType->ConsideredAircraft || type == AbstractType::Aircraft;
	case 4: // Naval units
		return type == AbstractType::Unit && pTechnoType->Naval;
	case 5: // Ground units
		return (type == AbstractType::Unit || type == AbstractType::Infantry)
			&& (!pTechnoType->ConsideredAircraft && !pTechnoType->Naval);
	case 6: // Dockable fighters
		if (pAircraftType)
			return type == AbstractType::Aircraft && pAircraftType->AirportBound;
		return false;
	case 7: // Same type in taskforce
		if (pTaskforce)
		{
			for (auto const& pEntry : pTaskforce->Entries)
			{
				if (pEntry.Type && pEntry.Type == pTechnoType)
					return true;
			}
		}
		return false;
	case 8: // Ground units, except for miners and MCVs
		if (pInfantryType)
			return (type == AbstractType::Unit || type == AbstractType::Infantry)
			&& (!pTechnoType->ConsideredAircraft && !pTechnoType->Naval && !pTechnoType->ResourceGatherer && !pTechnoType->DeploysInto && !pTechnoType->Slaved && !pInfantryType->Agent);
		return (type == AbstractType::Unit || type == AbstractType::Infantry)
			&& (!pTechnoType->ConsideredAircraft && !pTechnoType->Naval && !pTechnoType->ResourceGatherer && !pTechnoType->DeploysInto && !pTechnoType->Slaved);
	default:
		return false;
	}
}

void ScriptExt::VariablesHandler(TeamClass* pTeam, PhobosScripts eAction, int nArg)
{
	struct operation_set { int operator()(const int& a, const int& b) { return b; } };
	struct operation_add { int operator()(const int& a, const int& b) { return a + b; } };
	struct operation_minus { int operator()(const int& a, const int& b) { return a - b; } };
	struct operation_multiply { int operator()(const int& a, const int& b) { return a * b; } };
	struct operation_divide { int operator()(const int& a, const int& b) { return a / b; } };
	struct operation_mod { int operator()(const int& a, const int& b) { return a % b; } };
	struct operation_leftshift { int operator()(const int& a, const int& b) { return a << b; } };
	struct operation_rightshift { int operator()(const int& a, const int& b) { return a >> b; } };
	struct operation_reverse { int operator()(const int& a, const int& b) { return ~a; } };
	struct operation_xor { int operator()(const int& a, const int& b) { return a ^ b; } };
	struct operation_or { int operator()(const int& a, const int& b) { return a | b; } };
	struct operation_and { int operator()(const int& a, const int& b) { return a & b; } };

	int nLoArg = LOWORD(nArg);
	int nHiArg = HIWORD(nArg);

	switch (eAction)
	{
	case PhobosScripts::LocalVariableSet:
		VariableOperationHandler<false, operation_set>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableAdd:
		VariableOperationHandler<false, operation_add>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableMinus:
		VariableOperationHandler<false, operation_minus>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableMultiply:
		VariableOperationHandler<false, operation_multiply>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableDivide:
		VariableOperationHandler<false, operation_divide>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableMod:
		VariableOperationHandler<false, operation_mod>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableLeftShift:
		VariableOperationHandler<false, operation_leftshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableRightShift:
		VariableOperationHandler<false, operation_rightshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableReverse:
		VariableOperationHandler<false, operation_reverse>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableXor:
		VariableOperationHandler<false, operation_xor>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableOr:
		VariableOperationHandler<false, operation_or>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableAnd:
		VariableOperationHandler<false, operation_and>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableSet:
		VariableOperationHandler<true, operation_set>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableAdd:
		VariableOperationHandler<true, operation_add>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableMinus:
		VariableOperationHandler<true, operation_minus>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableMultiply:
		VariableOperationHandler<true, operation_multiply>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableDivide:
		VariableOperationHandler<true, operation_divide>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableMod:
		VariableOperationHandler<true, operation_mod>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableLeftShift:
		VariableOperationHandler<true, operation_leftshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableRightShift:
		VariableOperationHandler<true, operation_rightshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableReverse:
		VariableOperationHandler<true, operation_reverse>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableXor:
		VariableOperationHandler<true, operation_xor>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableOr:
		VariableOperationHandler<true, operation_or>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableAnd:
		VariableOperationHandler<true, operation_and>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableSetByLocal:
		VariableBinaryOperationHandler<false, false, operation_set>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableAddByLocal:
		VariableBinaryOperationHandler<false, false, operation_add>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableMinusByLocal:
		VariableBinaryOperationHandler<false, false, operation_minus>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableMultiplyByLocal:
		VariableBinaryOperationHandler<false, false, operation_multiply>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableDivideByLocal:
		VariableBinaryOperationHandler<false, false, operation_divide>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableModByLocal:
		VariableBinaryOperationHandler<false, false, operation_mod>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableLeftShiftByLocal:
		VariableBinaryOperationHandler<false, false, operation_leftshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableRightShiftByLocal:
		VariableBinaryOperationHandler<false, false, operation_rightshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableReverseByLocal:
		VariableBinaryOperationHandler<false, false, operation_reverse>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableXorByLocal:
		VariableBinaryOperationHandler<false, false, operation_xor>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableOrByLocal:
		VariableBinaryOperationHandler<false, false, operation_or>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableAndByLocal:
		VariableBinaryOperationHandler<false, false, operation_and>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableSetByLocal:
		VariableBinaryOperationHandler<false, true, operation_set>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableAddByLocal:
		VariableBinaryOperationHandler<false, true, operation_add>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableMinusByLocal:
		VariableBinaryOperationHandler<false, true, operation_minus>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableMultiplyByLocal:
		VariableBinaryOperationHandler<false, true, operation_multiply>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableDivideByLocal:
		VariableBinaryOperationHandler<false, true, operation_divide>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableModByLocal:
		VariableBinaryOperationHandler<false, true, operation_mod>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableLeftShiftByLocal:
		VariableBinaryOperationHandler<false, true, operation_leftshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableRightShiftByLocal:
		VariableBinaryOperationHandler<false, true, operation_rightshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableReverseByLocal:
		VariableBinaryOperationHandler<false, true, operation_reverse>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableXorByLocal:
		VariableBinaryOperationHandler<false, true, operation_xor>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableOrByLocal:
		VariableBinaryOperationHandler<false, true, operation_or>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableAndByLocal:
		VariableBinaryOperationHandler<false, true, operation_and>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableSetByGlobal:
		VariableBinaryOperationHandler<true, false, operation_set>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableAddByGlobal:
		VariableBinaryOperationHandler<true, false, operation_add>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableMinusByGlobal:
		VariableBinaryOperationHandler<true, false, operation_minus>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableMultiplyByGlobal:
		VariableBinaryOperationHandler<true, false, operation_multiply>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableDivideByGlobal:
		VariableBinaryOperationHandler<true, false, operation_divide>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableModByGlobal:
		VariableBinaryOperationHandler<true, false, operation_mod>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableLeftShiftByGlobal:
		VariableBinaryOperationHandler<true, false, operation_leftshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableRightShiftByGlobal:
		VariableBinaryOperationHandler<true, false, operation_rightshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableReverseByGlobal:
		VariableBinaryOperationHandler<true, false, operation_reverse>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableXorByGlobal:
		VariableBinaryOperationHandler<true, false, operation_xor>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableOrByGlobal:
		VariableBinaryOperationHandler<true, false, operation_or>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::LocalVariableAndByGlobal:
		VariableBinaryOperationHandler<true, false, operation_and>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableSetByGlobal:
		VariableBinaryOperationHandler<true, true, operation_set>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableAddByGlobal:
		VariableBinaryOperationHandler<true, true, operation_add>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableMinusByGlobal:
		VariableBinaryOperationHandler<true, true, operation_minus>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableMultiplyByGlobal:
		VariableBinaryOperationHandler<true, true, operation_multiply>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableDivideByGlobal:
		VariableBinaryOperationHandler<true, true, operation_divide>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableModByGlobal:
		VariableBinaryOperationHandler<true, true, operation_mod>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableLeftShiftByGlobal:
		VariableBinaryOperationHandler<true, true, operation_leftshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableRightShiftByGlobal:
		VariableBinaryOperationHandler<true, true, operation_rightshift>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableReverseByGlobal:
		VariableBinaryOperationHandler<true, true, operation_reverse>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableXorByGlobal:
		VariableBinaryOperationHandler<true, true, operation_xor>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableOrByGlobal:
		VariableBinaryOperationHandler<true, true, operation_or>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::GlobalVariableAndByGlobal:
		VariableBinaryOperationHandler<true, true, operation_and>(pTeam, nLoArg, nHiArg); break;
	case PhobosScripts::ChangeTeamGroup:
		TeamMemberSetGroup(pTeam, nArg); break;
	case PhobosScripts::DistributedLoading:
		DistributedLoadOntoTransport(pTeam, nArg == 0); break;
	case PhobosScripts::FollowFriendlyByGroup:
		FollowFriendlyByGroup(pTeam, nArg); break;
	}
}

template<bool IsGlobal, class _Pr>
void ScriptExt::VariableOperationHandler(TeamClass* pTeam, int nVariable, int Number)
{
	auto itr = ScenarioExt::Global()->Variables[IsGlobal].find(nVariable);

	if (itr != ScenarioExt::Global()->Variables[IsGlobal].end())
	{
		itr->second.Value = _Pr()(itr->second.Value, Number);
		if (IsGlobal)
			TagClass::NotifyGlobalChanged(nVariable);
		else
			TagClass::NotifyLocalChanged(nVariable);
	}

	pTeam->StepCompleted = true;
}

template<bool IsSrcGlobal, bool IsGlobal, class _Pr>
void ScriptExt::VariableBinaryOperationHandler(TeamClass* pTeam, int nVariable, int nVarToOperate)
{
	auto itr = ScenarioExt::Global()->Variables[IsSrcGlobal].find(nVarToOperate);

	if (itr != ScenarioExt::Global()->Variables[IsSrcGlobal].end())
		VariableOperationHandler<IsGlobal, _Pr>(pTeam, nVariable, itr->second.Value);

	pTeam->StepCompleted = true;
}

FootClass* ScriptExt::FindTheTeamLeader(TeamClass* pTeam)
{
	FootClass* pLeaderUnit = nullptr;
	int bestUnitLeadershipValue = -1;
	bool teamLeaderFound = false;

	if (!pTeam)
		return pLeaderUnit;

	// Find the Leader or promote a new one
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		if (!pUnit)
			continue;

		bool isValidUnit = IsUnitAvailable(pUnit, true, true);

		// Preventing >1 leaders in teams
		if (teamLeaderFound || !isValidUnit)
		{
			pUnit->IsTeamLeader = false;
			continue;
		}

		if (pUnit->IsTeamLeader)
		{
			pLeaderUnit = pUnit;
			teamLeaderFound = true;

			continue;
		}

		// The team Leader will be used for selecting targets, if there are living Team Members then always exists 1 Leader.
		int unitLeadershipRating = pUnit->GetTechnoType()->LeadershipRating;

		if (unitLeadershipRating > bestUnitLeadershipValue)
		{
			pLeaderUnit = pUnit;
			bestUnitLeadershipValue = unitLeadershipRating;
		}
	}

	if (pLeaderUnit)
		pLeaderUnit->IsTeamLeader = true;

	return pLeaderUnit;
}

bool ScriptExt::IsExtVariableAction(int action)
{
	auto eAction = static_cast<PhobosScripts>(action);
	return eAction >= PhobosScripts::LocalVariableAdd && eAction <= PhobosScripts::GlobalVariableAndByGlobal;
}

void ScriptExt::Set_ForceJump_Countdown(TeamClass* pTeam, bool repeatLine = false, int count = 0)
{
	if (!pTeam)
		return;

	auto const pTeamData = TeamExt::ExtMap.Find(pTeam);

	if (!pTeamData)
		return;

	if (count <= 0)
		count = 15 * pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	if (count > 0)
	{
		pTeamData->ForceJump_InitialCountdown = count;
		pTeamData->ForceJump_Countdown.Start(count);
		pTeamData->ForceJump_RepeatMode = repeatLine;
	}
	else
	{
		pTeamData->ForceJump_InitialCountdown = -1;
		pTeamData->ForceJump_Countdown.Stop();
		pTeamData->ForceJump_Countdown = -1;
		pTeamData->ForceJump_RepeatMode = false;
	}
	pTeamData->AllPassengers.Clear();
	pTeamData->AllyPassengers.Clear();
	pTeamData->IndividualTargets.Clear();
	pTeamData->CaptureTarget = nullptr;

	auto pScript = pTeam->CurrentScript;

	// This action finished
	pTeam->StepCompleted = true;
	ScriptExt::Log("AI Scripts - SetForceJumpCountdown: [%s] [%s](line: %d = %d,%d) Set Timed Jump -> (Countdown: %d, repeat action: %d)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, count, repeatLine);
}

void ScriptExt::Stop_ForceJump_Countdown(TeamClass* pTeam)
{
	if (!pTeam)
		return;

	auto const pTeamData = TeamExt::ExtMap.Find(pTeam);

	if (!pTeamData)
		return;

	pTeamData->AllPassengers.Clear();
	pTeamData->AllyPassengers.Clear();
	pTeamData->IndividualTargets.Clear();
	pTeamData->CaptureTarget = nullptr;

	pTeamData->ForceJump_InitialCountdown = -1;
	pTeamData->ForceJump_Countdown.Stop();
	pTeamData->ForceJump_Countdown = -1;
	pTeamData->ForceJump_RepeatMode = false;

	auto pScript = pTeam->CurrentScript;

	// This action finished
	pTeam->StepCompleted = true;
	ScriptExt::Log("AI Scripts - StopForceJumpCountdown: [%s] [%s](line: %d = %d,%d): Stopped Timed Jump\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument);
}

bool ScriptExt::IsUnitAvailable(TechnoClass* pTechno, bool checkIfInTransportOrAbsorbed, bool allowSubterranean)
{
	if (!pTechno)
		return false;

	bool isAvailable = pTechno->IsAlive && pTechno->Health > 0
		&& !pTechno->InLimbo && !pTechno->Transporter && !pTechno->Absorbed;

	bool isSubterranean = allowSubterranean && pTechno->InWhichLayer() == Layer::Underground;
	isAvailable &= pTechno->IsOnMap || isSubterranean;

	if (checkIfInTransportOrAbsorbed)
		isAvailable &= !pTechno->Absorbed && !pTechno->Transporter;

	return isAvailable;
}

void ScriptExt::Log(const char* pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);
	Debug::LogWithVArgs(pFormat, args);
	va_end(args);
}

void ScriptExt::TransportsReturn(TeamClass* pTeam, FootClass* pTransport)
{
	if (pTeam->Type->TransportsReturnOnUnload)
	{
		if (!pTransport->GetTechnoType()->HasPrimary)
		{
			auto pScript = pTeam->CurrentScript;
			Debug::Log("DEBUG: [%s] [%s](line: %d = %d,%d): Transport [%s] (UID: %lu) has no weapon and thus cannot return.\n",
				pTeam->Type->ID,
				pScript->Type->ID,
				pScript->CurrentMission,
				pScript->Type->ScriptActions[pScript->CurrentMission].Action,
				pScript->Type->ScriptActions[pScript->CurrentMission].Argument,
				pTransport->GetTechnoType()->get_ID(),
				pTransport->UniqueID);
		}
		pTransport->SetDestination(pTeam->SpawnCell, false);
		pTransport->QueueMission(Mission::Move, false);
	}
	return;
}

void ScriptExt::UnloadFromTransports(TeamClass* pTeam)
{
	if (!pTeam)
		return;

	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	if (!pTeamData)
		return;

	DynamicVectorClass<FootClass*> transports;
	int argument = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;
	double maxSizeLimit = 0;

	if (argument > 3)
	{
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			maxSizeLimit = std::max(maxSizeLimit, pUnit->GetTechnoType()->SizeLimit);
		}
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			if (!pUnit->GetTechnoType()->OpenTopped
				&& !pUnit->GetTechnoType()->Gunner
				&& pUnit->Passengers.NumPassengers > 0
				&& pUnit->GetTechnoType()->SizeLimit == maxSizeLimit) //Battle Fortress and IFV are not transports.
			{
				transports.AddUnique(pUnit);
			}
		}
	}
	else
	{
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			if (!pUnit->GetTechnoType()->OpenTopped && !pUnit->GetTechnoType()->Gunner && pUnit->Passengers.NumPassengers > 0) //Battle Fortress and IFV are not transports.
			{
				transports.AddUnique(pUnit);
			}
		}
	}

	bool stillMoving = false;

	for (int i = 0; i < transports.Count; i++)
	{
		auto pUnit = transports[i];
	
		if (pUnit->CurrentMission == Mission::Move || (pUnit->Locomotor->Is_Moving() && !pUnit->GetTechnoType()->JumpJet))
		{
			pUnit->ForceMission(Mission::Wait);
			pUnit->CurrentTargets.Clear();
			pUnit->SetTarget(nullptr);
			pUnit->SetFocus(nullptr);
			pUnit->SetDestination(nullptr, true);
			stillMoving = true;
		}
	}

	//no valid transports
	if (transports.Count == 0)
	{
		pTeamData->AllPassengers.Clear();
		pTeam->StepCompleted = true;
		return;
	}
	if (stillMoving)
	{
		pTeam->GuardAreaTimer.Start(45);
		return;
	}
	
	DynamicVectorClass<FootClass*> AllPassengers;
		
	if (argument > 3)
	{
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			maxSizeLimit = std::max(maxSizeLimit, pUnit->GetTechnoType()->SizeLimit);
		}
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			if (!pUnit->GetTechnoType()->OpenTopped
				&& !pUnit->GetTechnoType()->Gunner
				&& pUnit->Passengers.NumPassengers > 0
				&& pUnit->GetTechnoType()->SizeLimit == maxSizeLimit) //Battle Fortress and IFV are not transports.
			{
				bool isInAllPassenger = false;
				for (int i = 0; i < pTeamData->AllPassengers.Count; i++)
				{
					if ((pTeamData->AllPassengers)[i] == pUnit)
					{
						isInAllPassenger = true;
					}
				}
				if (!isInAllPassenger)
				{
					AllPassengers.AddItem(pUnit->Passengers.FirstPassenger);
					for (NextObject i(pUnit->Passengers.FirstPassenger->NextObject); i && abstract_cast<FootClass*>(*i); i++)
					{
						auto passenger = static_cast<FootClass*>(*i);
						AllPassengers.AddItem(passenger);
					}
					pUnit->QueueMission(Mission::Unload, true);
				}
			}
		}
	}
	else
	{
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			if (!pUnit->GetTechnoType()->OpenTopped && !pUnit->GetTechnoType()->Gunner && pUnit->Passengers.NumPassengers > 0) //Battle Fortress and IFV are not transports.
			{
				bool isInAllPassenger = false;
				for (int i = 0; i < pTeamData->AllPassengers.Count; i++)
				{
					if ((pTeamData->AllPassengers)[i] == pUnit)
					{
						isInAllPassenger = true;
					}
				}
				if (!isInAllPassenger)
				{
					AllPassengers.AddItem(pUnit->Passengers.FirstPassenger);
					for (NextObject i(pUnit->Passengers.FirstPassenger->NextObject); i && abstract_cast<FootClass*>(*i); i++)
					{
						auto passenger = static_cast<FootClass*>(*i);
						AllPassengers.AddItem(passenger);
					}
					pUnit->QueueMission(Mission::Unload, true);
				}
			}
		}
	}

	if (pTeamData->AllPassengers.Count == 0)
		pTeamData->AllPassengers = AllPassengers;

	for (int i = 0; i < pTeamData->AllPassengers.Count; i++)
	{
		auto pFoot = (pTeamData->AllPassengers)[i];

		// Must be owner
		if (pFoot
			&& !pFoot->InLimbo && pFoot->Health > 0
			&& pFoot->Owner == pTeam->Owner)
		{
			pTeam->AddMember(pFoot, true);
		}
	}

	//unload in progress
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		if (pUnit->GetCurrentMission() == Mission::Unload)
			return;

	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		if (!pUnit->GetTechnoType()->OpenTopped && !pUnit->GetTechnoType()->Gunner && pUnit->GetTechnoType()->Passengers > 0) //Battle Fortress and IFV are not transports.
		{
			transports.AddUnique(pUnit);
		}
	}

	//Save all
	if (argument == 0 || argument == 4)
	{
		for (int i = 0; i < pTeamData->AllPassengers.Count; i++)
		{
			auto pFoot = (pTeamData->AllPassengers)[i];

			// Must be owner
			if (pFoot
				&& !pFoot->InLimbo && pFoot->Health > 0
				&& pFoot->Owner == pTeam->Owner)
			{
				pTeam->AddMember(pFoot, true);
			}
		}
	}
	//Liberate passengers
	else if (argument == 1)
	{
		for (auto pPassengers : pTeamData->AllPassengers)
		{
			pTeam->LiberateMember(pPassengers);
		}
	}
	//Liberate transports
	else if (argument == 2)
	{
		for (auto pTransport : transports)
		{
			TransportsReturn(pTeam, pTransport);
			pTeam->LiberateMember(pTransport);
		}
		for (int i = 0; i < pTeamData->AllPassengers.Count; i++)
		{
			auto pFoot = (pTeamData->AllPassengers)[i];

			// Must be owner
			if (pFoot
				&& !pFoot->InLimbo && pFoot->Health > 0
				&& pFoot->Owner == pTeam->Owner)
			{
				pTeam->AddMember(pFoot, true);
			}
		}
	}
	//Liberate whole team
	else if (argument == 3)
	{
		for (auto pTransport : transports)
		{
			TransportsReturn(pTeam, pTransport);
		}
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			pTeam->LiberateMember(pUnit);
			//this team is over :)
			pTeam->CurrentScript->ClearMission();
		}
	}
	//Liberate passengers and transports that don't have a maxSizeLimit.
	else if (argument == 5)
	{
		for (auto pTransport : transports)
		{
			if (pTransport->GetTechnoType()->SizeLimit != maxSizeLimit)
			{
				pTeam->LiberateMember(pTransport);
			}
		}
		for (auto pPassengers : pTeamData->AllPassengers)
		{
			pTeam->LiberateMember(pPassengers);
		}
	}
	//Liberate transports that have a maxSizeLimit.
	else if (argument == 6)
	{
		for (auto pTransport : transports)
		{
			if (pTransport->GetTechnoType()->SizeLimit == maxSizeLimit)
			{
				TransportsReturn(pTeam, pTransport);
				pTeam->LiberateMember(pTransport);
			}
		}
		for (int i = 0; i < pTeamData->AllPassengers.Count; i++)
		{
			auto pFoot = (pTeamData->AllPassengers)[i];

			// Must be owner
			if (pFoot
				&& !pFoot->InLimbo && pFoot->Health > 0
				&& pFoot->Owner == pTeam->Owner)
			{
				pTeam->AddMember(pFoot, true);
			}
		}
	}
	//Liberate whole team. Only transports with maxSizeLimit will return.
	else if (argument == 7)
	{
		for (auto pTransport : transports)
		{
			if (pTransport->GetTechnoType()->SizeLimit == maxSizeLimit)
			{
				TransportsReturn(pTeam, pTransport);
			}
		}
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			pTeam->LiberateMember(pUnit);
			//this team is over :)
			pTeam->CurrentScript->ClearMission();
		}
	}

	// This action finished
	pTeamData->AllPassengers.Clear();
	pTeam->StepCompleted = true;
	return;
}

void ScriptExt::JumpBackToPreviousScript(TeamClass* pTeam)
{
	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	if (!pTeamData->PreviousScriptList.empty())
	{
		pTeam->CurrentScript = nullptr;
		pTeam->CurrentScript = pTeamData->PreviousScriptList.back();
		pTeamData->PreviousScriptList.pop_back();
		pTeam->StepCompleted = true;
		return;
	}
	else
	{
		auto pScript = pTeam->CurrentScript;
		Debug::Log("DEBUG: [%s] [%s](line: %d = %d,%d): Can't find the previous script! This script action must be used after PickRandomScript.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument);
		pTeam->StepCompleted = true;
		return;
	}
}

void ScriptExt::DistributedLoadOntoTransport2(TeamClass* pTeam, int nArg)
{
	const int T_NO_GATHER = 0, T_COUNTDOWN = 1, T_AVGPOS = 2;
	/*
	type 0: stop member from gathering and begin load now
	type 1: don't stop, maintain previous action, countdown LOWORD seconds then begin load
	type 2: don't stop, gether around first member's position range LOWORD then begin load
	*/

	// can proceed to load calculate logic
	const int R_CAN_PROCEED = 1;
	// type 1 waiting stage, start timer and wait timer stop, when stop, proceed to loading
	const int R_WAITING = 2;
	// type 2 waiting stage, start timer and wait timer stop, when stop, check average position
	const int R_WAIT_POS = 3;

	int nType = HIWORD(nArg);
	int nNum = LOWORD(nArg);
	auto pExt = TeamExt::ExtMap.Find(pTeam);
	auto remainingSize = [](FootClass* src)
	{
		auto type = src->GetTechnoType();
		return type->Passengers - src->Passengers.GetTotalSize();
	};

	auto pFoot = pTeam->FirstUnit;
	auto timer = pFoot->BlockagePathTimer;
	// Wait for timer stop
	if (pTeam->GuardAreaTimer.TimeLeft > 0)
	{
		pTeam->GuardAreaTimer.TimeLeft--;
		return;
	}
	// type 1 times up
	else if (pExt->GenericStatus == R_WAITING)
	{
		bool stillMoving = StopTeamMemberMoving(pTeam);
		if (!stillMoving)
		{
			pExt->GenericStatus = R_CAN_PROCEED;
			goto beginLoad;
		}
	}
	// type 2 times up, check distance
	else if (pExt->GenericStatus == R_WAIT_POS)
	{
		bool canProceed = true;
		auto pCell = pFoot->GetCell();

		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			// no blockage, just keep moving
			if (pUnit->Locomotor->Is_Moving_Now())
			{
				canProceed = false;
				continue;
			}

			if (pUnit->DistanceFrom(pFoot) / 256 <= nNum)
			{
				pUnit->StopMoving();
				pUnit->CurrentTargets.Clear();
				pUnit->SetTarget(nullptr);
				pUnit->SetFocus(nullptr);
				pUnit->SetDestination(nullptr, true);
			}
			else
			{
				CoordStruct coords = pCell->GetCoords();
				pUnit->MoveTo(&coords);
				canProceed = false;
			}
		}
		if (canProceed)
		{
			pExt->GenericStatus = R_CAN_PROCEED;
			return;
		}
		else
		{
			pTeam->GuardAreaTimer.Start(5);
			return;
		}
	}

	// Check if this script can skip this frame
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		// If anyone is entering transport, means this script is now proceeding
		if (pUnit->GetCurrentMission() == Mission::Enter)
			return;
	}

	// Status jump
	if (pExt->GenericStatus == R_CAN_PROCEED)
		goto beginLoad;

	// switch mode
	// 0: stop member from gathering
	if (nType == T_NO_GATHER)
	{
		// If anyone is moving, stop now, and add timer
		// why team member will converge upon team creation
		// fuck you westwood
		pTeam->Focus = nullptr;
		pTeam->QueuedFocus = nullptr;
		bool stillMoving = StopTeamMemberMoving(pTeam);
		if (stillMoving)
		{
			pTeam->GuardAreaTimer.Start(45);
			return;
		}
	}
	// 1: don't stop, maintain previous action
	// default: gather near team center position (upon team creation, it's auto), countdown timer LOWORD seconds
	else if (nType == T_COUNTDOWN)
	{
		if (pExt->GenericStatus == R_WAITING)
			return;
		pTeam->GuardAreaTimer.Start(nNum * 15);
		pExt->GenericStatus = R_WAITING;
		return;
	}
	// 2: don't stop, manually gather around first member's position
	else if (nType == T_AVGPOS)
	{
		auto pCell = pFoot->GetCell();
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			CoordStruct coords = pCell->GetCoords();
			pUnit->MoveTo(&coords);
		}
		pTeam->GuardAreaTimer.Start(5);
		pExt->GenericStatus = R_WAIT_POS;
		return;
	}

beginLoad:
	// Now we're talking
	DynamicVectorClass<FootClass*> transports, passengers;
	std::unordered_map<FootClass*, double> transportSpaces;
	// Find max SizeLimit to determine which type is considered as transport
	double maxSizeLimit = 0;
	double minSizeLimit = INFINITY;
	std::list<double> SizeLimitLists;
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		auto pType = pUnit->GetTechnoType();
		double SizeLimit = pType->SizeLimit;
		maxSizeLimit = std::max(maxSizeLimit, SizeLimit);
		if (SizeLimit > 0)
		{
			minSizeLimit = std::min(minSizeLimit, SizeLimit);
			SizeLimitLists.push_back(SizeLimit);
		}
	}
	SizeLimitLists.unique();
	SizeLimitLists.sort();

	// No transports remaining
	if (maxSizeLimit == 0)
	{
		pTeam->StepCompleted = true;
		pExt->GenericStatus = 0;
		return;
	}
	// range prioritize
	//bool passengerLoading = false;

	std::list<double>::iterator eleSizeLimit;
	for (eleSizeLimit = SizeLimitLists.begin(); eleSizeLimit != SizeLimitLists.end(); ++eleSizeLimit)
	{
		double currentSizeLimit = *eleSizeLimit;
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			auto pType = pUnit->GetTechnoType();
			if (pType->SizeLimit == currentSizeLimit)
			{
				int space = remainingSize(pUnit);
				transports.AddItem(pUnit);
				transportSpaces[pUnit] = space;
			}
			else if (pType->WhatAmI() != AbstractType::AircraftType &&
					!pType->ConsideredAircraft && !pType->JumpJet &&
					TECHNO_IS_ALIVE(pUnit))
				passengers.AddItem(pUnit);
		}
		// If there are no passengers
		// then this script is done
		if (passengers.Count == 0)
		{
			pTeam->StepCompleted = true;
			pExt->GenericStatus = 0;
			return;
		}
		if (transports.Count == 0)
		{
			pTeam->StepCompleted = true;
			pExt->GenericStatus = 0;
			return;
		}
		// If transport is on building, scatter, and discard this frame
		for (auto pUnit : transports)
		{
			if (pUnit->GetCell()->GetBuilding())
			{
				pUnit->Scatter(pUnit->GetCoords(), true, false);
				return;
			}
		}

		// Load logic

		// larger size first
		auto sizeSort = [](FootClass* a, FootClass* b)
		{
			return a->GetTechnoType()->Size > b->GetTechnoType()->Size;
		};
		std::sort(passengers.begin(), passengers.end(), sizeSort);
		for (auto pPassenger : passengers)
		{
			auto pPassengerType = pPassenger->GetTechnoType();
			// Is legal loadable unit ?
			if (pPassengerType->WhatAmI() != AbstractType::AircraftType &&
					!pPassengerType->ConsideredAircraft && !pPassengerType->JumpJet &&
					TECHNO_IS_ALIVE(pPassenger))
			{
				FootClass* targetTransport = nullptr;
				double distance = INFINITY;
				for (auto pTransport : transports)
				{
					auto pTransportType = pTransport->GetTechnoType();
					double currentSpace = transportSpaces[pTransport];
					// Can unit load onto this car ?
					if (currentSpace > 0 &&
						pPassengerType->Size > 0 &&
						pPassengerType->Size <= pTransportType->SizeLimit &&
						pPassengerType->Size <= currentSpace)
					{
						double d = pPassenger->DistanceFrom(pTransport);
						if (d < distance)
						{
							targetTransport = pTransport;
							distance = d;
						}
					}
				}
				// This is nearest available transport
				if (targetTransport)
				{
					// Get on the car
					if (pPassenger->GetCurrentMission() != Mission::Enter)
					{
						pPassenger->QueueMission(Mission::Enter, true);
						pPassenger->SetTarget(nullptr);
						pPassenger->SetDestination(targetTransport, false);
						transportSpaces[targetTransport] -= pPassengerType->Size;
						//passengerLoading = true;
					}
				}
			}
		}
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
			if (pUnit->GetCurrentMission() == Mission::Enter || pUnit->GetCurrentMission() == Mission::Move)
				return;
	}


	// This action finished
	if (pTeam->CurrentScript->HasNextMission())
		++pTeam->CurrentScript->CurrentMission;

	pTeam->StepCompleted = true;
	// If no one is loading, this script is done
	//if (!passengerLoading)
	//{
	//	pTeam->StepCompleted = true;
	//	pExt->GenericStatus = 0;
	//}
	// All member share this SizeLimit will consider as transport

	// Load logic
	// speed prioritize
	//for (auto pTransport : transports)
	//{
	//	DynamicVectorClass<FootClass*> loadedUnits;
	//	auto pTransportType = pTransport->GetTechnoType();
	//	double currentSpace = pTransportType->Passengers - pTransport->Passengers.GetTotalSize();
	//	if (currentSpace == 0) continue;
	//	for (auto pUnit : passengers)
	//	{
	//		auto pUnitType = pUnit->GetTechnoType();
	//		// Is legal loadable unit ?
	//		if (pUnitType->WhatAmI() != AbstractType::AircraftType &&
	//			!pUnit->InLimbo &&
	//			!pUnitType->ConsideredAircraft &&
	//			pUnit->Health > 0 &&
	//			pUnit != pTransport)
	//		{
	//			// Can unit load onto this car ?
	//			if (pUnitType->Size > 0
	//				&& pUnitType->Size <= pTransportType->SizeLimit
	//				&& pUnitType->Size <= currentSpace)
	//			{
	//				// Get on the car
	//				if (pUnit->GetCurrentMission() != Mission::Enter)
	//				{
	//					transportsNoSpace = false;
	//					pUnit->QueueMission(Mission::Enter, true);
	//					pUnit->SetTarget(nullptr);
	//					pUnit->SetDestination(pTransport, false);
	//					currentSpace -= pUnitType->Size;
	//					loadedUnits.AddItem(pUnit);
	//				}
	//			}
	//		}
	//		if (currentSpace == 0) break;
	//	}
	//	for (auto pRemove : loadedUnits) passengers.Remove(pRemove);
	//	loadedUnits.Clear();
	//}
	//if (transportsNoSpace)
	//{
	//	pTeam->StepCompleted = true;
	//	return;
	//}
}


void ScriptExt::Mission_Attack_Individually(TeamClass* pTeam, int numberPerTarget, bool repeatAction = true, int calcThreatMode = 0, int attackAITargetType = -1, int idxAITargetTypeItem = -1)
{
	auto pScript = pTeam->CurrentScript;
	int scriptArgument = pScript->Type->ScriptActions[pScript->CurrentMission].Argument; // This is the target type
	TechnoClass* selectedTarget = nullptr;
	HouseClass* enemyHouse = nullptr;
	bool noWaitLoop = false;
	FootClass* pLeaderUnit = nullptr;
	TechnoTypeClass* pLeaderUnitType = nullptr;
	bool bAircraftsWithoutAmmo = false;
	TechnoClass* pFocus = nullptr;
	bool agentMode = false;
	bool pacifistTeam = true;
	auto pTeamData = TeamExt::ExtMap.Find(pTeam);

	if (!pScript)
		return;

	if (!pTeamData)
	{
		pTeam->StepCompleted = true;
		Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Jump to next line: %d = %d,%d -> (Reason: ExtData found)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pScript->CurrentMission + 1, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Action, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Argument);

		return;
	}

	// When the new target wasn't found it sleeps some few frames before the new attempt. This can save cycles and cycles of unnecessary executed lines.
	if (pTeamData->WaitNoTargetCounter > 0)
	{
		if (pTeamData->WaitNoTargetTimer.InProgress())
			return;

		pTeamData->WaitNoTargetTimer.Stop();
		noWaitLoop = true;
		pTeamData->WaitNoTargetCounter = 0;

		if (pTeamData->WaitNoTargetAttempts > 0)
			pTeamData->WaitNoTargetAttempts--;
	}

	// This team has no units!
	if (!pTeam)
	{
		if (pTeamData->CloseEnough > 0)
			pTeamData->CloseEnough = -1;

		// This action finished
		pTeam->StepCompleted = true;
		Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Jump to next line: %d = %d,%d -> (Reason: No team members alive)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pScript->CurrentMission + 1, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Action, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Argument);

		return;
	}

	pFocus = abstract_cast<TechnoClass*>(pTeam->Focus);
	if (!pFocus
		|| !pFocus->IsAlive
		|| pFocus->Health <= 0
		|| pFocus->InLimbo
		|| !pFocus->IsOnMap
		|| pFocus->Absorbed)
	{
		pTeam->Focus = nullptr;
		pFocus = nullptr;
	}

	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		auto pKillerTechnoData = TechnoExt::ExtMap.Find(pUnit);
		if (pKillerTechnoData && pKillerTechnoData->LastKillWasTeamTarget)
		{
			// Time for Team award check! (if set any)
			if (pTeamData->NextSuccessWeightAward > 0)
			{
				IncreaseCurrentTriggerWeight(pTeam, false, pTeamData->NextSuccessWeightAward);
				pTeamData->NextSuccessWeightAward = 0;
			}

			// Let's clean the Killer mess
			pKillerTechnoData->LastKillWasTeamTarget = false;
			pFocus = nullptr;
			pTeam->Focus = nullptr;

			if (!repeatAction)
			{
				// If the previous Team's Target was killed by this Team Member and the script was a 1-time-use then this script action must be finished.
				for (auto pTeamUnit = pTeam->FirstUnit; pTeamUnit; pTeamUnit = pTeamUnit->NextTeamMember)
				{
					// Let's reset all Team Members objective
					auto pKillerTeamUnitData = TechnoExt::ExtMap.Find(pTeamUnit);
					pKillerTeamUnitData->LastKillWasTeamTarget = false;

					if (pTeamUnit->GetTechnoType()->WhatAmI() == AbstractType::AircraftType)
					{
						pTeamUnit->SetTarget(nullptr);
						pTeamUnit->LastTarget = nullptr;
						pTeamUnit->SetFocus(nullptr); // Lets see if this works or bug my little aircrafts
						pTeamUnit->CurrentTargets.Clear(); // Lets see if this works or bug my little aircrafts
						pTeamUnit->QueueMission(Mission::Guard, true);
					}
				}

				pTeamData->IdxSelectedObjectFromAIList = -1;

				// This action finished
				pTeam->StepCompleted = true;
				Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Force the jump to next line: %d = %d,%d (This action wont repeat)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pScript->CurrentMission + 1, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Action, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Argument);

				return;
			}
		}
	}

	int teamNumber = 0;
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		if (pUnit && pUnit->IsAlive && !pUnit->InLimbo)
		{
			teamNumber++;
			auto pUnitType = pUnit->GetTechnoType();
			if (pUnitType)
			{
				if (pUnitType->WhatAmI() == AbstractType::AircraftType
					&& !pUnit->IsInAir()
					&& abstract_cast<AircraftTypeClass*>(pUnitType)->AirportBound
					&& pUnit->Ammo < pUnitType->Ammo)
				{
					bAircraftsWithoutAmmo = true;
					pUnit->CurrentTargets.Clear();
				}

				bool pacifistUnit = true;
				if (pUnit->Veterancy.IsElite())
				{
					if (pUnitType->EliteWeapon[0].WeaponType || pUnitType->EliteWeapon[1].WeaponType
						|| (pUnitType->IsGattling && pUnitType->EliteWeapon[pUnit->CurrentWeaponNumber].WeaponType))
					{
						pacifistTeam = false;
						pacifistUnit = false;
					}
				}
				else
				{
					if (pUnitType->Weapon[0].WeaponType || pUnitType->Weapon[1].WeaponType
						|| (pUnitType->IsGattling && pUnitType->Weapon[pUnit->CurrentWeaponNumber].WeaponType))
					{
						pacifistTeam = false;
						pacifistUnit = false;
					}
				}

				// Any Team member (infantry) is a special agent? If yes ignore some checks based on Weapons.
				if (pUnitType->WhatAmI() == AbstractType::InfantryType)
				{
					auto pTypeInf = abstract_cast<InfantryTypeClass*>(pUnitType);
					if ((pTypeInf->Agent && pTypeInf->Infiltrate) || pTypeInf->Engineer)
					{
						agentMode = true;
					}
				}
			}
		}
	}

	pLeaderUnit = pTeamData->TeamLeader;
	if (!pLeaderUnit
		|| !pLeaderUnit->IsAlive
		|| pLeaderUnit->Health <= 0
		|| pLeaderUnit->InLimbo
		|| !pLeaderUnit->IsOnMap
		|| pLeaderUnit->Absorbed)
	{
		pLeaderUnit = FindTheTeamLeader(pTeam);
		pTeamData->TeamLeader = pLeaderUnit;
	}

	if (!pLeaderUnit || bAircraftsWithoutAmmo || (pacifistTeam && !agentMode))
	{
		pTeamData->IdxSelectedObjectFromAIList = -1;
		if (pTeamData->WaitNoTargetAttempts != 0)
		{
			pTeamData->WaitNoTargetTimer.Stop();
			pTeamData->WaitNoTargetCounter = 0;
			pTeamData->WaitNoTargetAttempts = 0;
		}

		// This action finished
		pTeam->StepCompleted = true;
		Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Jump to next line: %d = %d,%d -> (Reason: No Leader found | Exists Aircrafts without ammo | Team members have no weapons)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pScript->CurrentMission + 1, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Action, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Argument);

		return;
	}

	pLeaderUnitType = pLeaderUnit->GetTechnoType();
	bool leaderWeaponsHaveAA = false;
	bool leaderWeaponsHaveAG = false;
	// Note: Replace these lines when I have access to Combat_Damage() method in YRpp if that is better
	WeaponTypeClass* WeaponType1 = pLeaderUnit->Veterancy.IsElite() ?
		pLeaderUnitType->EliteWeapon[0].WeaponType :
		pLeaderUnitType->Weapon[0].WeaponType;

	WeaponTypeClass* WeaponType2 = pLeaderUnit->Veterancy.IsElite() ?
		pLeaderUnitType->EliteWeapon[1].WeaponType :
		pLeaderUnitType->Weapon[1].WeaponType;

	WeaponTypeClass* WeaponType3 = WeaponType1;

	if (pLeaderUnitType->IsGattling)
	{
		WeaponType3 = pLeaderUnit->Veterancy.IsElite() ?
			pLeaderUnitType->EliteWeapon[pLeaderUnit->CurrentWeaponNumber].WeaponType :
			pLeaderUnitType->Weapon[pLeaderUnit->CurrentWeaponNumber].WeaponType;

		WeaponType1 = WeaponType3;
	}

	// Weapon check used for filtering Leader targets.
	// Note: the Team Leader is picked for this task, be careful with leadership rating values in your mod
	if ((WeaponType1 && WeaponType1->Projectile->AA) || (WeaponType2 && WeaponType2->Projectile->AA))
		leaderWeaponsHaveAA = true;

	if ((WeaponType1 && WeaponType1->Projectile->AG) || (WeaponType2 && WeaponType2->Projectile->AG) || agentMode)
		leaderWeaponsHaveAG = true;

	// Special case: a Leader with OpenTopped tag
	if (pLeaderUnitType->OpenTopped && pLeaderUnit->Passengers.NumPassengers > 0)
	{
		for (NextObject j(pLeaderUnit->Passengers.FirstPassenger->NextObject); j && abstract_cast<FootClass*>(*j); ++j)
		{
			auto passenger = static_cast<FootClass*>(*j);
			auto pPassengerType = passenger->GetTechnoType();

			if (pPassengerType)
			{
				// Note: Replace these lines when I have access to Combat_Damage() method in YRpp if that is better
				WeaponTypeClass* passengerWeaponType1 = passenger->Veterancy.IsElite() ?
					pPassengerType->EliteWeapon[0].WeaponType :
					pPassengerType->Weapon[0].WeaponType;

				WeaponTypeClass* passengerWeaponType2 = passenger->Veterancy.IsElite() ?
					pPassengerType->EliteWeapon[1].WeaponType :
					pPassengerType->Weapon[1].WeaponType;

				if (pPassengerType->IsGattling)
				{
					WeaponTypeClass* passengerWeaponType3 = passenger->Veterancy.IsElite() ?
						pPassengerType->EliteWeapon[passenger->CurrentWeaponNumber].WeaponType :
						pPassengerType->Weapon[passenger->CurrentWeaponNumber].WeaponType;

					passengerWeaponType1 = passengerWeaponType3;
				}

				// Used for filtering targets.
				// Note: the units inside a openTopped Leader are used for this task
				if ((passengerWeaponType1 && passengerWeaponType1->Projectile->AA)
					|| (passengerWeaponType2 && passengerWeaponType2->Projectile->AA))
				{
					leaderWeaponsHaveAA = true;
				}

				if ((passengerWeaponType1 && passengerWeaponType1->Projectile->AG)
					|| (passengerWeaponType2 && passengerWeaponType2->Projectile->AG))
				{
					leaderWeaponsHaveAG = true;
				}
			}
		}
	}
	int targetMask = scriptArgument;

	if (pTeam->Type->OnlyTargetHouseEnemy && pLeaderUnit->Owner->EnemyHouseIndex >= 0)
		enemyHouse = HouseClass::Array->GetItem(pLeaderUnit->Owner->EnemyHouseIndex);

	//for (int i = 0; i < pTeamData->IndividualTargets.Count; i++)
	//{
	//	auto pTarget = pTeamData->IndividualTargets[i];
	//	if (!pLeaderUnit
	//		|| !pLeaderUnit->IsAlive
	//		|| pLeaderUnit->Health <= 0
	//		|| pLeaderUnit->InLimbo
	//		|| !pLeaderUnit->IsOnMap
	//		|| pLeaderUnit->Absorbed)
	//		pTeamData->IndividualTargets.RemoveItem(i);
	//}
	//if (pTeamData->IndividualTargets.Count < teamNumber / numberPerTarget + 1)
	//{
	//	pTeamData->IndividualTargets.Clear();
	//	MultiGreatestThreat(pLeaderUnit, pTeam, teamNumber / numberPerTarget + 2, targetMask, calcThreatMode, enemyHouse, attackAITargetType, idxAITargetTypeItem, agentMode, true);
	//}

	//if (pTeamData->IndividualTargets.Count == 0)
	//{
	//	pTeamData->IdxSelectedObjectFromAIList = -1;
	//	pTeamData->IndividualTargets.Clear();
	//	pTeam->StepCompleted = true;
	//	Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Jump to NEXT line: %d = %d,%d (No target found)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pScript->CurrentMission + 1, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Action, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Argument);
	//	return;
	//}

	
	int count = -1;
	int targetIndex = -1;
	TechnoClass* pEachTarget = nullptr;

	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		bool isnotLast = true;
		while (isnotLast)
		{
			for (int i = pTeamData->IndividualTargets.Count - 1; i >= 0; i--)
			{
				if (i <= 0)
					isnotLast = false;
				auto pTarget = pTeamData->IndividualTargets[i];
				if (!pTarget
					|| !pTarget->IsAlive
					|| pTarget->Health <= 0
					|| pTarget->InLimbo
					|| !pTarget->IsOnMap
					|| pTarget->Absorbed)
				{
					pTeamData->IndividualTargets.RemoveItem(i);
					break;
				}
			}
			if (pTeamData->IndividualTargets.Count == 0)
				isnotLast = false;
		}
		

		count++;

		bool isSmallLeader = false;
		if (count % numberPerTarget == 0)
			isSmallLeader = true;

		if (isSmallLeader)
			targetIndex++;

		// Find the Leader
		pLeaderUnit = pUnit;
		if (!pLeaderUnit
			|| !pLeaderUnit->IsAlive
			|| pLeaderUnit->Health <= 0
			|| pLeaderUnit->InLimbo
			|| !pLeaderUnit->IsOnMap
			|| pLeaderUnit->Absorbed)
		{
			pLeaderUnit = FindTheTeamLeader(pTeam);
			pTeamData->TeamLeader = pLeaderUnit;
		}

		pFocus = abstract_cast<TechnoClass*>(pUnit->Target);
		//if (pFocus)
		//{
		//	Debug::Log("    DEBUG: [%s] [%s] (line: %d = %d,%d) Team member [%s] (UID: %lu) 's target is [%s] (UID: %lu) \n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pLeaderUnit->GetTechnoType()->get_ID(), pLeaderUnit->UniqueID, pFocus->GetTechnoType()->get_ID(), pFocus->UniqueID);

		//}
		//else
		//{
		//	Debug::Log("    DEBUG: [%s] [%s] (line: %d = %d,%d) Team member [%s] (UID: %lu) has no target \n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pLeaderUnit->GetTechnoType()->get_ID(), pLeaderUnit->UniqueID);

		//}

		if (!pFocus && !bAircraftsWithoutAmmo)
		{
			// This part of the code is used for picking a new target.

			// Favorite Enemy House case. If set, AI will focus against that House

			if (pTeam->Type->OnlyTargetHouseEnemy && pLeaderUnit->Owner->EnemyHouseIndex >= 0)
				enemyHouse = HouseClass::Array->GetItem(pLeaderUnit->Owner->EnemyHouseIndex);


			//if (pTeamData->IndividualTargets.Count > targetIndex)
			//	selectedTarget = pTeamData->IndividualTargets[targetIndex];//GreatestThreat(pLeaderUnit, pTeam, targetMask, calcThreatMode, enemyHouse, attackAITargetType, idxAITargetTypeItem, agentMode, false);
			//else if (pTeamData->IndividualTargets.Count > 0)
			//{
			//	targetIndex = 0;
			//	selectedTarget = pTeamData->IndividualTargets[targetIndex];

			//}

			bool logged = false;
			if (isSmallLeader)
			{
				selectedTarget = GreatestThreat(pLeaderUnit, pTeam, targetMask, calcThreatMode, enemyHouse, attackAITargetType, idxAITargetTypeItem, agentMode, true);
				if (!selectedTarget)
				{
					selectedTarget = GreatestThreat(pLeaderUnit, pTeam, targetMask, calcThreatMode, enemyHouse, attackAITargetType, idxAITargetTypeItem, agentMode, false); // if targets is not enough, forbid individual attack
					if (selectedTarget)
					{
						Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Team member [%s] (UID: %lu) selected [%s] (UID: %lu) as target. Individually attacking is ignored because there are not enough targets.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pLeaderUnit->GetTechnoType()->get_ID(), pLeaderUnit->UniqueID, selectedTarget->GetTechnoType()->get_ID(), selectedTarget->UniqueID);
						logged = true;
					}

				}
				if (selectedTarget)
				{
					pTeamData->IndividualTargets.AddUnique(selectedTarget);
					pEachTarget = selectedTarget;
				}				
			}
			else
			{
				selectedTarget = pEachTarget;
			}

			if (selectedTarget && selectedTarget->IsAlive && !selectedTarget->InLimbo)
			{
				if (!logged)
					Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Team member [%s] (UID: %lu) selected [%s] (UID: %lu) as target.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pLeaderUnit->GetTechnoType()->get_ID(), pLeaderUnit->UniqueID, selectedTarget->GetTechnoType()->get_ID(), selectedTarget->UniqueID);

				pTeam->Focus = selectedTarget;
				pTeamData->WaitNoTargetAttempts = 0; // Disable Script Waits if there are any because a new target was selected
				pTeamData->WaitNoTargetTimer.Stop();
				pTeamData->WaitNoTargetCounter = 0; // Disable Script Waits if there are any because a new target was selected

				if (pUnit->IsAlive && !pUnit->InLimbo)
				{
					auto pUnitType = pUnit->GetTechnoType();
					if (pUnitType && pUnit != selectedTarget && pUnit->Target != selectedTarget)
					{
						pUnit->CurrentTargets.Clear();
						if (pUnitType->Underwater && pUnitType->LandTargeting == LandTargetingType::Land_Not_OK
							&& selectedTarget->GetCell()->LandType != LandType::Water) // Land not OK for the Naval unit
						{
							// Naval units like Submarines are unable to target ground targets
							// except if they have anti-ground weapons. Ignore the attack
							pUnit->CurrentTargets.Clear();
							pUnit->SetTarget(nullptr);
							pUnit->SetFocus(nullptr);
							pUnit->SetDestination(nullptr, false);
							pUnit->QueueMission(Mission::Area_Guard, true);

							continue;
						}

						// Aircraft hack. I hate how this game auto-manages the aircraft missions.
						if (pUnitType->WhatAmI() == AbstractType::AircraftType
							&& pUnit->Ammo > 0 && pUnit->GetHeight() <= 0)
						{
							pUnit->SetDestination(selectedTarget, false);
							pUnit->QueueMission(Mission::Attack, true);
						}

						pUnit->SetTarget(selectedTarget);

						if (pUnit->IsEngineer())
						{
							pUnit->QueueMission(Mission::Capture, true);
						}
						else
						{
							// Aircraft hack. I hate how this game auto-manages the aircraft missions.
							if (pUnitType->WhatAmI() != AbstractType::AircraftType)
							{
								pUnit->QueueMission(Mission::Attack, true);
								pUnit->ObjectClickedAction(Action::Attack, selectedTarget, false);

								if (pUnit->GetCurrentMission() != Mission::Attack)
									pUnit->Mission_Attack();

								if (pUnit->GetCurrentMission() == Mission::Move && pUnitType->JumpJet)
									pUnit->Mission_Attack();
							}
						}

						// Spy case
						if (pUnitType->WhatAmI() == AbstractType::InfantryType)
						{
							auto pInfantryType = abstract_cast<InfantryTypeClass*>(pUnitType);

							if (pInfantryType && pInfantryType->Infiltrate && pInfantryType->Agent)
							{
								// Check if target is an structure and see if spiable
								if (pUnit->GetCurrentMission() != Mission::Enter)
									pUnit->Mission_Enter();
							}
						}

						// Tanya / Commando C4 case
						if ((pUnitType->WhatAmI() == AbstractType::InfantryType && (abstract_cast<InfantryTypeClass*>(pUnitType)->C4 || pUnit->HasAbility(Ability::C4))) && pUnit->GetCurrentMission() != Mission::Sabotage)
						{
							pUnit->Mission_Attack();
							pUnit->QueueMission(Mission::Sabotage, true);
						}

						pUnit->QueueMission(Mission::Attack, true);
						//pUnit->ObjectClickedAction(Action::Attack, selectedTarget, false);
						//pUnit->Mission_Attack();
					}
					else
					{
						pUnit->QueueMission(Mission::Attack, true);
						pUnit->ObjectClickedAction(Action::Attack, selectedTarget, false);
						pUnit->Mission_Attack();
					}
				}
			}
			else
			{
				// No target was found with the specific criteria.
				if (!noWaitLoop)
				{
					pTeamData->WaitNoTargetCounter = 30;
					pTeamData->WaitNoTargetTimer.Start(30);
				}

				if (pTeamData->IdxSelectedObjectFromAIList >= 0)
					pTeamData->IdxSelectedObjectFromAIList = -1;

				if (pTeamData->WaitNoTargetAttempts != 0)
				{
					// No target? let's wait some frames
					pTeamData->WaitNoTargetCounter = 30;
					pTeamData->WaitNoTargetTimer.Start(30);

					return;
				}

				//pUnit->CurrentTargets.Clear();
				//pUnit->SetTarget(nullptr);
				//pUnit->SetFocus(nullptr);
				//pUnit->SetDestination(nullptr, false);
				//pUnit->QueueMission(Mission::Hunt, true);
				//Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Team Number [%s] (UID: %lu) start to hunt (NO targets).\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pLeaderUnit->GetTechnoType()->get_ID(), pLeaderUnit->UniqueID);
				pTeamData->IdxSelectedObjectFromAIList = -1;
				pTeamData->IndividualTargets.Clear();
				pTeam->StepCompleted = true;
				Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Jump to NEXT line: %d = %d,%d (No target found)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pScript->CurrentMission + 1, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Action, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Argument);
				return;
			}
		}
		else
		{
			// This part of the code is used for updating the "Attack" mission in each team unit

			if (pFocus
				&& pFocus->IsAlive
				&& pFocus->Health > 0
				&& !pFocus->InLimbo
				&& !pFocus->GetTechnoType()->Immune
				&& ((pFocus->IsInAir() && leaderWeaponsHaveAA)
					|| (!pFocus->IsInAir() && leaderWeaponsHaveAG))
				&& pFocus->IsOnMap
				&& !pFocus->Absorbed
				&& pFocus->Owner != pLeaderUnit->Owner
				&& (!pLeaderUnit->Owner->IsAlliedWith(pFocus)
					|| (pLeaderUnit->Owner->IsAlliedWith(pFocus)
						&& pFocus->IsMindControlled()
						&& !pLeaderUnit->Owner->IsAlliedWith(pFocus->MindControlledBy))))
			{


				auto pUnitType = pUnit->GetTechnoType();

				if (pUnitType
					&& pUnit->IsAlive
					&& pUnit->Health > 0
					&& !pUnit->InLimbo)
				{
					// Aircraft case 1
					if ((pUnitType->WhatAmI() == AbstractType::AircraftType
						&& abstract_cast<AircraftTypeClass*>(pUnitType)->AirportBound)
						&& pUnit->Ammo > 0
						&& (pUnit->Target != pFocus && !pUnit->InAir))
					{
						pUnit->SetTarget(pFocus);
						continue;
					}

					// Naval units like Submarines are unable to target ground targets except if they have nti-ground weapons. Ignore the attack
					if (pUnitType->Underwater
						&& pUnitType->LandTargeting == LandTargetingType::Land_Not_OK
						&& pFocus->GetCell()->LandType != LandType::Water) // Land not OK for the Naval unit
					{
						pUnit->CurrentTargets.Clear();
						pUnit->SetTarget(nullptr);
						pUnit->SetFocus(nullptr);
						pUnit->SetDestination(nullptr, false);
						pUnit->QueueMission(Mission::Hunt, true);

						continue;
					}

					// Aircraft case 2
					if (pUnitType->WhatAmI() == AbstractType::AircraftType
						&& pUnit->GetCurrentMission() != Mission::Attack
						&& pUnit->GetCurrentMission() != Mission::Enter)
					{
						if (pUnit->InAir)
						{
							if (pUnit->Ammo > 0)
							{
								pUnit->QueueMission(Mission::Attack, true);

								if (pFocus)
									pUnit->ObjectClickedAction(Action::Attack, pFocus, false);

								pUnit->Mission_Attack();
							}
							else
							{
								pUnit->ForceMission(Mission::Enter);
								pUnit->Mission_Enter();
								pUnit->SetFocus(pUnit);
								pUnit->LastTarget = nullptr;
								pUnit->SetTarget(pUnit);
							}
						}
						else
						{
							if (pUnit->Ammo > 0)
							{
								pUnit->QueueMission(Mission::Attack, true);

								if (pFocus)
									pUnit->ObjectClickedAction(Action::Attack, pFocus, false);

								pUnit->Mission_Attack();
							}
							else
							{
								pUnit->ForceMission(Mission::Enter);
								pUnit->Mission_Enter();
								pUnit->SetFocus(pUnit);
								pUnit->LastTarget = nullptr;
								pUnit->SetTarget(pUnit);
							}
						}

						continue;
					}

					// Tanya / Commando C4 case
					if ((pUnitType->WhatAmI() == AbstractType::InfantryType
						&& abstract_cast<InfantryTypeClass*>(pUnitType)->C4
						|| pUnit->HasAbility(Ability::C4)) && pUnit->GetCurrentMission() != Mission::Sabotage)
					{
						pUnit->Mission_Attack();
						pUnit->QueueMission(Mission::Sabotage, true);

						continue;
					}

					// Other cases
					if (pUnitType->WhatAmI() != AbstractType::AircraftType)
					{
						if (pUnit->Target != pFocus)
							pUnit->SetTarget(pFocus);

						if (pUnit->GetCurrentMission() != Mission::Attack
							&& pUnit->GetCurrentMission() != Mission::Unload
							&& pUnit->GetCurrentMission() != Mission::Selling)
						{
							pUnit->QueueMission(Mission::Attack, false);
						}

						continue;
					}
				}
			}
			else
			{
				pTeam->Focus = nullptr;
			}
		}
	}


	//reduce lagging
	pTeamData->WaitNoTargetCounter = 30;
	pTeamData->WaitNoTargetTimer.Start(30);

	bool hasTarget = false;
	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		if (pUnit->Target)
			hasTarget = true;
	}
	if (!hasTarget)
	{
		pTeamData->IdxSelectedObjectFromAIList = -1;
		pTeamData->IndividualTargets.Clear();
		pTeam->StepCompleted = true;
		Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Jump to NEXT line: %d = %d,%d (No target found)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pScript->CurrentMission + 1, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Action, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Argument);
		return;
	}
}

void ScriptExt::Mission_Attack_List_Individually(TeamClass* pTeam, int numberPerTarget, bool repeatAction, int calcThreatMode, int attackAITargetType)
{
	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	if (pTeamData)
		pTeamData->IdxSelectedObjectFromAIList = -1;

	if (attackAITargetType < 0)
		attackAITargetType = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	if (RulesExt::Global()->AITargetTypesLists.size() > 0
		&& RulesExt::Global()->AITargetTypesLists[attackAITargetType].size() > 0)
	{
		ScriptExt::Mission_Attack_Individually(pTeam, numberPerTarget, repeatAction, calcThreatMode, attackAITargetType, -1);
	}
}




void ScriptExt::SetAttackTargetRank(TeamClass* pTeam)
{
	int argument = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;
	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	if (pTeamData)
		pTeamData->AttackTargetRank = argument;
	//This action finished
	pTeam->StepCompleted = true;
	return;
}

void ScriptExt::StopIfHumanOrAI(TeamClass* pTeam)
{
	int argument = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;

	if (argument == 0 && pTeam->FirstUnit->Owner->IsControlledByHuman())
		pTeam->CurrentScript->ClearMission();
	else if (argument == 1 && !pTeam->FirstUnit->Owner->IsControlledByHuman())
		pTeam->CurrentScript->ClearMission();
	else
		pTeam->StepCompleted = true;
	return;
}

void ScriptExt::CaptureOccupiableBuildings(TeamClass* pTeam)
{
	auto pScript = pTeam->CurrentScript;
	//int scriptArgument = pScript->Type->ScriptActions[pScript->CurrentMission].Argument; 
	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	TechnoClass* pCaptureTarget = nullptr;
	TechnoClass* selectedTarget = nullptr;
	FootClass* pLeaderUnit = nullptr;
	HouseClass* enemyHouse = nullptr;

	if (!pScript)
		return;

	if (!pTeamData)
	{
		pTeam->StepCompleted = true;
		Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Jump to next line: %d = %d,%d -> (Reason: ExtData found)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pScript->CurrentMission + 1, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Action, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Argument);

		return;
	}

	pLeaderUnit = pTeamData->TeamLeader;
	if (!pLeaderUnit
		|| !pLeaderUnit->IsAlive
		|| pLeaderUnit->Health <= 0
		|| pLeaderUnit->InLimbo
		|| !pLeaderUnit->IsOnMap
		|| pLeaderUnit->Absorbed)
	{
		pLeaderUnit = FindTheTeamLeader(pTeam);
		pTeamData->TeamLeader = pLeaderUnit;
	}

	auto target = pTeamData->CaptureTarget;
	TechnoTypeClass* targetType = nullptr;
	if (target)
		targetType = target->GetTechnoType();
	if (target
		&& target->IsAlive
		&& target->Health > 0
		&& !target->InLimbo
		&& target->IsOnMap
		&& !target->Absorbed

		&& targetType->WhatAmI() == AbstractType::BuildingType
		&& abstract_cast<BuildingClass*>(target)->Occupants.Count < abstract_cast<BuildingTypeClass*>(targetType)->MaxNumberOccupants
		&& (target->Owner->IsNeutral() || target->Owner == pTeam->Owner)
		&& ((targetType->TechLevel == -1 && target->GetHealthStatus() != HealthState::Red) || targetType->TechLevel > 0))
	{
		pCaptureTarget = abstract_cast<BuildingClass*>(pTeamData->CaptureTarget);
	}
	else
	{
		pTeamData->CaptureTarget = nullptr;
		pCaptureTarget = nullptr;
	}

	if (pCaptureTarget)
	{
		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
		{
			auto pUnitType = pUnit->GetTechnoType();

			if (pUnitType
				&& pUnit->IsAlive
				&& pUnit->Health > 0
				&& !pUnit->InLimbo)
			{
				pUnit->QueueMission(Mission::Capture, false);
			}
		}
	}
	else
	{
		selectedTarget = GreatestThreat(pLeaderUnit, pTeam, 39, 2, enemyHouse, -1, -1, false, false);

		if (selectedTarget)
		{
			Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Leader [%s] (UID: %lu) selected [%s] (UID: %lu) to capture.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pLeaderUnit->GetTechnoType()->get_ID(), pLeaderUnit->UniqueID, selectedTarget->GetTechnoType()->get_ID(), selectedTarget->UniqueID);
			pTeamData->CaptureTarget = selectedTarget;

			for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
			{
				pUnit->SetTarget(selectedTarget);
				pUnit->SetFocus(selectedTarget);
				pUnit->SetDestination(selectedTarget, false);
				pUnit->QueueMission(Mission::Capture, true);
			}
		}
		else
		{
			// This action finished
			pTeamData->CaptureTarget = nullptr;
			pTeam->StepCompleted = true;
			Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Jump to next line: %d = %d,%d (Leader [%s] (UID: %lu) can't find a new target)\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pScript->CurrentMission + 1, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Action, pScript->Type->ScriptActions[pScript->CurrentMission + 1].Argument, pLeaderUnit->GetTechnoType()->get_ID(), pLeaderUnit->UniqueID);

			return;
		}
	}
}

void ScriptExt::MindControlledUnitsGoToGrinder(TeamClass* pTeam)
{
	auto pScript = pTeam->CurrentScript;
	int scriptArgument = pScript->Type->ScriptActions[pScript->CurrentMission].Argument;
	TechnoClass* selectedTarget = nullptr;
	HouseClass* enemyHouse = nullptr;

	TechnoClass* SpyselectedTarget = nullptr;
	TechnoClass* LCRFselectedTarget = nullptr;

	for (int i = 0; i < TechnoClass::Array->Count; i++)
	{
		auto pUnit = TechnoClass::Array->GetItem(i);
		auto pUnitType = pUnit->GetTechnoType();
		bool isSpy = false;
		bool isLandingCraft = false;

		if (!pUnit || !pUnitType || !pUnit->IsAlive || pUnit->InLimbo)
			continue;

		if (pUnit->Owner != pTeam->Owner)
			continue;

		if (!pUnit->IsMindControlled())
			continue;

		if (pUnit->BelongsToATeam())
			continue;

		if (pUnitType->WhatAmI() == AbstractType::InfantryType)
		{
			auto pInfantryType = abstract_cast<InfantryTypeClass*>(pUnitType);

			if (pInfantryType && pInfantryType->Infiltrate && pInfantryType->Agent)
			{
				isSpy = true;
			}
		}

		if (pUnitType->WhatAmI() == AbstractType::UnitType)
		{
			auto pMUnitType = abstract_cast<UnitTypeClass*>(pUnitType);

			if (pMUnitType && pMUnitType->Category == Category::Transport && pMUnitType->StupidHunt && pMUnitType->Passengers > 0)
			{
				isLandingCraft = true;
			}
		}

		if (scriptArgument == 0)
		{
			if (!selectedTarget)
			{
				selectedTarget = GreatestThreat(pUnit, pTeam, 40, 2, enemyHouse, -1, -1, false, false);
			}
			if (selectedTarget)
			{
				pUnit->SetTarget(selectedTarget);
				pUnit->SetFocus(selectedTarget);
				pUnit->SetDestination(selectedTarget, false);
				pUnit->QueueMission(Mission::Eaten, true);
				Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Mind Controlled Unit [%s] (UID: %lu) is sent to Grinder.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pUnit->GetTechnoType()->get_ID(), pUnit->UniqueID);
			}
			continue;
		}

		if (scriptArgument == 1)
		{
			if (isSpy && pUnit->GetCurrentMission() == Mission::Enter)
				continue;

			if (isLandingCraft)
			{
				if (!LCRFselectedTarget)
				{
					LCRFselectedTarget = GreatestThreat(pUnit, pTeam, 40, 2, enemyHouse, -1, -1, false, false);
				}
				if (LCRFselectedTarget)
				{
					pUnit->SetTarget(LCRFselectedTarget);
					pUnit->SetFocus(LCRFselectedTarget);
					pUnit->SetDestination(LCRFselectedTarget, false);
					pUnit->QueueMission(Mission::Eaten, true);
					Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Mind Controlled Transport [%s] (UID: %lu) is sent to Grinder.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pUnit->GetTechnoType()->get_ID(), pUnit->UniqueID);
				}
				continue;
			}

			if (!selectedTarget && !isSpy)
			{
				selectedTarget = GreatestThreat(pUnit, pTeam, 1, 2, enemyHouse, -1, -1, false, false);
			}
			if (isSpy)
			{
				selectedTarget = GreatestThreat(pUnit, pTeam, 4, 2, enemyHouse, -1, -1, false, false);
			}

			if (selectedTarget)
			{
				if (pUnit->IsAlive && !pUnit->InLimbo)
				{
					//auto pUnitType = pUnit->GetTechnoType();
					
					if (pUnitType && pUnit != selectedTarget && pUnit->Target != selectedTarget)
					{
						pUnit->CurrentTargets.Clear();
						if (pUnitType->Underwater && pUnitType->LandTargeting == LandTargetingType::Land_Not_OK
							&& selectedTarget->GetCell()->LandType != LandType::Water) // Land not OK for the Naval unit
						{
							// Naval units like Submarines are unable to target ground targets
							// except if they have anti-ground weapons. Ignore the attack
							pUnit->CurrentTargets.Clear();
							pUnit->SetTarget(nullptr);
							pUnit->SetFocus(nullptr);
							pUnit->SetDestination(nullptr, false);
							pUnit->QueueMission(Mission::Area_Guard, true);

							continue;
						}

						// Aircraft hack. I hate how this game auto-manages the aircraft missions.
						if (pUnitType->WhatAmI() == AbstractType::AircraftType
							&& pUnit->Ammo > 0 && pUnit->GetHeight() <= 0)
						{
							pUnit->SetDestination(selectedTarget, false);
							pUnit->QueueMission(Mission::Attack, true);
						}
						if (!isSpy || (isSpy && pUnit->Owner->IsAlliedWith(pUnit->DisguisedAsHouse)))
							pUnit->SetTarget(selectedTarget);

						if (pUnit->IsEngineer())
						{
							pUnit->QueueMission(Mission::Capture, true);
						}
						else
						{
							// Aircraft hack. I hate how this game auto-manages the aircraft missions.
							if (pUnitType->WhatAmI() != AbstractType::AircraftType)
							{
								pUnit->QueueMission(Mission::Attack, true);
								pUnit->ObjectClickedAction(Action::Attack, selectedTarget, false);

								if (pUnit->GetCurrentMission() != Mission::Attack)
									pUnit->Mission_Attack();

								if (pUnit->GetCurrentMission() == Mission::Move && pUnitType->JumpJet)
									pUnit->Mission_Attack();
							}
						}

						// Spy case
						if (isSpy && pUnit->Owner->IsAlliedWith(pUnit->DisguisedAsHouse))
						{
							pUnit->QueueMission(Mission::Attack, true);
							pUnit->ObjectClickedAction(Action::Attack, selectedTarget, false);
							pUnit->Mission_Attack();
						}

						// Tanya / Commando C4 case
						if ((pUnitType->WhatAmI() == AbstractType::InfantryType && (abstract_cast<InfantryTypeClass*>(pUnitType)->C4 || pUnit->HasAbility(Ability::C4))) && pUnit->GetCurrentMission() != Mission::Sabotage)
						{
							pUnit->Mission_Attack();
							pUnit->QueueMission(Mission::Sabotage, true);
						}
					}
					else
					{
						pUnit->QueueMission(Mission::Attack, true);
						pUnit->ObjectClickedAction(Action::Attack, selectedTarget, false);
						pUnit->Mission_Attack();
					}

					if (isSpy)
					{
						if (!pUnit->Owner->IsAlliedWith(pUnit->DisguisedAsHouse))
						{
							SpyselectedTarget = GreatestThreat(pUnit, pTeam, 41, 2, enemyHouse, -1, -1, true, false);
							if (SpyselectedTarget)
							{
								pUnit->SetTarget(SpyselectedTarget);
								pUnit->QueueMission(Mission::Enter, true);
								//if (pUnit->GetCurrentMission() != Mission::Enter)
								pUnit->Mission_Enter();
								Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Mind Controlled Spy [%s] (UID: %lu) selected [%s] (UID: %lu) as target.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pUnit->GetTechnoType()->get_ID(), pUnit->UniqueID, SpyselectedTarget->GetTechnoType()->get_ID(), SpyselectedTarget->UniqueID);

							}
						}


					}

				}
				Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Mind Controlled Unit [%s] (UID: %lu) selected [%s] (UID: %lu) as target.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pUnit->GetTechnoType()->get_ID(), pUnit->UniqueID, selectedTarget->GetTechnoType()->get_ID(), selectedTarget->UniqueID);
			}
			continue;
		}
	}

	pTeam->StepCompleted = true;
	return;
}

void ScriptExt::AllyUnitEnterTransport(TeamClass* pTeam)
{
	auto pScript = pTeam->CurrentScript;
	int scriptArgument = pScript->Type->ScriptActions[pScript->CurrentMission].Argument;
	TechnoClass* selectedTarget = nullptr;
	FootClass* pLeaderUnit = nullptr;
	int number = 0;
	int totalSize = 0;
	bool hasTransport = true;
	bool canLoad = true;

	if (!pTeam)
		return;

	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	if (!pTeamData)
		return;

	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
	{
		number++;
		if (pUnit->GetTechnoType()->Passengers < 1)
			hasTransport = false;
	}

	if (number > 1 || !hasTransport)
	{
		Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) This line is forcefully skipped (more than one member || no transports).\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument);
		pTeamData->AllyPassengers.Clear();
		pTeam->StepCompleted = true;
		return;
	}

	// Find the Leader
	pLeaderUnit = pTeamData->TeamLeader;
	if (!pLeaderUnit
		|| !pLeaderUnit->IsAlive
		|| pLeaderUnit->Health <= 0
		|| pLeaderUnit->InLimbo
		|| !pLeaderUnit->IsOnMap
		|| pLeaderUnit->Absorbed)
	{
		pLeaderUnit = FindTheTeamLeader(pTeam);
		pTeamData->TeamLeader = pLeaderUnit;
	}

	if (!pLeaderUnit)
	{
		// This action finished
		pTeam->StepCompleted = true;
		return;
	}

	bool isnotLast = true;
	while (isnotLast)
	{
		for (int i = pTeamData->AllyPassengers.Count - 1; i >= 0; i--)
		{
			if (i <= 0)
				isnotLast = false;
			auto pTarget = pTeamData->AllyPassengers[i];
			if (!pTarget
				|| !pTarget->IsAlive
				|| pTarget->Health <= 0
				|| pTarget->InLimbo
				|| !pTarget->IsOnMap
				|| pTarget->Absorbed)
			{
				pTeamData->AllyPassengers.RemoveItem(i);
				break;
			}
			else
				totalSize += (int)pTarget->GetTechnoType()->Size;
		}
		if (pTeamData->AllyPassengers.Count == 0)
			isnotLast = false;
	}


	if (totalSize == pLeaderUnit->GetTechnoType()->Passengers - pLeaderUnit->Passengers.NumPassengers)
		canLoad = false;

	while (canLoad)
	{
		double passengersPip = 0.0;

		if (RulesExt::Global()->AITargetTypesLists.size() > 0
		&& RulesExt::Global()->AITargetTypesLists[scriptArgument].size() > 0)
		{
			selectedTarget = FindBestObjectForAllyUnitEnterTransport(pLeaderUnit, pTeam, scriptArgument, 2, true, scriptArgument, -1);
		}
		else
		{
			pTeamData->AllyPassengers.Clear();
			pTeam->StepCompleted = true;
			return;
		}

		if (!selectedTarget)
			break;

		auto pFoot = abstract_cast<FootClass*>(selectedTarget);
		for (int j = 0; j < pTeamData->AllyPassengers.Count; j++)
		{
			passengersPip += pTeamData->AllyPassengers[j]->GetTechnoType()->Size;
		}

		if (passengersPip + selectedTarget->GetTechnoType()->Size > pLeaderUnit->GetTechnoType()->Passengers - pLeaderUnit->Passengers.NumPassengers)
			break;

		pTeamData->AllyPassengers.AddUnique(pFoot);
		Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) Team Leader [%s] (UID: %lu) asked [%s] (UID: %lu) to load onto it.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, pLeaderUnit->GetTechnoType()->get_ID(), pLeaderUnit->UniqueID, pFoot->GetTechnoType()->get_ID(), pFoot->UniqueID);


		if (pFoot->BelongsToATeam())
			if (pFoot->Team)
			{
				pFoot->Team->LiberateMember(pFoot);
				pFoot->RecruitableA = false;
				pFoot->RecruitableB = false;
			}
		pFoot->CurrentTargets.Clear();
		pFoot->SetTarget(nullptr);
		pFoot->SetFocus(nullptr);
		pFoot->SetDestination(nullptr, false);
		pFoot->QueueMission(Mission::Area_Guard, true);	
	}

	for (int i = 0; i < pTeamData->AllyPassengers.Count; i++)
	{
		auto pPassenger = pTeamData->AllyPassengers[i];

		if (pPassenger->BelongsToATeam())
			if (pPassenger->Team)
			{
				pPassenger->Team->LiberateMember(pPassenger);
			}

		if (pPassenger->GetCurrentMission() != Mission::Enter)
		{
			pPassenger->QueueMission(Mission::Enter, false);
			pPassenger->SetTarget(nullptr);
			pPassenger->SetDestination(pLeaderUnit, true);

			return;
		}
	}


	// Is loading
	for (int i = 0; i < pTeamData->AllyPassengers.Count; i++)
	{
		auto pPassenger = pTeamData->AllyPassengers[i];

		if (pPassenger->GetCurrentMission() == Mission::Enter)
			return;
	}

	pTeamData->AllyPassengers.Clear();
	pTeam->StepCompleted = true;
	return;
}

TechnoClass* ScriptExt::FindBestObjectForAllyUnitEnterTransport(TechnoClass* pTechno, TeamClass* pTeam, int method, int calcThreatMode = 0, bool pickAllies = false, int attackAITargetType = -1, int idxAITargetTypeItem = -1)
{
	TechnoClass* bestObject = nullptr;
	double bestVal = -1;
	HouseClass* enemyHouse = nullptr;
	auto pTeamData = TeamExt::ExtMap.Find(pTeam);

	// Favorite Enemy House case. If set, AI will focus against that House
	if (!pickAllies && pTechno->BelongsToATeam())
	{
		auto pFoot = abstract_cast<FootClass*>(pTechno);
		if (pFoot)
		{
			int enemyHouseIndex = pFoot->Team->FirstUnit->Owner->EnemyHouseIndex;

			if (pFoot->Team->Type->OnlyTargetHouseEnemy
				&& enemyHouseIndex >= 0)
			{
				enemyHouse = HouseClass::Array->GetItem(enemyHouseIndex);
			}
		}
	}

	// Generic method for targeting
	for (int i = 0; i < TechnoClass::Array->Count; i++)
	{
		auto object = TechnoClass::Array->GetItem(i);
		auto objectType = object->GetTechnoType();
		auto pTechnoType = pTechno->GetTechnoType();

		if (!object || !objectType || !pTechnoType)
			continue;

		bool isSameTarget = false;
		for (int j = 0; j < pTeamData->AllyPassengers.Count; j++)
		{
			if (object == pTeamData->AllyPassengers[j])
				isSameTarget = true;
		}
		if (isSameTarget)
			continue;


		if (object->GetTechnoType()->Size > pTechno->GetTechnoType()->SizeLimit)

		if (enemyHouse && enemyHouse != object->Owner)
			continue;

		if (!object->Owner->IsAlliedWith(pTeam->Owner))
			continue;

		if (objectType->WhatAmI() != AbstractType::InfantryType && objectType->WhatAmI() != AbstractType::UnitType)
		{
			continue;
		}

		if (object->GetCurrentMission() == Mission::Enter)
			continue;

		if (object->Owner == pTeam->Owner)
			continue;

		if (object->Owner->IsControlledByHuman())
			continue;

		if (object->Owner->IsNeutral())
			continue;

		if (object->IsMindControlled())
			continue;

		if (object->CaptureManager)
			if (object->CaptureManager->IsControllingSomething())
				continue;

		// Don't pick underground units
		if (object->InWhichLayer() == Layer::Underground)
			continue;

		// Stealth ground unit check
		if (object->CloakState == CloakState::Cloaked && !objectType->Naval)
			continue;

		// Submarines aren't a valid target
		if (object->CloakState == CloakState::Cloaked
			&& objectType->Underwater
			&& (pTechnoType->NavalTargeting == NavalTargetingType::Underwater_Never
				|| pTechnoType->NavalTargeting == NavalTargetingType::Naval_None))
		{
			continue;
		}

		// Land not OK for the Naval unit
		if (!objectType->Naval
			&& pTechnoType->LandTargeting == LandTargetingType::Land_Not_OK
			&& object->GetCell()->LandType != LandType::Water)
		{
			continue;
		}

		if (object != pTechno
			&& object->IsAlive
			&& !object->InLimbo
			&& object->IsOnMap
			&& !object->Absorbed
			&& ((pickAllies && pTechno->Owner->IsAlliedWith(object))
				|| (!pickAllies && !pTechno->Owner->IsAlliedWith(object))))
		{
			double value = 0;

			if (EvaluateObjectWithMask(object, method, attackAITargetType, idxAITargetTypeItem, pTechno))
			{
				CellStruct newCell;
				newCell.X = (short)object->Location.X;
				newCell.Y = (short)object->Location.Y;

				bool isGoodTarget = false;

				if (calcThreatMode == 0 || calcThreatMode == 1)
				{
					// Threat affected by distance
					double threatMultiplier = 128.0;
					double objectThreatValue = objectType->ThreatPosed;

					if (objectType->SpecialThreatValue > 0)
					{
						double const& TargetSpecialThreatCoefficientDefault = RulesClass::Instance->TargetSpecialThreatCoefficientDefault;
						objectThreatValue += objectType->SpecialThreatValue * TargetSpecialThreatCoefficientDefault;
					}

					// Is Defender house targeting Attacker House? if "yes" then more Threat
					if (pTechno->Owner == HouseClass::Array->GetItem(object->Owner->EnemyHouseIndex))
					{
						double const& EnemyHouseThreatBonus = RulesClass::Instance->EnemyHouseThreatBonus;
						objectThreatValue += EnemyHouseThreatBonus;
					}

					// Extra threat based on current health. More damaged == More threat (almost destroyed objects gets more priority)
					objectThreatValue += object->Health * (1 - object->GetHealthPercentage());
					value = (objectThreatValue * threatMultiplier) / ((pTechno->DistanceFrom(object) / 256.0) + 1.0);

					if (calcThreatMode == 0)
					{
						// Is this object very FAR? then LESS THREAT against pTechno.
						// More CLOSER? MORE THREAT for pTechno.
						if (value > bestVal || bestVal < 0)
							isGoodTarget = true;
					}
					else
					{
						// Is this object very FAR? then MORE THREAT against pTechno.
						// More CLOSER? LESS THREAT for pTechno.
						if (value < bestVal || bestVal < 0)
							isGoodTarget = true;
					}
				}
				else
				{
					// Selection affected by distance
					if (calcThreatMode == 2)
					{
						// Is this object very FAR? then LESS THREAT against pTechno.
						// More CLOSER? MORE THREAT for pTechno.
						value = pTechno->DistanceFrom(object); // Note: distance is in leptons (*256)

						if (value < bestVal || bestVal < 0)
							isGoodTarget = true;
					}
					else
					{
						if (calcThreatMode == 3)
						{
							// Is this object very FAR? then MORE THREAT against pTechno.
							// More CLOSER? LESS THREAT for pTechno.
							value = pTechno->DistanceFrom(object); // Note: distance is in leptons (*256)

							if (value > bestVal || bestVal < 0)
								isGoodTarget = true;
						}
					}
				}

				if (isGoodTarget)
				{
					bestObject = object;
					bestVal = value;
				}
			}
		}
	}

	return bestObject;
}

void ScriptExt::SetPickNeutral(TeamClass* pTeam)
{
	int argument = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;
	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	if (pTeamData)
	{
		if (argument > 0)
			pTeamData->SelectNeural = true;
		else
			pTeamData->SelectNeural = false;
	}

	//This action finished
	pTeam->StepCompleted = true;
	return;
}


void ScriptExt::MultiGreatestThreat(TechnoClass* pTechno, TeamClass* pTeam, int number, int method, int calcThreatMode = 0, HouseClass* onlyTargetThisHouseEnemy = nullptr, int attackAITargetType = -1, int idxAITargetTypeItem = -1, bool agentMode = false, bool individual = false)
{
	TechnoClass* bestObject = nullptr;
	
	DynamicVectorClass<TechnoClass*> TargetList;
	DynamicVectorClass<TechnoClass*> TargetList2;
	auto pTeamData = TeamExt::ExtMap.Find(pTeam);
	//pTeamData->IndividualTargets.Clear();

	bool unitWeaponsHaveAA = false;
	bool unitWeaponsHaveAG = false;

	bool enough = true;
	for (int k = 0; k < 5; k++)
	{

	
		double bestVal = -1;
		// Generic method for targeting
		for (int i = 0; i < TechnoClass::Array->Count; i++)
		{
			auto object = TechnoClass::Array->GetItem(i);
			auto objectType = object->GetTechnoType();
			auto pTechnoType = pTechno->GetTechnoType();

			if (!object || !objectType || !pTechnoType)
				continue;

			if (pTeamData)
			{
				int attackTargetRank = pTeamData->AttackTargetRank;
				if (attackTargetRank == 0)
				{
					if (!object->Veterancy.IsRookie())
						continue;
				}
				else if (attackTargetRank == 1)
				{
					if (!object->Veterancy.IsVeteran())
						continue;
				}
				else if (attackTargetRank == 2)
				{
					if (!object->Veterancy.IsElite())
						continue;
				}
				else if (attackTargetRank == 3)
				{
					if (object->Veterancy.IsRookie())
						continue;
				}

				if (individual)
				{
					bool isSameTarget = false;
					for (int j = 0; j < pTeamData->IndividualTargets.Count; j++)
					{
						if (object == pTeamData->IndividualTargets[j])
							isSameTarget = true;
					}
					if (isSameTarget)
						continue;
				}
				if (!pTeamData->SelectNeural)
					if (object->Owner->IsNeutral())
						continue;
			}

			// Note: the TEAM LEADER is picked for this task, be careful with leadership values in your mod
			int weaponIndex = pTechno->SelectWeapon(object);
			auto weaponType = pTechno->GetWeapon(weaponIndex)->WeaponType;

			if (weaponType && weaponType->Projectile->AA)
				unitWeaponsHaveAA = true;

			if ((weaponType && weaponType->Projectile->AG) || agentMode)
				unitWeaponsHaveAG = true;

			int weaponDamage = 0;

			if (weaponType)
			{
				if (weaponType->AmbientDamage > 0)
					weaponDamage = MapClass::GetTotalDamage(weaponType->AmbientDamage, weaponType->Warhead, objectType->Armor, 0) + MapClass::GetTotalDamage(weaponType->Damage, weaponType->Warhead, objectType->Armor, 0);
				else
					weaponDamage = MapClass::GetTotalDamage(weaponType->Damage, weaponType->Warhead, objectType->Armor, 0);
			}

			// If the target can't be damaged then isn't a valid target
			if (weaponType && weaponDamage <= 0 && !agentMode)
				continue;

			if (!agentMode)
			{
				if (object->IsInAir() && !unitWeaponsHaveAA)
					continue;

				if (!object->IsInAir() && !unitWeaponsHaveAG)
					continue;
			}

			// Don't pick underground units
			if (object->InWhichLayer() == Layer::Underground)
				continue;

			// Stealth ground unit check
			if (object->CloakState == CloakState::Cloaked && !objectType->Naval)
				continue;

			// Submarines aren't a valid target
			if (object->CloakState == CloakState::Cloaked
				&& objectType->Underwater
				&& (pTechnoType->NavalTargeting == NavalTargetingType::Underwater_Never || pTechnoType->NavalTargeting == NavalTargetingType::Naval_None))
			{
				continue;
			}

			// Land not OK for the Naval unit
			if (!objectType->Naval
				&& pTechnoType->LandTargeting == LandTargetingType::Land_Not_OK
				&& object->GetCell()->LandType != LandType::Water)
			{
				continue;
			}

			// OnlyTargetHouseEnemy forces targets of a specific (hated) house
			if (onlyTargetThisHouseEnemy && object->Owner != onlyTargetThisHouseEnemy)
				continue;

			if (object != pTechno
				&& object->IsAlive
				&& object->Health > 0
				&& !object->InLimbo
				&& !objectType->Immune
				&& !object->Transporter
				&& object->IsOnMap
				&& !object->Absorbed
				&& !object->TemporalTargetingMe
				&& !object->BeingWarpedOut
				&& (object->Owner != pTechno->Owner || (object->Owner == pTechno->Owner && attackAITargetType == -1 && method == 39))
				&& (!pTechno->Owner->IsAlliedWith(object) || (object->Owner == pTechno->Owner && attackAITargetType == -1 && method == 39)
					|| (pTechno->Owner->IsAlliedWith(object)
						&& object->IsMindControlled()
						&& !pTechno->Owner->IsAlliedWith(object->MindControlledBy))))
			{
				double value = 0;

				if (EvaluateObjectWithMask(object, method, attackAITargetType, idxAITargetTypeItem, pTechno))
				{
					CellStruct newCell;
					newCell.X = (short)object->Location.X;
					newCell.Y = (short)object->Location.Y;

					bool isGoodTarget = false;

					if (calcThreatMode == 0 || calcThreatMode == 1)
					{
						// Threat affected by distance
						double threatMultiplier = 128.0;
						double objectThreatValue = objectType->ThreatPosed;

						if (objectType->SpecialThreatValue > 0)
						{
							double const& TargetSpecialThreatCoefficientDefault = RulesClass::Instance->TargetSpecialThreatCoefficientDefault;
							objectThreatValue += objectType->SpecialThreatValue * TargetSpecialThreatCoefficientDefault;
						}

						// Is Defender house targeting Attacker House? if "yes" then more Threat
						if (pTechno->Owner == HouseClass::Array->GetItem(object->Owner->EnemyHouseIndex))
						{
							double const& EnemyHouseThreatBonus = RulesClass::Instance->EnemyHouseThreatBonus;
							objectThreatValue += EnemyHouseThreatBonus;
						}

						// Extra threat based on current health. More damaged == More threat (almost destroyed objects gets more priority)
						objectThreatValue += object->Health * (1 - object->GetHealthPercentage());
						value = (objectThreatValue * threatMultiplier) / ((pTechno->DistanceFrom(object) / 256.0) + 1.0);

						if (calcThreatMode == 0)
						{
							// Is this object very FAR? then LESS THREAT against pTechno.
							// More CLOSER? MORE THREAT for pTechno.
							if (value > bestVal || bestVal < 0)
								isGoodTarget = true;
						}
						else
						{
							// Is this object very FAR? then MORE THREAT against pTechno.
							// More CLOSER? LESS THREAT for pTechno.
							if (value < bestVal || bestVal < 0)
								isGoodTarget = true;
						}
					}
					else
					{
						// Selection affected by distance
						if (calcThreatMode == 2)
						{
							// Is this object very FAR? then LESS THREAT against pTechno.
							// More CLOSER? MORE THREAT for pTechno.
							value = pTechno->DistanceFrom(object); // Note: distance is in leptons (*256)

							if (value < bestVal || bestVal < 0)
								isGoodTarget = true;
						}
						else
						{
							if (calcThreatMode == 3)
							{
								// Is this object very FAR? then MORE THREAT against pTechno.
								// More CLOSER? LESS THREAT for pTechno.
								value = pTechno->DistanceFrom(object); // Note: distance is in leptons (*256)

								if (value > bestVal || bestVal < 0)
									isGoodTarget = true;
							}
						}
					}

					if (!enough)
					{
						if (pTechno->DistanceFrom(object) < k * 15)
						{
							TargetList2.AddUnique(object);

							auto pScript = pTeam->CurrentScript;
							Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) add Target [%s] (UID:%lu) becasue it was not enough.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, object->GetTechnoType()->get_ID(), object->UniqueID);							
						}
							
						if (k == 4)
						{
							TargetList2.AddUnique(object);
							auto pScript = pTeam->CurrentScript;
							Debug::Log("DEBUG: [%s] [%s] (line: %d = %d,%d) add Target [%s] (UID:%lu) becasue it was not enough.\n", pTeam->Type->ID, pScript->Type->ID, pScript->CurrentMission, pScript->Type->ScriptActions[pScript->CurrentMission].Action, pScript->Type->ScriptActions[pScript->CurrentMission].Argument, object->GetTechnoType()->get_ID(), object->UniqueID);
						}

					}

					if (isGoodTarget)
					{
						bestObject = object;
						bestVal = value;
						TargetList.AddUnique(object);
					}
					if (TargetList.Count + TargetList2.Count >= number)
						break;
				}
			}
		}

		if (TargetList.Count + TargetList2.Count >= number)
			break;
		else
			enough = false;
	}

	int count = 0;
	for (int i = TargetList.Count - 1; i >= 0; i--)
	{
		if (count > number)
			break;
		pTeamData->IndividualTargets.AddUnique(TargetList[i]);
		count++;
	}
	if (pTeamData->IndividualTargets.Count < number)
	{
		for (int i = 0; i < TargetList2.Count; i++)
		{
			if (count > number)
				break;
			pTeamData->IndividualTargets.AddUnique(TargetList2[i]);
			count++;
		}
	}

}



//void ScriptExt::RallyNearbyUnits(TeamClass* pTeam)
//{
//	int rallyDistance = pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument;
//	DynamicVectorClass<FootClass*> unitsReadyToRally;
//
//	for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
//	{
//		for (int i = 0; i < FootClass::Array->Count; i++)
//		{
//			auto pFoot = (*FootClass::Array)[i];
//
//			// Must be owner
//			if (pFoot
//				&& !pFoot->InLimbo && pFoot->Health > 0
//				&& pFoot->Owner == pTeam->Owner
//				&& pFoot->DistanceFrom(pUnit) / 256.0 <= rallyDistance)
//			{
//				unitsReadyToRally.AddItem(pFoot);
//			}
//		}
//	}
//
//	if (unitsReadyToRally.Count > 0)
//	{
//		for (int i = 0; i < unitsReadyToRally.Count; i++)
//		{
//			pTeam->AddMember(unitsReadyToRally.GetItem(i), true);
//		}
//	}
//
//
//	// This action finished
//	pTeam->StepCompleted = true;
//	return;
//}

//void ScriptExt::EngineerRepairBrokenBridge(TeamClass* pTeam)
//{
//	DynamicVectorClass<BuildingClass *> bridgeRepairHuts;
//	DynamicVectorClass<CellClass*> bridgeRepairHutsCells;
//
//	for (int i = 0; i < TechnoClass::Array->Count; i++)
//	{
//		auto pTechno = TechnoClass::Array->GetItem(i);
//		if (auto pBuilding = abstract_cast<BuildingClass*>(pTechno))
//		{ 
//			auto pBuildingType = abstract_cast<BuildingTypeClass*>(pTechno->GetTechnoType());
//			if (pBuildingType->BridgeRepairHut)
//			{
//				bridgeRepairHuts.AddItem(pBuilding);
//				bridgeRepairHutsCells.AddItem(pBuilding->GetCell());
//			}
//		}
//	}
//
//	//for (int i = 0; i < bridgeRepairHutsCells.Count; i++)
//	//{
//	//	CellClass* pCell = bridgeRepairHutsCells.GetItem(i);
//	//	
//	//	if (pCell->ContainsBridgeEx())
//	//	{
//	//		//auto pTerrain = pCell->GetTerrain( 1 );
//	//		Debug::Log("Height = 233.\n");
//	//		pCell->FogCell();//BlowUpBridge();
//	//	}
//	//	else
//	//	{
//	//		Debug::Log("Height = 114.\n");
//	//		pCell->BridgeOwnerCell->BlowUpBridge();
//	//	}
//	//}
//	//bool HasTarget = false;
//
//	//while (!HasTarget)
//	//{
//		int IdxSelectedHut = ScenarioClass::Instance->Random.RandomRanged(0, bridgeRepairHuts.Count - 1);
//		auto SelectedHut = bridgeRepairHuts.GetItem(IdxSelectedHut);
//
//
//		for (auto pUnit = pTeam->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
//		{
//			if (SelectedHut->ShouldRebuild)
//			{
//				pUnit->SetTarget(SelectedHut);
//				//HasTarget = true;
//
//				if (pUnit->IsEngineer())
//				{
//					pUnit->QueueMission(Mission::Capture, true);
//				}
//			}
//		}
//	//}
//
//	//This action finished
//	pTeam->StepCompleted = true;
//	return;
//}
