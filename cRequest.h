#pragma once

#include "cVirtualMachine.h"
#include "cService.h"

class cServer;

class cRequest
{
private:
	ID     id;
	servicetype service_type;
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

	void   serRestType(servicetype _service_type){service_type = _service_type;}
	servicetype getRestType(void) const {return service_type;}


public:
	cRequest(void);
	cRequest(ID _id, double _lambda,double _mu,double _start_time,double _duration_time, double _departure_time,servicetype _service_type,const map<servicetype,cService*>& _service_type_map,const map<VMtype,cBaseVM>& _base_vm_map);
	cRequest(const cRequest& _requ);
	cRequest& operator=(const cRequest& _requ);
	~cRequest(void);
};
