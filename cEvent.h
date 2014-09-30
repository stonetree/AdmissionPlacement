#pragma once

#include "stdafx.h"
#include "cRequest.h"

extern unsigned int total_request;
extern const unsigned int service_type_num;

class cEvent
{
private:
	ID        id;
	double    current_time;
	eventtype event_type;
	ID        requ_id;
	cRequest* request;

public:
	void setID(ID _id){id = _id;}
	ID   getID(void) const {return id;}

	void setCurrentTime(double _time){current_time = _time;}
	double getCurrentTime(void) const {return current_time;}

	void setEventType(eventtype _type){event_type = _type;}
	eventtype getEventType(void) const {return event_type;}

	void setRequest(cRequest* _request){request = _request;}
	cRequest* getRequest(void) {return request;}
public:
	cEvent(void);
	cEvent(ID _id,double _time,eventtype _event_type,ID _requ_id,cRequest* _requ);
	cEvent(const cEvent& _event);
	cEvent& operator=(const cEvent& _event);
	~cEvent(void);
};

