#pragma once

#include "cVirtualMachine.h"
#include "cService.h"

class cServer;

class cRequest
{
private:
	ID     id;
	bool   is_accepted;
	servicetype service_type;
	requesttype request_type;
	double arrival_rate;
	double departure_rate;
	double start_time;
	double duration_time;
	double departure_time;


public:
	vector<cVirtualMachine> vm_vec;

public:
	void   setID(ID _id){id = _id;}
	ID     getID(void) const {return id;}

	void   setAccepted(bool _bool){is_accepted = _bool;}
	bool   getAccepted(void) const {return is_accepted;}

	void setArrivalRate(double _rate){arrival_rate = _rate;}
	double getArrivalRate(void) const {return arrival_rate;}

	void setDepartureRate(double _rate){departure_rate = _rate;}
	double getDepartureRate(void) const {return departure_rate;}

	void   setStartTime(double _start_time){start_time = _start_time;}
	double getStartTime(void) const {return start_time;}

	void   setDurationTime(double _duration_time){duration_time = _duration_time;}
	double getDurationTime(void) const {return duration_time;}

	void   setDepartureTime(double _departure_time){departure_time = _departure_time;}
	double getDepartureTime(void) const {return departure_time;}

	void   serServiceType(servicetype _service_type){service_type = _service_type;}
	servicetype getServiceType(void) const {return service_type;}

	void   serRequestType(requesttype _request_type){request_type = _request_type;}
	requesttype getRequestType(void) const {return request_type;}


public:
	cRequest(void);
	cRequest(ID _id, double _lambda,double _mu,double _start_time,double _duration_time, double _departure_time,\
		requesttype _request_type,servicetype _service_type,const map<servicetype,cService*>& _service_type_map,\
		const map<VMtype,cBaseVM>& _base_vm_map,bool _is_accepted = false);
	cRequest(const cRequest& _requ);
	cRequest& operator=(const cRequest& _requ);
	~cRequest(void);
};

