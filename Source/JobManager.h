#ifndef JOBMANAGER
#define JOBMANAGER

#include<vector>
#include"Job.h"


class JobManager
{
public:
	
	static JobManager *instance();

	void addJob(Job new_job);
	void runJobs();
	void printQueue();

private:
	std::vector<Job> job_queue;
	static JobManager *s_instance;
};

#endif