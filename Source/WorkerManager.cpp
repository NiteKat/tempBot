#include"WorkerManager.h"

WorkerManager* WorkerManager::s_instance = new WorkerManager();

namespace { auto & game_state = *GameState::instance(); }

WorkerManager* WorkerManager::instance()
{
	if (!s_instance)
		s_instance = new WorkerManager;
	return s_instance;
}

int WorkerManager::manageWorkers()
{
	int minerals_committed_adjust = 0;
	auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
	{
		if (mineral_worker_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = mineral_worker_iterator;
			mineral_worker_iterator = game_state.getMineralWorkers()->erase(erase_iterator);
		}
		else if (!mineral_worker_iterator->getUnit()->exists())
		{
			auto erase_iterator = mineral_worker_iterator;
			mineral_worker_iterator = game_state.getMineralWorkers()->erase(erase_iterator);
		}
		else
		{
			if (mineral_worker_iterator->getUnit()->isUnderAttack() &&
				mineral_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::AttackUnit)
			{
				for (auto unit : mineral_worker_iterator->getUnit()->getUnitsInRadius(10))
				{
					if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling)
					{
						mineral_worker_iterator->getUnit()->attack(unit);
					}
				}
				mineral_worker_iterator++;
			}
			else if (mineral_worker_iterator->getUnit()->isIdle())
			{
				if (mineral_worker_iterator->getBase()->getArea()->Minerals().size() > 0)
				{
					mineral_worker_iterator->getUnit()->gather((*mineral_worker_iterator->getBase()->getArea()->Minerals().begin())->Unit());
				}
				mineral_worker_iterator++;
			}
			else
			{
				mineral_worker_iterator++;
			}
		}
	}
	return minerals_committed_adjust;
}