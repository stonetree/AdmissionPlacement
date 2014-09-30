#include "stdafx.h"

#include "cServer.h"
#include "cRequest.h"
#include "cEvent.h"
#include "cConfiguration.h"

class cPolity;

typedef bool (*placementfunction)(vector<cServer>& _server_vec,cRequest* _request,int* _iterationPlacement);
typedef map<pair<requesttype,unsigned long int>,double> system_value_map;
typedef map<pair<requesttype,unsigned long int>,cPolity> system_policy_map;

extern double low_arrival_rate;
extern double hight_arrival_rate;
extern double xhight_arrival_rate;

extern unsigned int total_request;
extern const unsigned int total_service_type_num;
extern const double discout_factor;
extern const double value_function_update_factor;
extern const unsigned int total_server_num;
extern double average_accepted_rate;
extern double average_allocation_fail_rate;
extern double accepted_requests_num;
extern unsigned int sample_request_num;
extern double workload_rate;

extern ID event_id;

extern system_value_map  *global_point_system_value_map;
extern system_policy_map *global_point_system_policy_map;

extern vector<vector<double>> commu_cost;
extern vector<double> basisFuncParameter;
extern const double local_communication_cost;
extern const double tor_communication_cost;
extern const double remote_communication_cost;

extern const double discout_factor;
extern map<requesttype,pair<double,double>> request_type_map;
extern map<servicetype,cService*> service_type_map;
extern map<VMtype,cBaseVM> base_vm_map;
extern vector<cService> service_vec;
extern pair<requesttype,unsigned long int> system_state;
extern unsigned long int initial_system_state_indicator;
extern vector<pair<string,placementfunction>> policy_vec;
extern map<pair<requesttype,unsigned long int>,double> system_state_value_map;
extern map<pair<requesttype,unsigned long int>,cPolity> system_state_policy_map;
extern unsigned int sample_index;
extern unsigned long int interval_timeSeed;
extern unsigned long int duration_time_seed;
extern unsigned long int required_vm_num_seed;
extern double current_time;
extern double allocation_fail_num;

extern void insertDepartureEvent(cRequest* _p_request,multimap<double,cEvent>& _event_multimap);

extern bool initialInputfile(vector<cConfiguration>& _config_vec);

extern void initialPhyServers(vector<cServer>& _server_vec);

extern void initialSystemState(vector<cServer>& _server_vec);

extern void initialRequestType(map<requesttype,pair<double,double>>& _request_type_map);

extern void initialPolicies();

extern void initialPolicies(unsigned int _conf_policy);

extern void generateSampleEvent(vector<cRequest>& _request_vec,multimap<double,cEvent>& _event_multimap);

extern void getStateValue(multimap<double,cEvent>& _event_multimap,vector<cRequest>& _request_vec,\
	vector<cServer>& _server_vec,map<double,double>& _state_value);

extern void obtainOptimalStateValue(multimap<double,cEvent>& _event_multimap,vector<cServer>& _server_vec);

extern void initialInputParameter(vector<cConfiguration>::iterator _iter_config_vec);

extern void initialServiceType(map<servicetype,cService*>& _service_map,vector<cService>& _service_vec);

extern void initialbaseVMType(map<VMtype,cBaseVM>& _base_vm_type_map);

extern void initialServiceType(map<servicetype,cService*>& _service_map,vector<cService>& _service_vec);

extern void initialCommuCost(const vector<cServer>& _server_vec);

extern bool vmDeployment(vector<cServer>& _server_vec,cRequest* _request,pair<string,placementfunction>& _placement_func_pair, int* _iterationPlacement);

extern double obtainCommuCost(const cRequest* _request);

extern unsigned long int calculateRequestStateIndicator(const vector<cServer>& _server_vec,const cRequest* _request);
extern unsigned long int calculateStateIndicator(const vector<cServer>& _server_vec);

extern void outputResults();

extern bool noPlacement(vector<cServer>& _server_vec,cRequest* _request,int* _iterationPlacement);
extern bool greedyVMPlacement(vector<cServer>& _server_vec,cRequest* _request,int* _iterationPlacement);
extern bool balanceVMPlacement(vector<cServer>& _server_vec,cRequest* _request,int* _iterationPlacement);
extern bool optimalVMPlacement(vector<cServer>& _server_vec,cRequest* _request,int* _iterationPlacement);

//for basis function use only
extern bool obtainOptimalActionBasicFunc(cEvent* _event,vector<cServer>& _server_vec,
	map<requesttype,unsigned int>& _hosted_requests_type_num_map,\
	map<ID,cRequest*>& _hosted_request_map);
extern void initialBasisFuncParameters(const vector<cServer>& _server_vec);
extern void outputResultsBasisFunct();


