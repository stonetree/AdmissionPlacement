#include "stdafx.h"
#include "cServer.h"
#include "cService.h"
#include "cEvent.h"
#include "cRequest.h"
#include "common.h"
#include "cPolity.h"
#include "cConfiguration.h"

#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"

unsigned int policy_indicator = 0;
int counting = -1;

//if the arriving request is accepted,then we should allocate resources to it
void allocateRequest(cRequest* _p_request)
{
	vector<cVirtualMachine>::iterator iter_vm_vec;
	for (iter_vm_vec = _p_request->p_vm_vec->begin();iter_vm_vec != _p_request->p_vm_vec->end();iter_vm_vec++)
	{
		(iter_vm_vec->getHostedServerPoint())->allocateVirtMach(&(*iter_vm_vec));
	}
	return ;
}

//release resources for those departing requests 
void releaseRequest(cRequest* _p_request)
{
	vector<cVirtualMachine>::iterator iter_vm_vec;
	for (iter_vm_vec = _p_request->p_vm_vec->begin();iter_vm_vec != _p_request->p_vm_vec->end();iter_vm_vec++)
	{
		(iter_vm_vec->getHostedServerPoint())->releaseVirtMach(&(*iter_vm_vec));
	}	
	return ;
}

bool vmDeployment(vector<cServer>& _server_vec,cRequest* _request,pair<string,placementfunction>& _placement_func_pair,int* _iterationPlacement)
{
	bool is_accepted;
	
	_request->setAccepted(false);
	vector<cVirtualMachine>::iterator iter_vm_vec;
	for (iter_vm_vec = _request->p_vm_vec->begin();iter_vm_vec != _request->p_vm_vec->end();iter_vm_vec++)
	{
		iter_vm_vec->setHostedServID(0);
		iter_vm_vec->setHostedServPoint(NULL);
	}
	
	is_accepted = (_placement_func_pair.second)(_server_vec,_request,_iterationPlacement);
	if (!is_accepted && _placement_func_pair.first == "NO_PLACEMENT")
	{
		is_accepted = true;		
	}

	if (!_request->getAccepted())
	{
		for (iter_vm_vec = _request->p_vm_vec->begin();iter_vm_vec != _request->p_vm_vec->end();iter_vm_vec++)
		{
			iter_vm_vec->setHostedServID(0);
			iter_vm_vec->setHostedServPoint(NULL);
		}
	}

	return is_accepted;
}

unsigned long int BKDRHash(const vector<cServer>& _server_vec,const cRequest* _request)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned long int hash = 0;

	vector<unsigned long int> state_cpu_residual;
	vector<unsigned long int> state_mem_residual;
	vector<unsigned long int> state_disk_residual;

	vector<unsigned long int>::iterator iter_stat_cpu_residual;
	vector<unsigned long int>::iterator iter_state_mem_residual;
	vector<unsigned long int>::iterator iter_stat_disk_residual;

	vector<cServer>::const_iterator iter_server_vec = _server_vec.begin();
	for (;iter_server_vec != _server_vec.end();iter_server_vec++)
	{
		state_cpu_residual.push_back(iter_server_vec->getcpuResidual());
		state_mem_residual.push_back(iter_server_vec->getmemResidual());
		state_disk_residual.push_back(iter_server_vec->getdiskResidual());
	}

	vector<cVirtualMachine>::const_iterator iter_vm_vec = _request->p_vm_vec->begin();
	for (;iter_vm_vec != _request->p_vm_vec->end();iter_vm_vec++)
	{
		state_cpu_residual[iter_vm_vec->getHostedServID() - 1] -= iter_vm_vec->getcpuRequired();
		state_mem_residual[iter_vm_vec->getHostedServID() - 1] -= iter_vm_vec->getmemRequired();
		state_disk_residual[iter_vm_vec->getHostedServID() - 1] -= iter_vm_vec->getdiskRequired();
	}
	

	for (iter_stat_cpu_residual = state_cpu_residual.begin();iter_stat_cpu_residual != state_cpu_residual.end();iter_stat_cpu_residual++)
	{
		hash = hash * seed + *iter_stat_cpu_residual;
	}
	

	//for(iter_state_mem_residual = state_mem_residual.begin();iter_state_mem_residual != state_mem_residual.end();iter_state_mem_residual++)
	//{
	//	hash = hash * seed + *iter_state_mem_residual;
	//}

	//for (iter_stat_disk_residual = state_disk_residual.begin();iter_stat_disk_residual != state_disk_residual.end();iter_stat_disk_residual++)
	//{
	//	hash = hash * seed + *iter_stat_disk_residual;
	//}
	

	return hash;
}

unsigned long int BKDRHash(const vector<cServer>& _server_vec)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned long int hash = 0;

	vector<unsigned long int> state_cpu_residual;
	vector<unsigned long int> state_mem_residual;
	vector<unsigned long int> state_disk_residual;

	vector<unsigned long int>::iterator iter_stat_cpu_residual;
	vector<unsigned long int>::iterator iter_state_mem_residual;
	vector<unsigned long int>::iterator iter_stat_disk_residual;

	vector<cServer>::const_iterator iter_server_vec = _server_vec.begin();
	for (;iter_server_vec != _server_vec.end();iter_server_vec++)
	{
		state_cpu_residual.push_back(iter_server_vec->getcpuResidual());
		state_mem_residual.push_back(iter_server_vec->getmemResidual());
		state_disk_residual.push_back(iter_server_vec->getdiskResidual());
	}


	for (iter_stat_cpu_residual = state_cpu_residual.begin();iter_stat_cpu_residual != state_cpu_residual.end();iter_stat_cpu_residual++)
	{
		hash = hash * seed + *iter_stat_cpu_residual;
	}


	//for(iter_state_mem_residual = state_mem_residual.begin();iter_state_mem_residual != state_mem_residual.end();iter_state_mem_residual++)
	//{
	//	hash = hash * seed + *iter_state_mem_residual;
	//}

	//for (iter_stat_disk_residual = state_disk_residual.begin();iter_stat_disk_residual != state_disk_residual.end();iter_stat_disk_residual++)
	//{
	//	hash = hash * seed + *iter_stat_disk_residual;
	//}


	return hash;
}

unsigned long int BKDRHashDeparture(const vector<cServer>& _server_vec,const cRequest* _request)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned long int hash = 0;

	vector<unsigned long int> state_cpu_residual;
	vector<unsigned long int> state_mem_residual;
	vector<unsigned long int> state_disk_residual;

	vector<unsigned long int>::iterator iter_stat_cpu_residual;
	vector<unsigned long int>::iterator iter_state_mem_residual;
	vector<unsigned long int>::iterator iter_stat_disk_residual;

	vector<cServer>::const_iterator iter_server_vec = _server_vec.begin();
	for (;iter_server_vec != _server_vec.end();iter_server_vec++)
	{
		state_cpu_residual.push_back(iter_server_vec->getcpuResidual());
		state_mem_residual.push_back(iter_server_vec->getmemResidual());
		state_disk_residual.push_back(iter_server_vec->getdiskResidual());
	}

	vector<cVirtualMachine>::const_iterator iter_vm_vec = _request->p_vm_vec->begin();
	for (;iter_vm_vec != _request->p_vm_vec->end();iter_vm_vec++)
	{
		state_cpu_residual[iter_vm_vec->getHostedServID() - 1] += iter_vm_vec->getcpuRequired();
		state_mem_residual[iter_vm_vec->getHostedServID() - 1] += iter_vm_vec->getmemRequired();
		state_disk_residual[iter_vm_vec->getHostedServID() - 1] += iter_vm_vec->getdiskRequired();
	}


	for (iter_stat_cpu_residual = state_cpu_residual.begin();iter_stat_cpu_residual != state_cpu_residual.end();iter_stat_cpu_residual++)
	{
		hash = hash * seed + *iter_stat_cpu_residual;
	}


	//for(iter_state_mem_residual = state_mem_residual.begin();iter_state_mem_residual != state_mem_residual.end();iter_state_mem_residual++)
	//{
	//	hash = hash * seed + *iter_state_mem_residual;
	//}

	//for (iter_stat_disk_residual = state_disk_residual.begin();iter_stat_disk_residual != state_disk_residual.end();iter_stat_disk_residual++)
	//{
	//	hash = hash * seed + *iter_stat_disk_residual;
	//}


	return hash;
}

unsigned long int BKDRHashArriDepar(const vector<cServer>& _server_vec,const cRequest* _curr_arrive_requ,const cRequest* _pontential_depar_requ)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned long int hash = 0;

	vector<unsigned long int> state_cpu_residual;
	vector<unsigned long int> state_mem_residual;
	vector<unsigned long int> state_disk_residual;

	vector<unsigned long int>::iterator iter_stat_cpu_residual;
	vector<unsigned long int>::iterator iter_state_mem_residual;
	vector<unsigned long int>::iterator iter_stat_disk_residual;

	vector<cServer>::const_iterator iter_server_vec = _server_vec.begin();
	for (;iter_server_vec != _server_vec.end();iter_server_vec++)
	{
		state_cpu_residual.push_back(iter_server_vec->getcpuResidual());
		state_mem_residual.push_back(iter_server_vec->getmemResidual());
		state_disk_residual.push_back(iter_server_vec->getdiskResidual());
	}

	vector<cVirtualMachine>::const_iterator iter_vm_vec = _curr_arrive_requ->p_vm_vec->begin();
	for (;iter_vm_vec != _curr_arrive_requ->p_vm_vec->end();iter_vm_vec++)
	{
		state_cpu_residual[iter_vm_vec->getHostedServID() - 1] -= iter_vm_vec->getcpuRequired();
		state_mem_residual[iter_vm_vec->getHostedServID() - 1] -= iter_vm_vec->getmemRequired();
		state_disk_residual[iter_vm_vec->getHostedServID() - 1] -= iter_vm_vec->getdiskRequired();
	}
		
	iter_vm_vec = _pontential_depar_requ->p_vm_vec->begin();
	for (;iter_vm_vec != _pontential_depar_requ->p_vm_vec->end();iter_vm_vec++)
	{
		state_cpu_residual[iter_vm_vec->getHostedServID() - 1] += iter_vm_vec->getcpuRequired();
		state_mem_residual[iter_vm_vec->getHostedServID() - 1] += iter_vm_vec->getmemRequired();
		state_disk_residual[iter_vm_vec->getHostedServID() - 1] += iter_vm_vec->getdiskRequired();
	}


	for (iter_stat_cpu_residual = state_cpu_residual.begin();iter_stat_cpu_residual != state_cpu_residual.end();iter_stat_cpu_residual++)
	{
		hash = hash * seed + *iter_stat_cpu_residual;
	}


	//for(iter_state_mem_residual = state_mem_residual.begin();iter_state_mem_residual != state_mem_residual.end();iter_state_mem_residual++)
	//{
	//	hash = hash * seed + *iter_state_mem_residual;
	//}

	//for (iter_stat_disk_residual = state_disk_residual.begin();iter_stat_disk_residual != state_disk_residual.end();iter_stat_disk_residual++)
	//{
	//	hash = hash * seed + *iter_stat_disk_residual;
	//}


	return hash;
}

unsigned long int calculateRequestStateIndicator(const vector<cServer>& _server_vec,const cRequest* _request)
{
	vector<cServer>::const_iterator const_iter_server_vec;
	double total_residual = 0;


	//if the request is not intensionally placed, the part of resources occupied by this request will not be calculated.
	if (!_request->getAccepted())
	{
		/*return total_residual;*/
		return BKDRHash(_server_vec);
	}

	return BKDRHash(_server_vec,_request);

	//vector<cVirtualMachine>::const_iterator const_iter_vm_vec;
	//for (const_iter_vm_vec = _request->p_vm_vec->begin();const_iter_vm_vec != _request->p_vm_vec->end();const_iter_vm_vec++)
	//{

	//	total_residual = ((const_iter_vm_vec->getcpuRequired() + const_iter_vm_vec->getmemRequired() + const_iter_vm_vec->getdiskRequired()) * \
	//		(const_iter_vm_vec->getHostedServerPoint())->getID());
	//}

	//return total_residual;
}

unsigned long int calculateRequestDepartureStateIndicator(const vector<cServer>& _server_vec,const cRequest* _request)
{

	return BKDRHashDeparture(_server_vec,_request);

	//vector<cVirtualMachine>::const_iterator const_iter_vm_vec;
	//for (const_iter_vm_vec = _request->p_vm_vec->begin();const_iter_vm_vec != _request->p_vm_vec->end();const_iter_vm_vec++)
	//{

	//	total_residual = ((const_iter_vm_vec->getcpuRequired() + const_iter_vm_vec->getmemRequired() + const_iter_vm_vec->getdiskRequired()) * \
	//		(const_iter_vm_vec->getHostedServerPoint())->getID());
	//}

	//return total_residual;
}

//map the system state into the system state indicator
//it can be calculated by
//sum (cpu_residual + mem_residual + disk_residual) * server_id over all servers
unsigned long int calculateStateIndicator(const vector<cServer>& _server_vec)
{

	return BKDRHash(_server_vec);
}

unsigned long int calcPotentialRequDeparStateIndicator(const vector<cServer>& _server_vec,const cRequest* _curr_arrive_requ,const cRequest* _pontential_depar_requ)
{
	if (_curr_arrive_requ->getAccepted())
	{
		return BKDRHashArriDepar(_server_vec,_curr_arrive_requ,_pontential_depar_requ);
	}
	else
	{
		return BKDRHashDeparture(_server_vec,_pontential_depar_requ);
	}
}

//double calculateStateIndicator(const vector<cServer>& _server_vec)
//{
//	vector<cServer>::const_iterator const_iter_server_vec;
//	double total_residual = 0;
//
//	for (const_iter_server_vec = _server_vec.begin();const_iter_server_vec != _server_vec.end();const_iter_server_vec++)
//	{
//		total_residual += ((const_iter_server_vec->getcpuResidual() + const_iter_server_vec->getmemResidual() + \
//			const_iter_server_vec->getdiskResidual()) * (double)const_iter_server_vec->getID());
//	}
//
//	return total_residual;
//}


double getStateValue(requesttype _request_type,unsigned long int _system_state_indicator)
{
	map<pair<requesttype,unsigned long int>,double>::iterator iter_state_value;

	//iter_state_value = system_state_value_map.find(make_pair(_request_type,_system_state_indicator));
	
	iter_state_value = global_point_system_value_map[counting + 1].find(make_pair(_request_type,_system_state_indicator));

	//if (iter_state_value == system_state_value_map.end())
    if (iter_state_value == global_point_system_value_map[counting + 1].end())
	{
		return 0;
	}

	return iter_state_value->second;
}

double obtainCommuCost(const cRequest* _request)
{
	double requ_commu_cost = 0;
	vector<cVirtualMachine>::const_iterator const_iter_src_vm_vec,const_iter_des_vm_vec;
	const_iter_src_vm_vec = _request->p_vm_vec->begin();

	for (;const_iter_src_vm_vec != _request->p_vm_vec->end();const_iter_src_vm_vec++)
	{
		for (const_iter_des_vm_vec = const_iter_src_vm_vec + 1;const_iter_des_vm_vec != _request->p_vm_vec->end();const_iter_des_vm_vec++)
		{
			requ_commu_cost += commu_cost[const_iter_src_vm_vec->getHostedServID() - 1][const_iter_des_vm_vec->getHostedServID() - 1];
		}
	}

	return requ_commu_cost;
}

double obtainDeploymentProfits(vector<cServer>& _server_vec,cRequest* _request,\
	map<requesttype,unsigned int>& _hosted_requests_type_num_map,\
	map<ID,cRequest*>& _hosted_request_map,\
	double* _hash_current_placement)
{
 	/****************************************************
 	* here the continue Markov chain will be calculated *
 	*****************************************************/

	unsigned long int current_system_state_hash;

	if (system_state.first == NONE)
	{
		current_system_state_hash = calculateStateIndicator(_server_vec);
	}
	else
		current_system_state_hash = calculateRequestStateIndicator(_server_vec,_request);
		
	//return the hash value of current system state such that no calculation is needed again
	*_hash_current_placement = current_system_state_hash;

	//calculate the total transition rate
	double total_transition_rate = 0;
	map<requesttype,pair<double,double>>::iterator iter_request_type = request_type_map.begin();
	for (;iter_request_type != request_type_map.end();iter_request_type++)
	{
		total_transition_rate += iter_request_type->second.first;
	}

	map<requesttype,unsigned int>::iterator iter_hosted_requests_type_num_map = _hosted_requests_type_num_map.begin();
	for (;iter_hosted_requests_type_num_map != _hosted_requests_type_num_map.end();iter_hosted_requests_type_num_map++)
	{
		total_transition_rate += (double)(iter_hosted_requests_type_num_map->second) * (request_type_map[iter_hosted_requests_type_num_map->first]).second;
	}

	//calculate the expected value 
	//firstly, iterate the possible request arrival event
	double state_value = 0;
	map<pair<requesttype,double>,double>::iterator iter_find_state_value;
	iter_request_type = request_type_map.begin();
	for (;iter_request_type != request_type_map.end();iter_request_type++)
	{		
		state_value += iter_request_type->second.first / total_transition_rate * getStateValue(iter_request_type->first,current_system_state_hash);
	}

	//calculate the expected value
	//secondly, iterate the possible request departure event
	map<ID,cRequest*>::iterator iter_hosted_request_map = _hosted_request_map.begin();
	for (;iter_hosted_request_map != _hosted_request_map.end();iter_hosted_request_map++)
	{
		unsigned long int temp_stat_indicator = calcPotentialRequDeparStateIndicator(_server_vec,_request,iter_hosted_request_map->second);
		state_value += request_type_map[iter_hosted_request_map->second->getRequestType()].second / total_transition_rate * getStateValue(NONE,temp_stat_indicator);
	}
	


	if (_request->getAccepted())
	{

		return discout_factor * state_value + _request->getRequestUnitTimeRevenue()  * _request->getDurationTime();	
	}
	else
	{
		return discout_factor * state_value - _request->getRequestUnitTimePenalty() * _request->getDurationTime();
	}

}

static pair<string, pair<double,vector<cServer*>>> choose_policy_epsilon_algorithm(double _epsilon_soft, string _name_optimal_policy,double _max_profit, vector<cServer*>& _optimal_deployment, vector<pair<string, pair<double, vector<cServer*>>>>& _placement_solution)
{
	double random_num;
	unsigned int index_chosen_policy;
	const gsl_rng_type * T;
	gsl_rng * r;

	
	//If the greedy policy instead of the epsilon policy is chosen,
	//directly returns 
	if (mix_solution == 0)
	{
		return make_pair(_name_optimal_policy, make_pair(_max_profit, _optimal_deployment));
	}

	static unsigned long episolon_seed = 0;
	gsl_rng_env_setup();

	gsl_rng_default_seed = episolon_seed++;

	T = gsl_rng_default;
	r = gsl_rng_alloc(T);

	random_num = gsl_rng_uniform(r);

	if (random_num < 1 - _epsilon_soft)
	{
		gsl_rng_free(r);
		return make_pair(_name_optimal_policy,make_pair(_max_profit, _optimal_deployment));
	}

	index_chosen_policy = gsl_rng_uniform_int(r,(unsigned int)_placement_solution.size());
	gsl_rng_free(r);
	return _placement_solution[index_chosen_policy];
}


//obtain the optimal action over the action space (e.g.,whether should accept the arriving request or not and which polity should be take such that optimal
//profit can be obtained)
bool obtainOptimalAction(cEvent* _event,vector<cServer>& _server_vec,
	map<requesttype,unsigned int>& _hosted_requests_type_num_map,\
	map<ID,cRequest*>& _hosted_request_map)
{
	vector<pair<string,placementfunction>>::iterator iter_placement_vec;
	double profit,max_profit = -10000000;

	vector<cServer*> optimal_deployment;
	string name_optimal_policy;

	cRequest* request = _event->getRequest();

	pair<string, pair<double,vector<cServer*>>> chosen_policy;

	vector<pair<string, pair<double,vector<cServer*>>>> placement_solution;

	bool accepted_arriving_request = false;

	bool find_solution = false;
	double hash_current_placement = 0;
	double hash_optimal_placement = 0;

	//For collecting statistic data purpose only.
	bool flag_find_placement = false;

	int iteration_placement = 0;
	for (iter_placement_vec = policy_vec.begin();iter_placement_vec != policy_vec.end();iter_placement_vec++)
	{
		while(vmDeployment(_server_vec,request,*iter_placement_vec,&iteration_placement))
		{						
			//The placement func processes correctly
			//A solution is found including "NO_PLACEMENT" solution
			find_solution = true;

			//Calculate the profit associated with the current policy
			profit = obtainDeploymentProfits(_server_vec,request,_hosted_requests_type_num_map,_hosted_request_map,&hash_current_placement);
			
			//Store the currently found policy to be used in the \epsilon-soft algorithm
			vector<cServer*> tem_hosting_server_vec;
			vector<cVirtualMachine>::iterator tem_iter_vm_vec = request->p_vm_vec->begin();
			for (; tem_iter_vm_vec != request->p_vm_vec->end(); tem_iter_vm_vec++)
			{
				tem_hosting_server_vec.push_back(tem_iter_vm_vec->getHostedServerPoint());
			}
			placement_solution.push_back(make_pair(iter_placement_vec->first, make_pair(profit,tem_hosting_server_vec)));
			
			//Check the profits obtained via the current placement func
			//If it is larger than those achieved for this request before, update the profit and 
			//save the placement solution to be used to allocate resources purpose			
			if(profit > max_profit)
			{
				//found a better placement solution with more profits obtained
				//save the placement solution
				max_profit = profit;
				name_optimal_policy = iter_placement_vec->first;
				//hash_optimal_placement = hash_current_placement;
				optimal_deployment.clear();

				////if the request is intentionally rejected, we just need empty the optimal solution vec;
 			//	if (!request->getAccepted())
				//{
				//	accepted_arriving_request  = false;
				//	//break;
				//}

				vector<cVirtualMachine>::iterator iter_vm_vec = request->p_vm_vec->begin();
				for (;iter_vm_vec != request->p_vm_vec->end();iter_vm_vec++)
				{
					optimal_deployment.push_back(iter_vm_vec->getHostedServerPoint());
				}

			}
			
			//The flag is reset break out the current placement function
			//otherwise rerun the function again
			//NOTE: Currently, it is used only for "OPTIMAL" placement solution
			if (iteration_placement == 0)
			{
				break;
			}
			else
			{
				continue;
			}

		}//end... while(vmDeployment

	}//end... for (iter_placement_vec

	chosen_policy = choose_policy_epsilon_algorithm(epsilon_soft, name_optimal_policy, max_profit, optimal_deployment, placement_solution);
	
	
	//For collecting statistic data purpose only.
	if (chosen_policy.first != "NO_PLACEMENT")
	{
		flag_find_placement = true;
	}
	
	
	//finally found an optimal placement solution
	//update the deployment information for each VM belonging to the current request

	if (chosen_policy.first != "NO_PLACEMENT")
	{			

		//The arrived request is accepted.
		accepted_arriving_request = true;

		request->setAccepted(true);
		vector<cVirtualMachine>::iterator iter_vm_vec = request->p_vm_vec->begin();
		vector<cServer*>::iterator iter_optimal_depolyment_vec = chosen_policy.second.second.begin();
		for (;iter_vm_vec != request->p_vm_vec->end();iter_vm_vec++,iter_optimal_depolyment_vec++)
		{
			iter_vm_vec->setHostedServPoint(*iter_optimal_depolyment_vec);
			iter_vm_vec->setHostedServID((*iter_optimal_depolyment_vec)->getID());
		}

	}

	//If a policy has been found, including "NO_PLACEMENT"
	//update the value function for current system state
	if (find_solution)
	{
		//update state value and corresponding state policy
		
		if (request->getAccepted() == false)
		{
			system_state.first = NONE;
		}

		//set the global system state variable, or system_state

		system_state.second = calculateStateIndicator(_server_vec);

		map<pair<requesttype,unsigned long int>,double>::iterator iter_find_system_state_value_map = global_point_system_value_map[counting].find(make_pair(system_state.first,system_state.second));
		//map<pair<requesttype,unsigned long int>,double>::iterator iter_find_system_state_value_map = system_state_value_map.find(make_pair(system_state.first,system_state.second));
		if (iter_find_system_state_value_map == global_point_system_value_map[counting].end())
		{
			//insert the info of current system state
			global_point_system_value_map[counting].insert(make_pair(make_pair(system_state.first, system_state.second), chosen_policy.second.first));
/*			cPolity policy;
			policy.system_state_policy.insert(make_pair(name_optimal_policy,1));
			global_point_system_policy_map[counting].insert(make_pair(make_pair(system_state.first,system_state.second),policy));	*/	
		}
		else
		{
			iter_find_system_state_value_map->second = (1 - value_function_update_factor)* iter_find_system_state_value_map->second + value_function_update_factor * chosen_policy.second.first;


			//map<pair<requesttype,unsigned long int>,cPolity>::iterator iter_find_system_state_policy_map = global_point_system_policy_map[counting].find(make_pair(system_state.first,system_state.second));
			//if (iter_find_system_state_policy_map == global_point_system_policy_map[counting].end())
			//{
			//	cout<<"Error!!!Can not locate the policy for current state!!!"<<endl;
			//	exit(0);
			//}

			//map<string,int>::iterator iter_find_state_policy;
			//iter_find_state_policy = iter_find_system_state_policy_map->second.system_state_policy.find(name_optimal_policy);
			//if (iter_find_state_policy == iter_find_system_state_policy_map->second.system_state_policy.end())
			//{
			//	iter_find_system_state_policy_map->second.system_state_policy.insert(make_pair(name_optimal_policy,1));
			//}
			//else
			//{
			//	(iter_find_state_policy->second)++;
			//}
		}
	}//end...if(find_solutino)

	if (!flag_find_placement)
	{
		allocation_fail_num++;
	}
	
	//system_state.second = calculateRequestStateIndicator(_server_vec,request);
	return accepted_arriving_request;
}

void updateStateValueRequDepar(const vector<cServer>& _server_vec,const map<ID,cRequest*>& _hosted_request_map,const map<requesttype,unsigned int>& _hosted_requests_type_num_map)
{
	//calculate the value for "NONE" state
	unsigned long int current_system_state_hash;
	if (system_state.first == NONE)
	{
		current_system_state_hash = calculateStateIndicator(_server_vec);
	}
	else
		cout<<"Error!!! Processing an unexpected event!"<<endl;

	//return the hash value of current system state such that no calculation is needed again
	system_state.second = current_system_state_hash;

	//calculate the total transition rate
	double total_transition_rate = 0;
	map<requesttype,pair<double,double>>::iterator iter_request_type = request_type_map.begin();
	for (;iter_request_type != request_type_map.end();iter_request_type++)
	{
		total_transition_rate += iter_request_type->second.first;
	}

	map<requesttype,unsigned int>::const_iterator iter_hosted_requests_type_num_map = _hosted_requests_type_num_map.begin();
	for (;iter_hosted_requests_type_num_map != _hosted_requests_type_num_map.end();iter_hosted_requests_type_num_map++)
	{
		total_transition_rate += (double)(iter_hosted_requests_type_num_map->second) * (request_type_map[iter_hosted_requests_type_num_map->first]).second;
	}

	//calculate the expected value 
	//firstly, iterate the possible request arrival event
	double state_value = 0;
	map<pair<requesttype,unsigned long int>,double>::iterator iter_find_state_value;
	iter_request_type = request_type_map.begin();
	for (;iter_request_type != request_type_map.end();iter_request_type++)
	{		
		state_value += iter_request_type->second.first / total_transition_rate * getStateValue(iter_request_type->first,current_system_state_hash);
	}

	//calculate the expected value
	//secondly, iterate the possible request departure event
	map<ID,cRequest*>::const_iterator iter_hosted_request_map = _hosted_request_map.begin();
	for (;iter_hosted_request_map != _hosted_request_map.end();iter_hosted_request_map++)
	{
		unsigned long int temp_stat_indicator = calculateRequestDepartureStateIndicator(_server_vec,iter_hosted_request_map->second);
		state_value += request_type_map[iter_hosted_request_map->second->getRequestType()].second / total_transition_rate * getStateValue(NONE,temp_stat_indicator);
	}

	state_value = state_value * discout_factor;


	//update state value and corresponding state policy
	map<pair<requesttype,unsigned long int>,double>::iterator iter_find_system_state_value_map = global_point_system_value_map[counting].find(make_pair(system_state.first,system_state.second));
	if (iter_find_system_state_value_map == global_point_system_value_map[counting].end())
	{
		//insert the info of current system state
		system_state_value_map.insert(make_pair(make_pair(system_state.first,system_state.second),state_value));	
	}
	else
	{
		iter_find_system_state_value_map->second = (1 - value_function_update_factor)* iter_find_system_state_value_map->second + value_function_update_factor * state_value;
	}
	
	return ;
}

void obtainOptimalStateValue(multimap<double,cEvent>& _event_multimap,vector<cServer>& _server_vec)
{
	multimap<double,cEvent>::iterator iter_event_multimap;
	counting = -1;

	//store the number of accepted requests corresponding to each type of requests
	map<requesttype,unsigned int> hosted_requests_type_num_map;
	map<requesttype,unsigned int>::iterator iter_find_hosted_request_num_map;
	
	map<ID,cRequest*>  hosted_request_map;
	map<ID,cRequest*>::iterator iter_find_request_map;

	
	for (iter_event_multimap = _event_multimap.begin();iter_event_multimap != _event_multimap.end();iter_event_multimap++)
	{		
		if (iter_event_multimap->second.getEventType() == DEPARTURE)
		{
			//set current time
			current_time = iter_event_multimap->first;
			
			//delete the request from the hosting list
			iter_find_request_map = hosted_request_map.find((iter_event_multimap->second.getRequest())->getID());

			if (iter_find_request_map == hosted_request_map.end())
			{
				cout<<"Error!!!Can not locate the request that should exist in the hosting list!"<<endl;
				exit(0);
			}

			hosted_request_map.erase(iter_find_request_map);

			//minus 1 from the entry corresponding the type of currently departing requests in the counting list of hosting requests
			iter_find_hosted_request_num_map = hosted_requests_type_num_map.find((iter_event_multimap->second.getRequest())->getRequestType());
			if (iter_find_hosted_request_num_map == hosted_requests_type_num_map.end())
			{
				cout<<"Error!!!Can not locate the request that should exist in the counting list of types of the hosting requests!"<<endl;
				exit(0);
			}
			else
				(iter_find_hosted_request_num_map->second)--;

			//release the resources allocated to the request
			releaseRequest(iter_event_multimap->second.getRequest());


			//update the value of current system state after any request departure
			//system_state.first = NONE;
			//updateStateValueRequDepar(_server_vec,hosted_request_map,hosted_requests_type_num_map);
			//system_state.second = calculateRequestDepartureStateIndicator(_server_vec,iter_event_multimap->second.getRequest());
		}
		else
		{
			//set current time
			current_time = iter_event_multimap->first;
			
			//a request is arriving
			counting++;
			//update the system state
			system_state.first = (iter_event_multimap->second.getRequest())->getRequestType();

			//we should determine which action should be take base on the purpose of maximizing the expected profits
			//if(obtainOptimalAction(&(iter_event_multimap->second),_server_vec,hosted_requests_type_num_map,hosted_request_map))
            if(obtainOptimalAction(&(iter_event_multimap->second),_server_vec,hosted_requests_type_num_map,hosted_request_map))
			{
				cRequest* tem_p_request = iter_event_multimap->second.getRequest();
				total_revenue += tem_p_request->getRequestUnitTimeRevenue() * tem_p_request->getDurationTime();
				
				//if the arriving request is accepted, we should allocate physical resources for it and insert a departure event into the event list
				tem_p_request->setAccepted(true);
				allocateRequest(tem_p_request);
				insertDepartureEvent(tem_p_request, _event_multimap);
				accepted_requests_num++;

				hosted_request_map.insert(make_pair(tem_p_request->getID(), tem_p_request));

				iter_find_hosted_request_num_map = hosted_requests_type_num_map.find((iter_event_multimap->second.getRequest())->getRequestType());
				if (iter_find_hosted_request_num_map == hosted_requests_type_num_map.end())
				{
					//there is no request in the system which has the same type as currently arriving request
					hosted_requests_type_num_map.insert(make_pair(tem_p_request->getRequestType(), 1));
				}
				else
					(iter_find_hosted_request_num_map->second)++;
				
			}
			else
			{
				
				cRequest* tem_p_request = iter_event_multimap->second.getRequest();
				total_revenue -= tem_p_request->getRequestUnitTimePenalty() * tem_p_request->getDurationTime();
				//do nothing
				//the arriving request is rejected due to 1) having not enough residual resources, or 2) maximizing profits policy
				tem_p_request->setAccepted(false);

				//update the value of current system state after any request departure
				system_state.first = NONE;
				updateStateValueRequDepar(_server_vec,hosted_request_map,hosted_requests_type_num_map);
			}

		}
	}

	return ;
}


void initialCommuCost(const vector<cServer>& _server_vec)
{
	vector<cServer>::const_iterator const_iter_src_server_vec,const_iter_des_server_vec;
	div_t tem_src,tem_des;
	ID src_server_id,des_server_id;
	for (const_iter_src_server_vec = _server_vec.begin();const_iter_src_server_vec != _server_vec.end();const_iter_src_server_vec++)
	{
		vector<double> commu_cost_vec;
		for (const_iter_des_server_vec = _server_vec.begin();const_iter_des_server_vec != _server_vec.end();const_iter_des_server_vec++)
		{
			src_server_id = const_iter_src_server_vec->getID() - 1;
			des_server_id = const_iter_des_server_vec->getID() - 1;

			tem_src = div(src_server_id,2);
			tem_des = div(des_server_id,2);

			if (tem_src.quot == tem_des.quot)
			{
				if (tem_src.rem == tem_des.rem)
				{
					//It's the communication cost on local server
					commu_cost_vec.push_back(local_communication_cost);
				}
				else
				{
					//It's the communication cost between two servers that locate on different servers connected to the same tor
					commu_cost_vec.push_back(tor_communication_cost);
				}
			}
			else
			{
				//It's the communication cost between two servers that locate on different tor
				commu_cost_vec.push_back(remote_communication_cost);
			}
		}

		commu_cost.push_back(commu_cost_vec);
	}
	
	return ;
}

void outputResults()
{
	ofstream output_file;

	output_file.open("output.txt",ios::app);
	double state_value;
	double expected_state_value = 0;
	double total_transition_rate = 0;

	//output_file<<"The workload is "<<workload_rate<<"The policy is "<<policy_indicator<<endl;
	output_file<<workload_rate<<" "<<policy_indicator;

	map<pair<requesttype,unsigned long int>,double>::iterator iter_find_system_state_value_map;
	map<pair<requesttype,unsigned long int>,cPolity>::iterator iter_find_system_state_policy_map;

	map<requesttype,pair<double,double>>::iterator iter_request_type_map = request_type_map.begin();
	for (;iter_request_type_map != request_type_map.end();iter_request_type_map++)
	{
		state_value = 0;
		iter_find_system_state_value_map = global_point_system_value_map[0].find(make_pair(iter_request_type_map->first,initial_system_state_indicator));
		//iter_find_system_state_value_map = system_state_value_map.find(make_pair(iter_request_type_map->first,initial_system_state_indicator));
		if (iter_find_system_state_value_map != global_point_system_value_map[0].end())
		{
			state_value = iter_find_system_state_value_map->second;
		}
		//output_file<<"<"<<iter_request_type_map->first<<","<<state_value<<">"<<endl;
		output_file<<" "<<state_value;
		expected_state_value += state_value * iter_request_type_map->second.first;
		total_transition_rate += iter_request_type_map->second.first;

		//iter_find_system_state_policy_map = global_point_system_policy_map[0].find(make_pair(iter_request_type_map->first,initial_system_state_indicator));
		
		
		//iter_find_system_state_policy_map = system_state_policy_map.find(make_pair(iter_request_type_map->first,initial_system_state_indicator));
		//if (iter_find_system_state_policy_map != global_point_system_policy_map[0].end())
		//{
		//	map<string,int>::iterator iter_state_policy = iter_find_system_state_policy_map->second.system_state_policy.begin();
		//	for (;iter_state_policy != iter_find_system_state_policy_map->second.system_state_policy.end();iter_state_policy++)
		//	{
		//		output_file<<iter_state_policy->first<<"--"<<iter_state_policy->second<<endl;
		//	}
		//}
		//output_file<<"\n"<<endl;
	}
	
	//output the expected value
	//output_file<<" "<<expected_state_value/total_transition_rate;
	output_file << " " << total_revenue / sample_request_num;
	//output_file<<"The average number of accepted requests is "<<average_accepted_rate/sample_request_num<<endl;
	output_file<<" "<<average_accepted_rate/sample_request_num<<" "<<average_allocation_fail_rate/sample_request_num<<endl;
	
	output_file.close();
	return ;
}

bool initialInputfile(vector<cConfiguration>& _config_vec)
{
	ifstream inputFile("input.txt");

	_config_vec.clear();

	char workload[20];
	char low_rate[20];
	char high_rate[20];
	char xhigh_rate[20];
	char policy[20];
	char num_sample_path[20];
	char num_requests[20];


	char buf[101];

	if (!inputFile)
	{
		cout<<"Unable to open input file!"<<endl;
		return false;
	}

	while(!inputFile.eof())
	{
		inputFile.getline(buf,100,'\n');
		sscanf(buf,"%s %s %s %s %s %s %s\n",workload, low_rate,high_rate,xhigh_rate,policy,num_sample_path,num_requests);
		_config_vec.push_back(cConfiguration(atof(workload),atof(low_rate),atof(high_rate),atof(xhigh_rate),atof(policy),atof(num_sample_path),atof(num_requests)));
	}

	return true;
}

void initialInputParameter(vector<cConfiguration>::iterator _iter_config_vec)
{

	workload_rate = _iter_config_vec->getWorkLoadRate();
	low_arrival_rate = _iter_config_vec->getLowArrivalRate();
	hight_arrival_rate = _iter_config_vec->getHighArrivalRate();
	xhight_arrival_rate = _iter_config_vec->getXhighArrivalRate();

	policy_indicator = _iter_config_vec->getPolicy();
	initialPolicies(policy_indicator);	

	sample_request_num = _iter_config_vec->getNumSamplePath();
	total_request = _iter_config_vec->getNumRequest();
	return;
}