#include "StdAfx.h"
#include "cRequest.h"


cRequest::cRequest(void)
{
}

cRequest::cRequest(ID _id, double _lambda,double _mu,double _start_time,double _duration_time, double _departure_time,requesttype _request_type,\

	servicetype _service_type,const map<servicetype,cService*>& _service_type_map,const map<VMtype,cBaseVM>& _base_vm_map,unsigned long int _required_service_num,bool _is_accepted)
{
	id = _id;
	is_accepted = _is_accepted;
	start_time = _start_time;
	arrival_rate = _lambda;
	departure_rate = _mu;
	duration_time = _duration_time;
	departure_time = _departure_time;
	service_type = _service_type;
	request_type = _request_type;

	request_unit_time_revenue = 0;
	request_unit_time_penalty = 0;

	map<VMtype,int>::const_iterator const_iter_vm_type;
	map<VMtype,cBaseVM>::const_iterator const_find_vm_type;
	map<servicetype,cService*>::const_iterator const_find_service_type;

	const_find_service_type = _service_type_map.find(service_type);
	if (const_find_service_type == _service_type_map.end())
	{
		
		cout<<"It's looking for "<<const_find_service_type->first<<" service type"<<endl;
		cout<<"Error!!! There is no specified service type existed!"<<endl;
		exit(0);
	}

	p_vm_vec = new vector<cVirtualMachine>;

	unsigned long int index_required_service_num = 0;
	for (;index_required_service_num < _required_service_num;index_required_service_num++)
	{
		for (const_iter_vm_type = const_find_service_type->second->vm_request_map.begin();const_iter_vm_type != const_find_service_type->second->vm_request_map.end();const_iter_vm_type++)
		{
			int i = 0;
			ID id = 1;
			for (i = 0;i < const_iter_vm_type->second;i++,id++)
			{
				const_find_vm_type = _base_vm_map.find(const_iter_vm_type->first);
				if (const_find_vm_type == _base_vm_map.end())
				{
					cout<<"It's looking for "<<const_iter_vm_type->first<<" vm type"<<endl;
					cout<<"Error!! There is no vm type existing!"<<endl;
					exit(0);
				}
				request_unit_time_revenue += const_find_service_type->second->getUnitReward();
				request_unit_time_penalty += const_find_service_type->second->getUnitPenalty();
				//p_vm_vec->push_back(cVirtualMachine(id,0,0,NULL,NULL,const_find_vm_type->first,const_find_vm_type->second));
				p_vm_vec->push_back(cVirtualMachine(id,0,0,NULL,NULL,const_find_vm_type->first,const_find_vm_type->second));
			}
		}//end for(const_iter_vm_type...
	}//end for(;index_required_service_num...
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
		is_accepted = _requ.is_accepted;
		arrival_rate = _requ.arrival_rate;
		departure_rate = _requ.departure_rate;
		start_time = _requ.start_time;
		duration_time = _requ.duration_time;
		departure_time = _requ.departure_time;
		service_type = _requ.service_type;
		request_type = _requ.request_type;
		request_unit_time_revenue = _requ.request_unit_time_revenue;
		request_unit_time_penalty = _requ.request_unit_time_penalty;

		this->p_vm_vec = new vector<cVirtualMachine>;
		this->p_vm_vec->clear();
		this->p_vm_vec->assign(_requ.p_vm_vec->begin(),_requ.p_vm_vec->end());

		//p_vm_vec->clear();
		//p_vm_vec->assign(_requ.p_vm_vec->begin(),_requ.p_vm_vec->end());
	}

	return *this;
}

cRequest::~cRequest(void)
{
	delete p_vm_vec;
}
