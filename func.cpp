#include "stdafx.h"
#include "cServer.h"
#include "cService.h"
#include "cEvent.h"
#include "cRequest.h"
#include "common.h"


static map<requesttype,unsigned int> hosted_requests_type_num;

void iterateStateValue(map<double,double>& _state_value)
{
	return;
}

double getStateValue(double _state_indicator,map<double,double>& _state_value)
{
	map<double,double>::iterator iter_state_value;

	iter_state_value = _state_value.find(_state_indicator);

	if (iter_state_value == _state_value.end())
	{
		//the specified state has not visited yet
		//add current state indicator and state value into the state value map
		_state_value.insert(make_pair(_state_indicator,0.0));
		return 0;
	}
	
	return iter_state_value->second;
}

double calculateStateIndicator(const vector<cServer>& _server_vec,cRequest* _p_request)
{
	vector<cServer>::const_iterator const_iter_server_vec;
	double total_residual = 0;

	for (const_iter_server_vec = _server_vec.begin();const_iter_server_vec != _server_vec.end();const_iter_server_vec++)
	{
		total_residual += ((const_iter_server_vec->getcpuResidual() + const_iter_server_vec->getmemResidual() + \
			const_iter_server_vec->getdiskResidual()) * (double)const_iter_server_vec->getID());
	}
	
	vector<cVirtualMachine>::iterator iter_vm_vec;
	for (iter_vm_vec = _p_request->vm_vec.begin();iter_vm_vec != _p_request->vm_vec.end();iter_vm_vec++)
	{
		total_residual -= ((iter_vm_vec->getcpuRequired() + iter_vm_vec->getmemRequired() + iter_vm_vec->getdiskRequired()) * \
			(iter_vm_vec->getHostedServerPoint())->getID());
	}
	
	return 0;
}

void allocateRequest(cRequest* _p_request)
{
	vector<cVirtualMachine>::iterator iter_vm_vec;
	for (iter_vm_vec = _p_request->vm_vec.begin();iter_vm_vec != _p_request->vm_vec.end();iter_vm_vec++)
	{
		(iter_vm_vec->getHostedServerPoint())->allocateVirtMach(&(*iter_vm_vec));
	}
	return ;
}

void releaseRequest(cRequest* _p_request)
{
	vector<cVirtualMachine>::iterator iter_vm_vec;
	for (iter_vm_vec = _p_request->vm_vec.begin();iter_vm_vec != _p_request->vm_vec.end();iter_vm_vec++)
	{
		(iter_vm_vec->getHostedServerPoint())->releaseVirtMach(&(*iter_vm_vec));
	}	
	return ;
}

//obtain the optimal action over the action space (e.g.,whether should accept the arriving request or not and which polity should be take such that optimal
//profit can be obtained)
bool obtainOptimalAction(multimap<double,cEvent>& _event_multimap,multimap<double,cEvent>::iterator _iter_current,vector<cRequest>& _request_vec,vector<cServer>& _server_vec,vector<pair<placementfunction,int>>& _placement_func_vec,map<double,double>& _state_value)
{
	vector<pair<placementfunction,int>>::iterator iter_placement_vec;
	double max_profit;

	for (iter_placement_vec = _placement_func_vec.begin();iter_placement_vec != _placement_func_vec.end();iter_placement_vec++)
	{
		//obtainDeploymentProfits(_server_vec,_request,iter_placement_vec->first);
	}

	//the arriving will not be accepted
	return false;
}

void obtainOptimalStateValue(multimap<double,cEvent>& _event_multimap,vector<cRequest>& _request_vec,vector<cServer>& _server_vec,vector<pair<placementfunction,int>>& _placement_func,map<double,double>& _state_value)
{
	multimap<double,cEvent>::iterator iter_event_multimap;
	
	hosted_requests_type_num.clear();
	map<requesttype,unsigned int>::iterator iter_find_hosted_request_num_map;

	for (iter_event_multimap = _event_multimap.begin();iter_event_multimap != _event_multimap.end();iter_event_multimap++)
	{
		if (iter_event_multimap->second.getEventType() == DEPARTURE)
		{
			releaseRequest(iter_event_multimap->second.getRequest());
		}
		else
		{
			//a request arriving
			//we should determine which action should be take base on the purpose of maximizing the expected profits
			if(obtainOptimalAction(_event_multimap,iter_event_multimap,_request_vec,_server_vec,_placement_func,_state_value))
			{
				//if the arriving request is accepted, we should allocate physical resources for it and insert a departure event into the event list
				allocateRequest(iter_event_multimap->second.getRequest());
				insertDepartureEvent(iter_event_multimap->second.getRequest(),_event_multimap);

				iter_find_hosted_request_num_map = hosted_requests_type_num.find((iter_event_multimap->second.getRequest())->getRequestType());
				if (iter_find_hosted_request_num_map == hosted_requests_type_num.end())
				{
					//there is no request in the system which has the same type as currently arriving request
					hosted_requests_type_num.insert(make_pair((iter_event_multimap->second.getRequest())->getRequestType(),1));
				}
				else
					(iter_find_hosted_request_num_map->second)++;
				
			}

		}
	}

	return ;
}