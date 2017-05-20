#include"Job.h"

using namespace BWAPI;

namespace { auto & game_state = *GameState::instance(); }

Job::Job()
{
	job_type = INVALID_JOB;
}

Job::Job(int new_job_type)
{
	job_type = new_job_type;
}

bool Job::execute()
{
	if (job_type == BUILD_SCV)
	{
		auto building_list_iterator = game_state.getBuildingList()->begin();
		while (building_list_iterator != game_state.getBuildingList()->end())
		{
			if (building_list_iterator->getUnit()->getType() == UnitTypes::Terran_Command_Center &&
				Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
				game_state.getSupplyUsed() < game_state.getSupplyTotal())
			{
				if (building_list_iterator->getUnit()->train(UnitTypes::Terran_SCV))
				{
					return true;
				}
				else
					return false;
			}
			else
			{
				building_list_iterator++;
			}
		}
		return false;
	}
	else if (job_type == BUILD_SUPPLY_DEPOT_MAIN)
	{
		Job temp_job(job_type);
		return game_state.createNewSquadForJob(temp_job);
	}
	return false;
}

int Job::getType()
{
	return job_type;
}