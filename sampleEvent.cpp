#include "stdafx.h"

#include "cRequest.h"
#include "cEvent.h"
#include "cService.h"
#include "cVirtualMachine.h"
#include "common.h"


#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"


ID event_id = 1;
unsigned long int interval_timeSeed = 10;
unsigned long int duration_time_seed = 100;
unsigned long int required_vm_num_seed = 1000;

//the average profits over all sample paths if all requests are accepted.
double average_profits = 0;

//request types
map<requesttype,pair<double,double>> request_type_map;

//service types
map<servicetype,cService*> service_type_map;
vector<cService> service_vec;
map<VMtype,cBaseVM> base_vm_map;

/******************************************************************************/

//initial the arrival time for requests with different type of services.
static int generateArrivalTime(map<requesttype,pair<double,double>>& _request_type_map,multimap<double,requesttype>& _request_arrival_time_multimap)
{
  const gsl_rng_type * T;
  gsl_rng * r;

  vector<double> interval_time_vec;
  vector<double>::iterator iter_interval_time_vec;

  double mu;

  //vector<vector<double>> randomNum_vvec;
  //vector<vector<double>>::iterator iter_randomNum_vvec ;
  map<requesttype,pair<double,double>>::iterator iter_request_type_map;  
  int i,j;
  int request_type;
  
  _request_arrival_time_multimap.clear();

  //generate vectors to storage the arrival time for different types of requests corresponding to their arrival rate
  for (i = 0,request_type = 0,iter_request_type_map = _request_type_map.begin();iter_request_type_map != _request_type_map.end(); i++,iter_request_type_map++,request_type++)
  {

	  interval_time_vec.clear();
	  
	  gsl_rng_env_setup();

	  gsl_rng_default_seed = interval_timeSeed++;

	  T = gsl_rng_default;
	  r = gsl_rng_alloc (T);

	  mu = 1/(iter_request_type_map->second.first);

	  double interval_time,next_start_time = 0;
	  for (j = 0; j < total_request; j++) 
	  {
		  interval_time = gsl_ran_exponential(r, mu);
		  next_start_time += interval_time;
		  _request_arrival_time_multimap.insert(make_pair(next_start_time,(requesttype)request_type));
	  }

	  gsl_rng_free (r);
  }

  //only need the first "total_request" requests
  //and following elements will be deleted.
  i = 0;
  multimap<double,requesttype>::iterator iter_arrival_time_multimap = _request_arrival_time_multimap.begin();
  while (i < total_request)
  {
	  iter_arrival_time_multimap++;
	  i++;
  }


  _request_arrival_time_multimap.erase(iter_arrival_time_multimap,_request_arrival_time_multimap.end());

  return 0;
}

//allocate the service type for each type of requests.
//here we can change the mapping relationship between request types and service types
//Currently we use a quite simple mapping function as following
servicetype determineServiceType(requesttype _request_type)
{
	switch(_request_type){
	case LOW:
		return LIGHT;
	case HIGHT:
		return MIDDLE;
	case XHIGHT:
		return HEAVY;
	default:
		cout<<"Error!!!Can not locate the service type"<<endl;
		exit(0);
	}
}

unsigned long int determineMaxRequiredServiceNum(requesttype _request_type)
{
	switch(_request_type){
	case LOW:
		return 5;
	case HIGHT:
		return 3;
	case XHIGHT:
		return 2;
	default:
		cout<<"Error!!!Can not locate the service type"<<endl;
		exit(0);
	}
}

//here we will initial a sample path of arriving requests and 
//generate a sample event
static void generateSampleRequest(const multimap<double,requesttype>& _request_arrival_time_multimap,const map<requesttype,pair<double,double>>& _request_type_map,\
	const map<servicetype,cService*>& _service_type_map,const map<VMtype,cBaseVM>& _base_vm_type,vector<cRequest>& _request_vec)
{
	ID requ_id = 1;
	ID eventID = 1;
	double duration_time;
	servicetype service_type;
	unsigned int max_required_service_num,required_service_num;

	map<requesttype,pair<const gsl_rng_type*,gsl_rng*>> rng_generator_map;

	map<requesttype,pair<const gsl_rng_type*,gsl_rng*>> rng_service_num_generator_map;

	gsl_rng_env_setup();
	gsl_rng_default_seed = duration_time_seed++;

	//initial the random number generators for different types of requests
	//which mainly indicate the arrival rate and the departure rate
	map<requesttype,pair<double,double>>::const_iterator const_iter_request_type_map;
	for (const_iter_request_type_map = _request_type_map.begin();const_iter_request_type_map != _request_type_map.end();const_iter_request_type_map++)
	{
		const gsl_rng_type * tem_T;
		gsl_rng * tem_r;

		tem_T = gsl_rng_default;
		tem_r = gsl_rng_alloc(tem_T);

		rng_generator_map.insert(make_pair(const_iter_request_type_map->first,make_pair(tem_T,tem_r)));
	}

	//initial the random number generators for different types of requests
	//which mainly indicate the number of services required
	for (const_iter_request_type_map = _request_type_map.begin();const_iter_request_type_map != _request_type_map.end();const_iter_request_type_map++)
	{
		const gsl_rng_type * tem_service_num_T;
		gsl_rng * tem_service_num_r;

		tem_service_num_T = gsl_rng_default;
		tem_service_num_r = gsl_rng_alloc(tem_service_num_T);

		rng_service_num_generator_map.insert(make_pair(const_iter_request_type_map->first,make_pair(tem_service_num_T,tem_service_num_r)));
	}

	multimap<double,requesttype>::const_iterator const_iter_request_arrival_time_multipmap = _request_arrival_time_multimap.begin();
	for (;const_iter_request_arrival_time_multipmap != _request_arrival_time_multimap.end();const_iter_request_arrival_time_multipmap++)
	{
		map<requesttype,pair<double,double>>::const_iterator const_iter_request_type_map;
		map<requesttype,pair<const gsl_rng_type*,gsl_rng*>>::iterator const_iter_rng_generator_map;
		map<requesttype,pair<const gsl_rng_type*,gsl_rng*>>::iterator const_iter_rng_service_num_generator_map;

		const_iter_request_type_map = _request_type_map.find(const_iter_request_arrival_time_multipmap->second);
		if (const_iter_request_type_map == _request_type_map.end())
		{
			cout<<"Error!!!Can not find the corresponding request type!!!"<<endl;
			exit(0);
		}

		const_iter_rng_generator_map = rng_generator_map.find(const_iter_request_arrival_time_multipmap->second);
		if (const_iter_rng_generator_map == rng_generator_map.end())
		{
			cout<<"Error!!!Can not find the corresponding request type!!!"<<endl;
			exit(0);
		}

		const_iter_rng_service_num_generator_map = rng_service_num_generator_map.find(const_iter_request_arrival_time_multipmap->second);
		if (const_iter_rng_service_num_generator_map == rng_service_num_generator_map.end())
		{
			cout<<"Error!!!Can not find the corresponding request type!!!"<<endl;
			exit(0);
		}
		
		service_type = determineServiceType(const_iter_request_arrival_time_multipmap->second);

		//generate the random duration time based on the type of current request
		duration_time = gsl_ran_exponential(const_iter_rng_generator_map->second.second,1/const_iter_request_type_map->second.second);

		max_required_service_num = determineMaxRequiredServiceNum(const_iter_request_arrival_time_multipmap->second);
				
		required_service_num = 1 + gsl_rng_uniform_int(const_iter_rng_service_num_generator_map->second.second,max_required_service_num);
		
		_request_vec.push_back(cRequest(requ_id++,const_iter_request_type_map->second.first,const_iter_request_type_map->second.second,const_iter_request_arrival_time_multipmap->first,\
			duration_time,const_iter_request_arrival_time_multipmap->first + duration_time,const_iter_request_arrival_time_multipmap->second,service_type,_service_type_map,_base_vm_type,required_service_num));	
	}


	map<requesttype,pair<const gsl_rng_type*,gsl_rng*>>::iterator iter_rng_generator_map = rng_generator_map.begin();
	for (;iter_rng_generator_map != rng_generator_map.end();iter_rng_generator_map++)
	{
		gsl_rng_free(iter_rng_generator_map->second.second);
	}

	map<requesttype,pair<const gsl_rng_type*,gsl_rng*>>::iterator iter_rng_service_num_map = rng_service_num_generator_map.begin();
	for (;iter_rng_service_num_map != rng_service_num_generator_map.end();iter_rng_service_num_map++)
	{
		gsl_rng_free(iter_rng_service_num_map->second.second);
	}
	//vector<cRequest>::iterator iter_request = _request_vec.begin();
	//for (;iter_request != _request_vec.end();iter_request++)
	//{
	//	average_profits += ((service_type_map[iter_request->getServiceType()])->getUnitReward() * iter_request->p_vm_vec->size()) * iter_request->getDurationTime();
	//}

	//cout<<"The average profits is "<<average_profits / (sample_index + 1)<<endl;

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
		
		for (iter_vm = iter_request_vec->p_vm_vec->begin();iter_vm != iter_request_vec->p_vm_vec->end();iter_vm++)
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
void insertDepartureEvent(cRequest* _p_request,multimap<double,cEvent>& _event_multimap)
{
	
	_event_multimap.insert(make_pair(_p_request->getDepartureTime(),cEvent(event_id++,_p_request->getDepartureTime(),DEPARTURE,_p_request->getID(),_p_request)));
	
	return ;
}


//Generate a sample 
void generateSampleEvent(vector<cRequest>& _request_vec,multimap<double,cEvent>& _event_multimap)
{
	//clear the output file _event_vec
	_request_vec.clear();
	_event_multimap.clear();

	//initial the service types
	initialServiceType(service_type_map,service_vec);
	
	//initial VM types
	initialbaseVMType(base_vm_map);

	//initial request types
	initialRequestType(request_type_map);

	//initial optional policies or placement functions
	//initialPolicies();

	//It is used to initialize the set of requests
	//Since we want to initialize a set of requests with different types of arrival configurations,
	//we should more carefully think about how to make those requests spread reasonably over the whole simulation time. 
	multimap<double,requesttype> *p_request_arrival_time_multimap = new multimap<double,requesttype>;
	//multimap<double,requesttype>  request_arrival_time_multimap;

	//initial the arrival time sequence for coming requests
	generateArrivalTime(request_type_map,*p_request_arrival_time_multimap);

	//initial the set of coming requests
	generateSampleRequest(*p_request_arrival_time_multimap,request_type_map,service_type_map,base_vm_map,_request_vec);

	//initial the event list
	generateSmapleEventList(_request_vec,_event_multimap);

	delete p_request_arrival_time_multimap;
	
	return;
}