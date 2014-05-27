// AdmissionPlacement.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cRequest.h"
#include "cEvent.h"
#include "cServer.h"
#include "common.h"

static const unsigned int sample_request_num = 10000;


int _tmain(int argc, _TCHAR* argv[])
{

	//In order to accelerate the access to the system state, we code the system state into a double which is calculated as followed
	//for each server, state_index = (cpu_residual+mem_residual+disk_residual) * server_id
	//for the whole system, state_index = sum the state_index over the all servers.
	//With this method, we can quickly locate the state and its corresponding value.
	map<double,double> state_value;
	
	unsigned int sample_index;
	//for (sample_index = 0;sample_index < sample_request_num; sample_index++)
	{
		vector<cRequest> request_vec;
		multimap<double,cEvent> event_multimap;

		//initial the set of physical servers
		vector<cServer> server_vec;
		initialPhyServers(server_vec);

		//initial the set of policies
		vector<pair<placementfunction,int>> policy_vec;
		//initialPolicies(policy_vec);

		generateSampleEvent(request_vec,event_multimap);

		obtainOptimalStateValue(event_multimap,request_vec,server_vec,policy_vec,state_value);
	}

	return 0;
}

