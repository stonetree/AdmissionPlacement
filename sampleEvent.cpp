#include "stdafx.h"

#include "cRequest.h"
#include "cEvent.h"
#include "cService.h"
#include "cVirtualMachine.h"


#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"

//global variables
const unsigned int total_request = 10000;
const unsigned int total_service_type_num = 3;

//define the parameters w.r.t the coming flows for different service types
static const double service_light_arrival_rate = 6;
static const double service_middle_arrival_rate = 3;
static const double service_heavy_arrival_rate = 1;

static const double service_light_departure_rate = 1.5;
static const double service_middle_departure_rate = 0.2;
static const double service_heavy_departure_rate = 0.035;

extern void initialServiceType(map<servicetype,cService*>& _service_map,vector<cService>& _service_vec);
extern void initialbase_vm_typeType(map<VMtype,cBaseVM>& _base_vm_type_map);
extern void initialServiceType(map<servicetype,cService*>& _service_map,vector<cService>& _service_vec);

static ID event_id = 1;

/******************************************************************************/

//initial the arrival time for requests with different type of services.
static int generateArrivalTime(vector<double> _mu_vec,double _request_type_num,multimap<double,servicetype>& _request_arrival_time_multimap)
{
  const gsl_rng_type * T;
  gsl_rng * r;
  static unsigned long int interval_timeSeed = 10;

  vector<double> interval_time_vec;
  vector<double>::iterator iter_interval_time_vec;

  double mu;

  //vector<vector<double>> randomNum_vvec;
  //vector<vector<double>>::iterator iter_randomNum_vvec ;
  vector<double>::iterator iter_mu_vec;  
  int i,j;
  int service_type;
  
  _request_arrival_time_multimap.clear();

  //generate vectors to storage the arrival time for different types of requests corresponding to their arrival rate
  for (i = 0,service_type = 0,iter_mu_vec = _mu_vec.begin();i < _request_type_num; i++,iter_mu_vec++,service_type++)
  {

	  interval_time_vec.clear();

	  gsl_rng_default_seed = interval_timeSeed++;

	  gsl_rng_env_setup();

	  T = gsl_rng_default;
	  r = gsl_rng_alloc (T);

	  mu = 1/(*iter_mu_vec);

	  double interval_time,next_start_time = 0;
	  for (j = 0; j < total_request; j++) 
	  {
		  interval_time = gsl_ran_exponential(r, mu);
		  next_start_time += interval_time;
		  _request_arrival_time_multimap.insert(make_pair(next_start_time,(servicetype)service_type));
	  }

	  gsl_rng_free (r);
  }

  i = 0;
  multimap<double,servicetype>::iterator iter_arrival_time_multimap = _request_arrival_time_multimap.begin();
  while (i < total_request)
  {
	  iter_arrival_time_multimap++;
	  i++;
  }


  _request_arrival_time_multimap.erase(iter_arrival_time_multimap,_request_arrival_time_multimap.end());

  return 0;
}

//here we will initial a sample path of arriving requests and 
//generate a sample event
static void generateSampleRequest(multimap<double,servicetype>& request_arrival_time_multimap,\
	const map<servicetype,cService*>& _service_type_map,const map<VMtype,cBaseVM>& _base_vm_type,vector<cRequest>& _request_vec)
{
	const gsl_rng_type * T_light,*T_middle,*T_heavy; //corresponding to three types of services
	gsl_rng * r,*r_light,*r_middle,*r_heavy;
	static unsigned long int duration_time_seed = 100;
	ID requ_id = 1;
	ID eventID = 1;
	double arrival_rate,departure_rate;

	gsl_rng_env_setup();

	gsl_rng_default_seed = duration_time_seed++;

	T_light = gsl_rng_default;
	r_light = gsl_rng_alloc(T_light);

	T_middle = gsl_rng_default;
	r_middle = gsl_rng_alloc(T_middle);

	T_heavy = gsl_rng_default;
	r_heavy = gsl_rng_alloc(T_heavy);

	double mu;

	multimap<double,servicetype>::iterator iter_request_arrival_time_multipmap = request_arrival_time_multimap.begin();

	double duration_time;
	for (;iter_request_arrival_time_multipmap != request_arrival_time_multimap.end();iter_request_arrival_time_multipmap++)
	{
		switch(iter_request_arrival_time_multipmap->second){
			case LIGHT:
				r = r_light;
				mu = 1/service_light_departure_rate;
				arrival_rate = service_light_arrival_rate;
				departure_rate = service_light_departure_rate;
				break;
			case MIDDLE:
				r = r_middle;
				mu = 1/service_middle_departure_rate;
				arrival_rate =  service_middle_arrival_rate;
				departure_rate = service_middle_departure_rate;
				break;
			case HEAVY:
				r = r_heavy;
				mu =1/service_heavy_departure_rate;
				arrival_rate = service_heavy_arrival_rate;
				departure_rate = service_heavy_departure_rate;
				break;
		}

		duration_time = gsl_ran_exponential(r,mu);

		_request_vec.push_back(cRequest(requ_id++,arrival_rate,departure_rate,iter_request_arrival_time_multipmap->first,\
			duration_time,iter_request_arrival_time_multipmap->first + duration_time,iter_request_arrival_time_multipmap->second,_service_type_map,_base_vm_type));	
	}

	return ;
}

//generate sample event list
static void generateSmapleEventList(vector<cRequest>& _request_vec,multimap<double,cEvent>& _event_multimap)
{
	vector<cRequest>::iterator iter_request_vec = _request_vec.begin();

	ID id = 1;

	vector<cVirtualMachine>::iterator iter_vm;
	
	for (;iter_request_vec != _request_vec.end();iter_request_vec++,id++)
	{
		
		for (iter_vm = iter_request_vec->vm_vec.begin();iter_vm != iter_request_vec->vm_vec.end();iter_vm++)
		{
			iter_vm->setRequestID(iter_request_vec->getID());
			iter_vm->setRequestPoint(&(*iter_request_vec));
		}
		_event_multimap.insert(make_pair(iter_request_vec->getStartTime(),cEvent(event_id,iter_request_vec->getStartTime(),ARRIVAL,iter_request_vec->getID(),&(*iter_request_vec))));
//		_event_multimap.insert(make_pair(iter_request_vec->getDepartureTime(),cEvent(id,iter_request_vec->getDepartureTime(),DEPARTURE,iter_request_vec->getID(),&(*iter_request_vec))));
	}
	
	multimap<double,cEvent>::iterator iter_event_multimap;
	for (iter_event_multimap = _event_multimap.begin();iter_event_multimap != _event_multimap.end();iter_event_multimap++)
	{
		iter_event_multimap->second.setID(event_id++);
	}
	return ;
}

//insert a departure event into the event list if one of requests is accepted.
void insertDepartureEvent(cRequest* _p_request,map<double,cEvent>& _event_multimap)
{
	
	_event_multimap.insert(make_pair(_p_request->getDepartureTime(),cEvent(event_id,_p_request->getDepartureTime(),DEPARTURE,_p_request->getID(),_p_request)));
	
	return ;
}


//Generate a sample 
void generateSampleEvent(vector<cRequest>& _request_vec,multimap<double,cEvent>& _event_multimap)
{
	//clear the output file _event_vec
	_request_vec.clear();
	_event_multimap.clear();

	//initial the service type
	map<servicetype,cService*> service_type_map;
	vector<cService> service_vec;
	initialServiceType(service_type_map,service_vec);
	
	//initial VM type
	map<VMtype,cBaseVM> base_vm_map;
	initialbase_vm_typeType(base_vm_map);

	//It is used to initialize the set of requests
	//Since we want to initialize a set of requests with different types of arrival configurations,
	//we should more carefully think about how to make those requests spread reasonably over the whole simulation time. 
	multimap<double,servicetype>  request_arrival_time_multimap;

	vector<vector<double>> arrival_time_vvec;
	vector<vector<double>>::iterator iter_arrival_time_vec;
	vector<double> arrival_rate_vec;
	arrival_rate_vec.push_back(service_light_arrival_rate);
	arrival_rate_vec.push_back(service_middle_arrival_rate);
	arrival_rate_vec.push_back(service_heavy_arrival_rate);

	//initial the arrival time sequence for coming requests
	generateArrivalTime(arrival_rate_vec,arrival_rate_vec.size(),request_arrival_time_multimap);

	//initial the set of coming requests
	generateSampleRequest(request_arrival_time_multimap,service_type_map,base_vm_map,_request_vec);

	//initial the event list
	generateSmapleEventList(_request_vec,_event_multimap);
	
	return;
}