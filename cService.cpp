#include "StdAfx.h"
#include "cService.h"


cService::cService(void)
{
}


cService::~cService(void)
{
}

cService::cService(servicetype _type)
{
	service_type = _type;
}

cService::cService(servicetype _type,vector<pair<VMtype,int>>& _vm_request_vec,double _unit_reward,double _unit_penalty)
{
	vector<pair<VMtype,int>>::iterator iter_vm_request_vec;
	service_type = _type;
	unit_reward = _unit_reward;
	unit_penalty = _unit_penalty;

	for (iter_vm_request_vec = _vm_request_vec.begin();iter_vm_request_vec != _vm_request_vec.end();iter_vm_request_vec++)
	{
		vm_request_map.insert(make_pair(iter_vm_request_vec->first,iter_vm_request_vec->second));
	}
}

cService::cService(const cService& _service)
{
	this->operator=(_service);
}

cService& cService::operator=(const cService& _service)
{
	if (this != &_service)
	{
		service_type = _service.service_type;
		unit_reward = _service.unit_reward;
		unit_penalty = _service.unit_penalty;

		vm_request_map.clear();
		vm_request_map.insert(_service.vm_request_map.begin(),_service.vm_request_map.end());
	}

	return *this;
}