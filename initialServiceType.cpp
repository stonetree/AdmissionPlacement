#include "stdafx.h"
#include "cService.h"

//global variables
extern const unsigned int total_request;
extern const unsigned int total_service_type_num;

//define the parameters w.r.t the configurations of different types of services in terms of the type and num of VMs required/
//service type 1 referred as LIGHT here
static const VMtype service_light_vm_type = MEDIUM;
static const int    service_light_vm_num  = 3;

//service type 2 referred as MIDDLE here
static const VMtype service_middle_vm_type = LARGE;
static const int    service_middle_vm_num = 3;

//service type 3 referred as HEAVY here
static const VMtype service_heavy_vm_type = XLARGE;
static const int    service_heavy_vm_num  = 2;

//define the reward as well as the penalty per unit time for each type of services
static const double service_light_unit_reward = 0.21;
static const double service_light_unit_penalty = 0.03;

static const double service_middle_unit_reward = 0.42;
static const double service_middle_unit_penalty = 0.12;

static const double service_heavy_unit_reward = 0.56;
static const double service_heavy_unit_penalty = 0.24;

//define the type and num of VMs corresponding to different type of services
static pair<VMtype,int> serviceLightVM = make_pair(service_light_vm_type,service_light_vm_num);
static pair<VMtype,int> serviceMiddleVM = make_pair(service_middle_vm_type,service_middle_vm_num);
static pair<VMtype,int> serviceHeavyVM = make_pair(service_heavy_vm_type,service_heavy_vm_num);

//
void initialServiceType(map<servicetype,cService*>& _service_map,vector<cService>& _service_vec)
{
	vector<pair<VMtype,int>> vm_type;
	vector<cService>::iterator iter_service_vec;
	_service_map.clear();
	_service_vec.clear();

	//right now only ONE VM configuration will be considered.
	//the first type of service
	vm_type.clear();
	vm_type.push_back(serviceLightVM);
	_service_vec.push_back(cService(LIGHT,vm_type,service_light_unit_reward,service_light_unit_penalty));

	vm_type.clear();
	vm_type.push_back(serviceMiddleVM);
	_service_vec.push_back(cService(MIDDLE,vm_type,service_middle_unit_reward,service_middle_unit_penalty));

	vm_type.clear();
	vm_type.push_back(serviceHeavyVM);
	_service_vec.push_back(cService(HEAVY,vm_type,service_heavy_unit_reward,service_heavy_unit_penalty));
	
	for (iter_service_vec = _service_vec.begin();iter_service_vec != _service_vec.end();iter_service_vec++)
	{
		_service_map.insert(make_pair(iter_service_vec->getServType(),&(*iter_service_vec)));
	}
	
	return;
}