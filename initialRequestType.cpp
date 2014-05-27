#include "stdafx.h"

//define the parameters w.r.t the coming flows for different service types
static const double low_arrival_rate = 6;
static const double hight_arrival_rate = 3;
static const double xhight_arrival_rate = 1;

static const double low_departure_rate = 1.5;
static const double hight_departure_rate = 0.2;
static const double xhight_departure_rate = 0.035;

void initialRequestType(map<requesttype,pair<double,double>>& _request_type_map)
{
	_request_type_map.clear();

	_request_type_map.insert(make_pair(LOW,make_pair(low_arrival_rate,low_departure_rate)));
	_request_type_map.insert(make_pair(HIGHT,make_pair(hight_arrival_rate,hight_departure_rate)));
	_request_type_map.insert(make_pair(XHIGHT,make_pair(xhight_arrival_rate,xhight_departure_rate)));
	return ;
}