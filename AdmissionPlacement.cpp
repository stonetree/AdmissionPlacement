// AdmissionPlacement.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cRequest.h"
#include "cEvent.h"
#include "cServer.h"
#include "common.h"
#include "cSystemState.h"

const unsigned int sample_request_num = 500;
const unsigned int total_request = 1000;
const unsigned int total_server_num = 50;
const unsigned int total_service_type_num = 3;
const double discout_factor = 0.8;
const double value_function_update_factor = 0.8;

const double local_communication_cost = 0.03;
const double tor_communication_cost = 0.05;
const double remote_communication_cost = 0.1;

vector<vector<double>> commu_cost;
vector<double> basisFuncParameter;

pair<requesttype,double> system_state;
double initial_system_state_indicator;

double average_accepted_requests_num = 0;
double accepted_requests_num = 0;
double sample_index = 0;


vector<pair<string,placementfunction>> policy_vec;
map<pair<requesttype,double>,double> system_state_value_map;
map<pair<requesttype,double>,cPolity> system_state_policy_map;

int _tmain(int argc, _TCHAR* argv[])
{

	//In order to accelerate the access to the system state, we code the system state into a double which is calculated as followed
	//for each server, state_index = (cpu_residual+mem_residual+disk_residual) * server_id
	//for the whole system, state_index = sum the state_index over the all servers.
	//With this method, we can quickly locate the state and its corresponding value.
	system_state_value_map.clear();

	//store the system state visited ever
	system_state_policy_map.clear();

	//initial the set of policies
	policy_vec.clear();
	
	unsigned int sample_index;
	//unsigned int sample_index;
	for (sample_index = 0;sample_index < sample_request_num; sample_index++)
	{
		average_accepted_requests_num = 0;

		cout<<"The "<<sample_index<<" sample path"<<endl;
		vector<cRequest> request_vec;
		multimap<double,cEvent> event_multimap;

		//initial the set of physical servers
		vector<cServer> server_vec;
		initialPhyServers(server_vec);

		if (sample_index == 0)
		{
			initialCommuCost(server_vec);
			initialBasisFuncParameters(server_vec);
		}

		initialSystemState(server_vec);

		

		generateSampleEvent(request_vec,event_multimap);


		obtainOptimalStateValue(event_multimap,server_vec);

	}

	//outputResults();

	outputResultsBasisFunct();

	return 0;
}

