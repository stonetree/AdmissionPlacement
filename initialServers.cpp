#include "stdafx.h"
#include "cServer.h"
#include "common.h"


static const double server_cpu_capacity = 50;
static const double server_mem_capacity = 80;
static const double server_disk_capacity = 500;

void initialPhyServers(vector<cServer>& _server_vec)
{
	int server_index;
	ID id;
	for (server_index = 0,id = 1;server_index < total_server_num;server_index++,id++)
	{
		_server_vec.push_back(cServer(id,server_cpu_capacity,server_mem_capacity,server_disk_capacity));
	}
	
	return;
}

void initialSystemState(vector<cServer>& _server_vec)
{
	vector<cServer>::iterator iter_server_vec;
	system_state.first = NONE;
	system_state.second = 0;

	for (iter_server_vec = _server_vec.begin(); iter_server_vec != _server_vec.end();iter_server_vec++)
	{
		system_state.second += ((iter_server_vec->getcpuResidual() + iter_server_vec->getmemResidual() + iter_server_vec->getdiskResidual()) * iter_server_vec->getID());
	}
	
	initial_system_state_indicator = system_state.second;

	return ;
}