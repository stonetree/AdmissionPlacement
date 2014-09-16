// AdmissionPlacement.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cRequest.h"
#include "cEvent.h"
#include "cServer.h"
#include "common.h"
#include "cSystemState.h"

const unsigned int sample_request_num = 10000;
const unsigned int total_request = 100;
const unsigned int total_server_num = 10;
const unsigned int total_service_type_num = 3;
const double discout_factor = 0.999;
const double value_function_update_factor = 0.9;

const double local_communication_cost = 0.01;
const double tor_communication_cost = 0.02;
const double remote_communication_cost = 0.05;

vector<vector<double>> commu_cost;
vector<double> basisFuncParameter;

pair<requesttype,unsigned long int> system_state;
unsigned long int initial_system_state_indicator = 0;

double average_accepted_rate = 0;
double accepted_requests_num = 0;

double workload_rate = 0;

double current_time = 0;


vector<pair<string,placementfunction>> policy_vec;

system_value_map system_state_value_map;
system_policy_map system_state_policy_map;

system_value_map  *global_point_system_value_map = NULL;
system_policy_map *global_point_system_policy_map = NULL;

unsigned int sample_index = 0;

int _tmain(int argc, _TCHAR* argv[])
{

	//In order to accelerate the access to the system state, we code the system state into a double which is calculated as followed
	//for each server, state_index = (cpu_residual+mem_residual+disk_residual) * server_id
	//for the whole system, state_index = sum the state_index over the all servers.
	//With this method, we can quickly locate the state and its corresponding value.
	
	
	vector<cConfiguration> config_vec;

	initialInputfile(config_vec);

	vector<cConfiguration>::iterator iter_config_vec = config_vec.begin();
	for (;iter_config_vec != config_vec.end();iter_config_vec++)
	{
		
		//reset the seed of random number generator for generating arrival times
		interval_timeSeed = 10;

		//reset the seed of random number generator for generating duration times
		duration_time_seed = 100;

		//reset the system state value
		system_state_value_map.clear();

		//store the system state visited ever
		system_state_policy_map.clear();

		//initial the set of policies
		policy_vec.clear();

		//initial the input parameters
		initialInputParameter(iter_config_vec);

		average_accepted_rate = 0;
		
		system_value_map  *p_system_value_map = new system_value_map[total_request + 1];
		system_policy_map *p_system_policy_map = new system_policy_map[total_request + 1];

		global_point_system_value_map = p_system_value_map;
		global_point_system_policy_map = p_system_policy_map;

		//unsigned int sample_index;
		for (sample_index = 0;sample_index < sample_request_num; sample_index++)
		{
			accepted_requests_num = 0;

			cout<<"The "<<sample_index<<" sample path"<<endl;

			//initial request sequence
			//vector<cRequest> request_vec;
			vector<cRequest> *p_request_vec = new vector<cRequest>;

			multimap<double,cEvent> *p_event_multimap = new multimap<double,cEvent>;
			//multimap<double,cEvent> event_multimap;

			//initial the set of physical servers
			vector<cServer> *p_server_vec = new vector<cServer>;
			//vector<cServer> server_vec;
			initialPhyServers(*p_server_vec);

			if (iter_config_vec == config_vec.begin() && sample_index == 0)
			{
				initialCommuCost(*p_server_vec);
				//initialBasisFuncParameters(server_vec);
			}

			initialSystemState(*p_server_vec);

			generateSampleEvent(*p_request_vec,*p_event_multimap);

			obtainOptimalStateValue(*p_event_multimap,*p_server_vec);

			//reset the event indicator
			event_id = 1;
			
			average_accepted_rate += accepted_requests_num/total_request;

			delete p_request_vec;
			delete p_server_vec;
			delete p_event_multimap;

		}//end...for (sample_index...)

		outputResults();

		delete[] p_system_policy_map;
		delete[] p_system_value_map;
		
		//outputResultsBasisFunct();
	}//end...for (iter_config_vec)
	return 0;
}

