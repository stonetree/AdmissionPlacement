#include "StdAfx.h"
#include "cRequest.h"


cRequest::cRequest(void)
{
}

cRequest::cRequest(ID _id, double _lambda,double _mu,double _start_time,double _duration_time, double _departure_time,servicetype _service_type,\
	const map<servicetype,cService*>& _service_type_map,const map<VMtype,cBaseVM>& _base_vm_map)
{
	id = _id;
	start_time = _start_time;
	arrival_rate = _lambda;
	departure_rate = _mu;
	duration_time = _duration_time;
	departure_time = _departure_time;
	service_type = _service_type;

	map<VMtype,int>::const_iterator const_iter_vm_type;
	map<VMtype,cBaseVM>::const_iterator const_find_vm_type;
	map<servicetype,cService*>::const_iterator const_find_service_type;

	switch (service_type)
	{
	case LIGHT:
		const_find_service_type = _service_type_map.find(LIGHT);
		break;
	case MIDDLE:		
		const_find_service_type = _service_type_map.find(MIDDLE);
		break;
	case HEAVY:
		const_find_service_type = _service_type_map.find(HEAVY);
		break;
	}

	if (const_find_service_type == _service_type_map.end())
	{
		
		cout<<"It's looking for "<<const_find_service_type->first<<" service type\n"<<endl;
		cout<<"Error!!! There is no specified service type existed!\n"<<endl;
		exit(0);
	}

	for (const_iter_vm_type = const_find_service_type->second->vm_request_map.begin();const_iter_vm_type != const_find_service_type->second->vm_request_map.end();const_iter_vm_type++)
	{
		int i = 0;
		ID id = 1;
		for (i = 0;i < const_iter_vm_type->second;i++,id++)
		{
			const_find_vm_type = _base_vm_map.find(const_iter_vm_type->first);
			if (const_find_vm_type == _base_vm_map.end())
			{
				cout<<"It's looking for "<<const_iter_vm_type->first<<" vm type\n"<<endl;
				cout<<"Error!! There is no vm type existing!\n"<<endl;
				exit(0);
			}
			vm_vec.push_back(cVirtualMachine(id,0,0,NULL,NULL,const_find_vm_type->first,const_find_vm_type->second));
		}
	}
}

cRequest::cRequest(const cRequest& _requ)
{
	operator=(_requ);
}

cRequest& cRequest::operator=(const cRequest& _requ)
{
	if (this != &_requ)
	{
		id = _requ.id;
		arrival_rate = _requ.arrival_rate;
		departure_rate = _requ.departure_rate;
		start_time = _requ.start_time;
		duration_time = _requ.duration_time;
		departure_time = _requ.departure_time;
		service_type = _requ.service_type;

		vm_vec.clear();
		vm_vec.assign(_requ.vm_vec.begin(),_requ.vm_vec.end());
	}

	return *this;
}

cRequest::~cRequest(void)
{
}
