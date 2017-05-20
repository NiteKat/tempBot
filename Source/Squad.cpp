#include"Squad.h"

using namespace BWAPI;

namespace { auto & game_state = *GameState::instance(); }
namespace { auto & job_manager = *JobManager::instance(); }

void Squad::addUnit(Object new_unit)
{
	my_units.push_back(new_unit);
}

void Squad::setJob(Job new_job)
{
	my_job = new_job;
}

void Squad::executeJob()
{
	if (my_job.getType() == BUILD_SUPPLY_DEPOT_MAIN)
	{
		if (!my_units.begin()->getUnit()->exists())
		{
			job_manager.addJob(my_job);
			disband();
		}
		else if (my_units.begin()->getUnit()->getOrder() != Orders::PlaceBuilding &&
			my_units.begin()->getUnit()->getOrder() != Orders::ConstructingBuilding)
		{
			TilePosition build_location;
			auto building_list_iterator = game_state.getBuildingList()->begin();
			while (building_list_iterator != game_state.getBuildingList()->end())
			{
				if (building_list_iterator->getUnit()->getType() == UnitTypes::Terran_Supply_Depot &&
					building_list_iterator->getBase()->getBaseClass() == 3)
				{
					build_location.x = building_list_iterator->getUnit()->getTilePosition().x;
					build_location.y = building_list_iterator->getUnit()->getTilePosition().y;
					while (!Broodwar->canBuildHere(build_location, UnitTypes::Terran_Supply_Depot))
					{
						int random_number = rand() % 3 + 1;
						if (random_number == 1)
							build_location.x += 3;
						else if (random_number == 2)
							build_location.x += -3;
						random_number = rand() % 3 + 1;
						if (random_number == 1)
							build_location.y += 3;
						else if (random_number == 2)
							build_location.y += -3;
					}
					return;
				}
				else
					building_list_iterator++;
			}
			build_location = Broodwar->getBuildLocation(UnitTypes::Terran_Supply_Depot, my_units.begin()->getBase()->getArea()->Bases().begin()->Location());
			while (!Broodwar->canBuildHere(build_location, UnitTypes::Terran_Supply_Depot))
			{
				int random_number = rand() % 3 + 1;
				if (random_number == 1)
					build_location.x += 3;
				else if (random_number == 2)
					build_location.x += -3;
				random_number = rand() % 3 + 1;
				if (random_number == 1)
					build_location.y += 3;
				else if (random_number == 2)
					build_location.y += -3;
			}
			my_units.begin()->getUnit()->build(UnitTypes::Terran_Supply_Depot, build_location);
		}
		else if (my_units.begin()->getUnit()->getOrder() == Orders::ConstructingBuilding)
		{
			disband();
		}
	}
}

void Squad::disband()
{
	if (my_units.size() != 0)
	{
		auto unit_iterator = my_units.begin();
		while (unit_iterator != my_units.end())
		{
			if (unit_iterator->getUnit()->getType() == UnitTypes::Terran_SCV)
			{
				game_state.addMineralWorker(*unit_iterator);
			}
			unit_iterator++;
		}
	}
}