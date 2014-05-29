#include "stdafx.h"
#include "cServer.h"
#include "cService.h"
#include "cEvent.h"
#include "cRequest.h"
#include "common.h"
#include "cPolity.h"

const double discout_factor = 0.8;
const double value_function_update_factor = 0.5;

pair<requesttype,double> system_state;

void iterateStateValue(map<double,double>& _state_value)
{
	return;
}


//if the arriving request is accepted,then we should allocate resources to it
void allocateRequest(cRequest* _p_request)
{
	vector<cVirtualMachine>::iterator iter_vm_vec;
	for (iter_vm_vec = _p_request->vm_vec.begin();iter_vm_vec != _p_request->vm_vec.end();iter_vm_vec++)
	{
		(iter_vm_vec->getHostedServerPoint())->allocateVirtMach(&(*iter_vm_vec));
	}
	return ;
}

//release resources for those departing requests 
void releaseRequest(cRequest* _p_request)
{
	vector<cVirtualMachine>::iterator iter_vm_vec;
	for (iter_vm_vec = _p_request->vm_vec.begin();iter_vm_vec != _p_request->vm_vec.end();iter_vm_vec++)
	{
		(iter_vm_vec->getHostedServerPoint())->releaseVirtMach(&(*iter_vm_vec));
	}	
	return ;
}

bool vmDeployment(vector<cServer>& _server_vec,cRequest* _request,pair<string,placementfunction>& _placement_func_pair)
{
	bool is_accepted;
	
	_request->setAccepted(false);
	vector<cVirtualMachine>::iterator iter_vm_vec = _request->vm_vec.begin();
	for (;iter_vm_vec != _request->vm_vec.end();iter_vm_vec++)
	{
		iter_vm_vec->setHostedServID(0);
		iter_vm_vec->setHostedServPoint(NULL);
	}
	
	is_accepted = (_placement_func_pair.second)(_server_vec,_request);
	if (!is_accepted && _placement_func_pair.first == "NO_PLACEMENT")
	{
		return true;		
	}

	return is_accepted;
}

double calculateRequestStateIndicator(const cRequest* _request)
{
	vector<cServer>::const_iterator const_iter_server_vec;
	double total_residual = 0;


	//if the request is not intensionally placed, the part of resources occupied by this request will not be calculated.
	if (!_request->getAccepted())
	{
		return total_residual;
	}

	vector<cVirtualMachine>::const_iterator const_iter_vm_vec;
	for (const_iter_vm_vec = _request->vm_vec.begin();const_iter_vm_vec != _request->vm_vec.end();const_iter_vm_vec++)
	{

		total_residual = ((const_iter_vm_vec->getcpuRequired() + const_iter_vm_vec->getmemRequired() + const_iter_vm_vec->getdiskRequired()) * \
			(const_iter_vm_vec->getHostedServerPoint())->getID());
	}

	return total_residual;
}

//map the system state into the system state indicator
//it can be calculated by
//sum (cpu_residual + mem_residual + disk_residual) * server_id over all servers
double calculateStateIndicator(const vector<cServer>& _server_vec)
{
	vector<cServer>::const_iterator const_iter_server_vec;
	double total_residual = 0;

	for (const_iter_server_vec = _server_vec.begin();const_iter_server_vec != _server_vec.end();const_iter_server_vec++)
	{
		total_residual += ((const_iter_server_vec->getcpuResidual() + const_iter_server_vec->getmemResidual() + \
			const_iter_server_vec->getdiskResidual()) * (double)const_iter_server_vec->getID());
	}

	return total_residual;
}


double getStateValue(requesttype _request_type,double _system_state_indicator)
{
	map<pair<requesttype,double>,double>::iterator iter_state_value;

	iter_state_value = system_state_value_map.find(make_pair(_request_type,_system_state_indicator));

	if (iter_state_value == system_state_value_map.end())
	{
		return 0;
	}

	return iter_state_value->second;
}


double obtainDeploymentProfits(vector<cServer>& _server_vec,cRequest* _request,\
	map<requesttype,unsigned int>& _hosted_requests_type_num_map,\
	map<ID,cRequest*>& _hosted_request_map)
{
 	/****************************************************
 	* here the continue Markov chain will be calculated *
 	*****************************************************/

	double current_request_indicator;
	if (system_state.first == NONE)
	{
		current_request_indicator = system_state.second;
	}
	else
		current_request_indicator = system_state.second - calculateRequestStateIndicator(_request);
		
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
		state_value += iter_request_type->second.first / total_transition_rate * getStateValue(iter_request_type->first,current_request_indicator);
	}

	//calculate the expected value
	//secondly, iterate the possible request departure event
	map<ID,cRequest*>::iterator iter_hosted_request_map = _hosted_request_map.begin();
	for (;iter_hosted_request_map != _hosted_request_map.end();iter_hosted_request_map++)
	{
		state_value += request_type_map[iter_hosted_request_map->second->getRequestType()].second / total_transition_rate * getStateValue(NONE,current_request_indicator);
	}
	
	if (_request->getAccepted())
	{
		
		return state_value + (service_type_map[_request->getServiceType()])->getUnitReward() * _request->getDurationTime();	
	}
	else
	{
		return state_value - (service_type_map[_request->getServiceType()])->getUnitPenalty() * _request->getDurationTime();
	}
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

	bool accepted_arriving_request = false;


	for (iter_placement_vec = policy_vec.begin();iter_placement_vec != policy_vec.end();iter_placement_vec++)
	{
		if (vmDeployment(_server_vec,request,*iter_placement_vec))
		{
			//the placement func processes correctly


			//check whether profits under the current placement func
			//if larger profit can be obtained, update the profit and save the placement solution to be used to allocate resources
			profit = obtainDeploymentProfits(_server_vec,request,_hosted_requests_type_num_map,_hosted_request_map);
			if(profit > max_profit)
			{
				//find a better placement solution with more profits to be obtained
				//save the placement solution
				max_profit = profit;
				name_optimal_policy = iter_placement_vec->first;
				optimal_deployment.clear();

				//if the request is intentionally rejected, we just need empty the optimal solution vec;
				if (!request->getAccepted())
				{
					continue;
				}

				vector<cVirtualMachine>::iterator iter_vm_vec = request->vm_vec.begin();
				for (;iter_vm_vec != request->vm_vec.end();iter_vm_vec++)
				{
					optimal_deployment.push_back(iter_vm_vec->getHostedServerPoint());
				}

			}
		}
	}

	//finally find a optimal placement solution
	if (!optimal_deployment.empty() && name_optimal_policy != "NO_PLACEMENT")
	{			
		request->setAccepted(true);
		vector<cVirtualMachine>::iterator iter_vm_vec = request->vm_vec.begin();
		vector<cServer*>::iterator iter_optimal_depolyment_vec = optimal_deployment.begin();
		for (;iter_vm_vec != request->vm_vec.end();iter_vm_vec++,iter_optimal_depolyment_vec++)
		{
			iter_vm_vec->setHostedServPoint(*iter_optimal_depolyment_vec);
			iter_vm_vec->setHostedServID((*iter_optimal_depolyment_vec)->getID());
		}

		accepted_arriving_request  = true;
	}

	//update state value and corresponding state policy
	map<pair<requesttype,double>,double>::iterator iter_find_system_state_value_map = system_state_value_map.find(make_pair(system_state.first,system_state.second));
	if (iter_find_system_state_value_map == system_state_value_map.end())
	{
		//insert the info of current system state
		system_state_value_map.insert(make_pair(make_pair(system_state.first,system_state.second),max_profit));
		cPolity policy;
		policy.system_state_policy.insert(make_pair(name_optimal_policy,1));
		system_state_policy_map.insert(make_pair(make_pair(system_state.first,system_state.second),policy));		
	}
	else
	{
		iter_find_system_state_value_map->second = (1 - value_function_update_factor)* iter_find_system_state_value_map->second + value_function_update_factor * max_profit;


		map<pair<requesttype,double>,cPolity>::iterator iter_find_system_state_policy_map = system_state_policy_map.find(make_pair(system_state.first,system_state.second));
		if (iter_find_system_state_policy_map == system_state_policy_map.end())
		{
			cout<<"Error!!!Can not locate the policy for current state!!!\n"<<endl;
			exit(0);
		}

		map<string,int>::iterator iter_find_state_policy;
		iter_find_state_policy = iter_find_system_state_policy_map->second.system_state_policy.find(name_optimal_policy);
		if (iter_find_state_policy == iter_find_system_state_policy_map->second.system_state_policy.end())
		{
			iter_find_system_state_policy_map->second.system_state_policy.insert(make_pair(name_optimal_policy,1));
		}
		else
		{
			(iter_find_state_policy->second)++;
		}
	}

	
	
	system_state.second -= calculateRequestStateIndicator(request);

	//the arriving will not be accepted
	return accepted_arriving_request;
}

void obtainOptimalStateValue(multimap<double,cEvent>& _event_multimap,vector<cServer>& _server_vec)
{
	multimap<double,cEvent>::iterator iter_event_multimap;

	//store the number of accepted requests corresponding to each type of requests
	map<requesttype,unsigned int> hosted_requests_type_num_map;
	map<requesttype,unsigned int>::iterator iter_find_hosted_request_num_map;
	
	map<ID,cRequest*>  hosted_request_map;
	map<ID,cRequest*>::iterator iter_find_request_map;

	
	for (iter_event_multimap = _event_multimap.begin();iter_event_multimap != _event_multimap.end();iter_event_multimap++)
	{		
		if (iter_event_multimap->second.getEventType() == DEPARTURE)
		{
			//delete the request from the hosting list
			iter_find_request_map = hosted_request_map.find((iter_event_multimap->second.getRequest())->getID());

			if (iter_find_request_map == hosted_request_map.end())
			{
				cout<<"Error!!!Can not locate the request that should exist in the hosting list!\n"<<endl;
				exit(0);
			}

			hosted_request_map.erase(iter_find_request_map);

			//minus 1 from the entry corresponding the type of currently departing requests in the counting list of hosting requests
			iter_find_hosted_request_num_map = hosted_requests_type_num_map.find((iter_event_multimap->second.getRequest())->getRequestType());
			if (iter_find_hosted_request_num_map == hosted_requests_type_num_map.end())
			{
				cout<<"Error!!!Can not locate the request that should exist in the counting list of types of the hosting requests!\n"<<endl;
				exit(0);
			}
			else
				(iter_find_hosted_request_num_map->second)--;
			
			system_state.first = NONE;
			system_state.second += calculateRequestStateIndicator(iter_event_multimap->second.getRequest());

			//release the resources allocated to the request
			releaseRequest(iter_event_multimap->second.getRequest());
		}
		else
		{
			//a request arriving
			//update the system state
			system_state.first = (iter_event_multimap->second.getRequest())->getRequestType();

			//we should determine which action should be take base on the purpose of maximizing the expected profits
			if(obtainOptimalAction(&(iter_event_multimap->second),_server_vec,hosted_requests_type_num_map,hosted_request_map))
			{
				//if the arriving request is accepted, we should allocate physical resources for it and insert a departure event into the event list
				(iter_event_multimap->second.getRequest())->setAccepted(true);
				allocateRequest(iter_event_multimap->second.getRequest());
				insertDepartureEvent(iter_event_multimap->second.getRequest(),_event_multimap);

				hosted_request_map.insert(make_pair((iter_event_multimap->second.getRequest())->getID(),iter_event_multimap->second.getRequest()));

				iter_find_hosted_request_num_map = hosted_requests_type_num_map.find((iter_event_multimap->second.getRequest())->getRequestType());
				if (iter_find_hosted_request_num_map == hosted_requests_type_num_map.end())
				{
					//there is no request in the system which has the same type as currently arriving request
					hosted_requests_type_num_map.insert(make_pair((iter_event_multimap->second.getRequest())->getRequestType(),1));
				}
				else
					(iter_find_hosted_request_num_map->second)++;
				
			}
			else
			{
				//do nothing
				//the arriving request is rejected due to 1) having not enough residual resources, or 2) the maximizing profits policy
				(iter_event_multimap->second.getRequest())->setAccepted(false);
			}

		}
	}

	return ;
}