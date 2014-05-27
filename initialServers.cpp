#include "stdafx.h"
#include "cServer.h"

const unsigned int total_server_num = 50;
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