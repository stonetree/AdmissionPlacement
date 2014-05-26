#include "StdAfx.h"
#include "cEvent.h"


cEvent::cEvent(void)
{
}


cEvent::~cEvent(void)
{
}

cEvent::cEvent(ID _id,double _time,eventtype _event_type,ID _requ_id,cRequest* _requ)
{
	id = _id;
	requ_id = _requ_id;
	current_time = _time;
	event_type = _event_type;
	request = _requ;
}

cEvent::cEvent(const cEvent& _event)
{
	this->operator=(_event);
}

cEvent& cEvent::operator=(const cEvent& _event)
{
	if (this != &_event)
	{
		id = _event.id;
		requ_id = _event.requ_id;
		current_time = _event.current_time;
		event_type =_event.event_type;
		request = _event.request;
	}

	return *this;
}