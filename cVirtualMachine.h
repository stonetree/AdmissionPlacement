#pragma once
#include "stdafx.h"
#include "cBaseVM.h"

class cServer;
class cRequest;

class cVirtualMachine:
	public cBaseVM
{
private:
	ID id;
	ID  hosted_server_id;
	ID  request_id;
	cServer* p_hosted_server;
	cRequest* p_request;


public:
	void setID(ID _id){id = _id;}
	ID   getID(void) const {return id;}

	void setHostedServID(ID _serverID){hosted_server_id = _serverID;}
	ID   getHostedServID(void) const {return hosted_server_id;}

	void setRequestID(ID _request_id){request_id = _request_id;}
	ID   getRequestID(void) const {return request_id;}

	void setHostedServPoint(cServer* _pServer){p_hosted_server = _pServer;}
	cServer* getHostedServerPoint(void) const {return p_hosted_server;}

	void setRequestPoint(cRequest* _p_request){p_request = _p_request;}
	cRequest* getRequestPoint(void) const {return p_request;}

public:
	cVirtualMachine(void);
	cVirtualMachine(ID _id,ID _hostServerID,ID _request_id,cServer* _p_hosted_server,cRequest* _p_request,VMtype _type,\
		double _cpu_required,double _mem_required,double _disk_required):cBaseVM(_type,_cpu_required,_mem_required, _disk_required){id = _id;\
		hosted_server_id = _hostServerID;request_id = _request_id; p_hosted_server = _p_hosted_server; p_request = _p_request;}
	cVirtualMachine(ID _id,ID _hostServerID,ID _request_id,cServer* _p_hosted_server,cRequest* _p_request,VMtype _type,\
		const cBaseVM& _base_vm_type):cBaseVM(_base_vm_type){id = _id;hosted_server_id = _hostServerID;request_id = _request_id; \
		p_hosted_server = _p_hosted_server; p_request = _p_request;}
	cVirtualMachine(const cVirtualMachine& _vm):cBaseVM(_vm){operator=(_vm);}

	cVirtualMachine& operator=(const cVirtualMachine& _vm);
	~cVirtualMachine(void);
};

