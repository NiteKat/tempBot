#include "JobManager.h"

using namespace BWAPI;

JobManager* JobManager::s_instance = new JobManager();

JobManager* JobManager::instance()
{
	if (!s_instance)
		s_instance = new JobManager;
	return s_instance;
}

void JobManager::addJob(Job new_job)
{
	job_queue.push_back(new_job);
}

void JobManager::runJobs()
{
	if (job_queue.size() == 0)
		return;
	else
	{
		auto job_queue_iterator = job_queue.begin();
		while (job_queue_iterator != job_queue.end())
		{
			if (job_queue_iterator->execute())
			{
				auto erase_iterator = job_queue_iterator;
				job_queue_iterator = job_queue.erase(erase_iterator);
			}
			else
				job_queue_iterator++;
		}
	}
}

void JobManager::printQueue()
{
	if (job_queue.size() == 0)
		Broodwar->drawTextScreen(0, 12, "Job Queue empty.");
	else
	{
		int job_number = 1;
		auto job_queue_iterator = job_queue.begin();
		while (job_queue_iterator != job_queue.end() &&
			job_number < 11)
		{
			if (job_queue_iterator->getType() == BUILD_SCV)
				Broodwar->drawTextScreen(0, 12 * job_number, "Build_SCV");
			job_queue_iterator++;
			job_number++;
		}
	}
}