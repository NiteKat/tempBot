#include "NiteKat.h"
#include <iostream>
#include <ctime>



using namespace BWAPI;
using namespace Filter;
using namespace BWEM;
using namespace BWEM::BWAPI_ext;
using namespace BWEM::utils;

namespace { auto & theMap = BWEM::Map::Instance(); }
namespace { auto & game_state = *GameState::instance(); }
namespace { auto & job_manager = *JobManager::instance(); }
namespace { auto & worker_manager = *WorkerManager::instance(); }


void NiteKat::onStart()
{

	try
	{
		srand(time(NULL));
		// Hello World!
		Broodwar->sendText("Hello world!");

		// Print the map name.
		// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
		Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

		// Enable the UserInput flag, which allows us to control the bot and type messages.
		//Broodwar->enableFlag(Flag::UserInput);

		// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
		//Broodwar->enableFlag(Flag::CompleteMapInformation);

		// Set the command optimization level so that common commands can be grouped
		// and reduce the bot's APM (Actions Per Minute).
		Broodwar->setCommandOptimizationLevel(2);

		// Check if this is a replay
		if (Broodwar->isReplay())
		{

			// Announce the players in the replay
			Broodwar << "The following players are in this replay:" << std::endl;

			// Iterate all the players in the game using a std:: iterator
			Playerset players = Broodwar->getPlayers();
			for (auto p : players)
			{
				// Only print the player if they are not an observer
				if (!p->isObserver())
					Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
			}

		}
		else // if this is not a replay
		{
			// Retrieve you and your enemy's races. enemy() will just return the first enemy.
			// If you wish to deal with multiple enemies then you must use enemies().
			if (Broodwar->enemy()) // First make sure there is an enemy
				Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;

			Broodwar << "Map initialization..." << std::endl;
			theMap.Initialize();
			theMap.EnableAutomaticPathAnalysis();
			bool startingLocationsOK = theMap.FindBasesForStartingLocations();
			assert(startingLocationsOK);

			BWEM::utils::MapPrinter::Initialize(&theMap);
			BWEM::utils::printMap(theMap);
			BWEM::utils::pathExample(theMap);
			theMap.FindBasesForStartingLocations();

			Broodwar << "gg" << std::endl;
			for (const auto &area : theMap.Areas())
			{
				int base_region = 0;
				if (theMap.GetNearestArea(Broodwar->self()->getStartLocation())->BottomRight() == area.BottomRight())
					base_region = 3;
				else if (area.Minerals().size() > 0)
					base_region = 1;
				AIBase new_base(&area, base_region);
				if (new_base.getBaseClass() == 3)
					new_base.toggleScouted();
				game_state.addAIBase(new_base);
			}
			game_state.initializeGasLocations();
			job_manager.addJob(JobType::BUILD_SCV);
			job_manager.addJob(JobType::BUILD_SCV);
			job_manager.addJob(JobType::BUILD_SCV);
			job_manager.addJob(JobType::BUILD_SCV);
			job_manager.addJob(JobType::BUILD_SCV);
		}
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void NiteKat::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!
	}
}

void NiteKat::onFrame()
{
	try
	{
		// Called once every game frame
		BWEM::utils::drawMap(theMap);
		job_manager.printQueue();

		// Return if the game is a replay or is paused
		if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
			return;


		// Prevent spamming by only running our onFrame once every number of latency frames.
		// Latency frames are the number of frames before commands are processed.
		if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
			return;
		worker_manager.manageWorkers();
		job_manager.runJobs();
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void NiteKat::onSendText(std::string text)
{

	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());


	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!

}

void NiteKat::onReceiveText(BWAPI::Player player, std::string text)
{
	BWEM::utils::MapDrawer::ProcessCommand(text);
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void NiteKat::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void NiteKat::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void NiteKat::onUnitDiscover(BWAPI::Unit unit)
{
}

void NiteKat::onUnitEvade(BWAPI::Unit unit)
{
}

void NiteKat::onUnitShow(BWAPI::Unit unit)
{
}

void NiteKat::onUnitHide(BWAPI::Unit unit)
{
}

void NiteKat::onUnitCreate(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
	else
	{
		if (unit->getType() == UnitTypes::Terran_SCV &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_worker(unit, game_state.getContainingBase(unit));
			game_state.addMineralWorker(new_worker);
			game_state.addSupplyUsed(1);
		}
		else if ((unit->getType() == UnitTypes::Terran_Marine ||
			unit->getType() == UnitTypes::Terran_Medic) &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_unit(unit);
			game_state.addSupplyUsed(1);
			game_state.addUnit(new_unit);
		}
		else if (unit->getType() == UnitTypes::Terran_Supply_Depot &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addMineralsCommitted(-100);
		}
		else if (unit->getType() == UnitTypes::Terran_Barracks &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addMineralsCommitted(-150);
		}
		else if (unit->getType() == UnitTypes::Terran_Refinery &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addMineralsCommitted(-100);
			game_state.setGeyserUsed(unit->getTilePosition());
		}
		else if (unit->getType() == UnitTypes::Terran_Comsat_Station &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addMineralsCommitted(-50);
			game_state.addGasCommitted(-50);
			game_state.toggleComsatStation();
		}
		else if (unit->getType() == UnitTypes::Terran_Academy &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addMineralsCommitted(-150);
		}
		else if (unit->getType() == UnitTypes::Terran_Command_Center &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
		}
	}
}

void NiteKat::onUnitDestroy(BWAPI::Unit unit)
{
	try
	{

		if (unit->getType().isMineralField())
		{
			theMap.OnMineralDestroyed(unit);
		}
		else if (unit->getType().isSpecialBuilding())
		{
			theMap.OnStaticBuildingDestroyed(unit);
		}

	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void NiteKat::onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
	else
	{
		
	}
}

void NiteKat::onUnitRenegade(BWAPI::Unit unit)
{
}

void NiteKat::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void NiteKat::onUnitComplete(BWAPI::Unit unit)
{
	if (unit->getType() == UnitTypes::Terran_Command_Center &&
		unit->getPlayer() == Broodwar->self())
	{
		game_state.addSupplyTotal(10);
	}
	else if ((unit->getType() == UnitTypes::Terran_Supply_Depot ||
		unit->getType() == UnitTypes::Protoss_Pylon) &&
		unit->getPlayer() == Broodwar->self())
	{
		game_state.addSupplyTotal(8);
	}
}
