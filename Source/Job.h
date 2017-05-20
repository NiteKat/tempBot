#ifndef JOB
#define JOB
#include"GameState.h"

enum JobType
{
	INVALID_JOB,
	BUILD_SCV,
	BUILD_SUPPLY_DEPOT_MAIN
};

class Job
{

public:
	Job(int new_job_type);
	Job();
	bool execute();

	int getType();
private:
	int job_type;
};

#endif