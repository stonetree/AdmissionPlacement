#include "stdafx.h"

#include "cServer.h"
#include "cRequest.h"

typedef void (*placementfunction)(vector<cServer>& _server_vec,cRequest* _request);

extern void insertDepartureEvent(cRequest* _p_request,multimap<double,cEvent>& _event_multimap);
extern void initialPhyServers(vector<cServer>& _server_vec);
extern void initialRequestType(map<requesttype,pair<double,double>>& _request_type_map);
extern void generateSampleEvent(vector<cRequest>& _request_vec,multimap<double,cEvent>& _event_multimap);
extern void getStateValue(multimap<double,cEvent>& _event_multimap,vector<cRequest>& _request_vec,vector<cServer>& _server_vec,map<double,double>& _state_value);
extern void obtainOptimalStateValue(multimap<double,cEvent>& _event_multimap,vector<cRequest>& _request_vec,vector<cServer>& _server_vec,vector<pair<placementfunction,int>>& _placement_func,map<double,double>& _state_value);

extern void initialServiceType(map<servicetype,cService*>& _service_map,vector<cService>& _service_vec);
extern void initialbase_vm_typeType(map<VMtype,cBaseVM>& _base_vm_type_map);
extern void initialServiceType(map<servicetype,cService*>& _service_map,vector<cService>& _service_vec);