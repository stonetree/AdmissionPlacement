#include "stdafx.h"
#include "cServer.h"
#include "cService.h"
#include "cEvent.h"
#include "cRequest.h"

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

void calculateStateValue(multimap<double,cEvent>& _event_multimap,vector<cRequest>& _request_vec,vector<cServer>& _server_vec,map<double,double>& _state_value)
{
	multimap<double,cEvent>::iterator iter_event_multimap;
	for (iter_event_multimap = _event_multimap.begin();iter_event_multimap != _event_multimap.end();iter_event_multimap++)
	{

	}
	
	
	
	return ;
}