#ifndef SQUAD
#define SQUAD

#include<vector>
#include"Object.h"
#include"Job.h"
#include"JobManager.h"

class Squad
{
public:
	void addUnit(Object new_unit);
	void setJob(Job new_job);
	void executeJob();
	void disband();

private:
	std::vector<Object> my_units;
	Job my_job;
};

#endif