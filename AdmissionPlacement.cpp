// AdmissionPlacement.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "cRequest.h"
#include "cEvent.h"

extern void generateSampleEvent(vector<cRequest>& _request_vec,multimap<double,cEvent>& _event_multimap);

int _tmain(int argc, _TCHAR* argv[])
{
	vector<cRequest> request_vec;
	multimap<double,cEvent> event_multimap;

	generateSampleEvent(request_vec,event_multimap);

	
	
	return 0;
}

