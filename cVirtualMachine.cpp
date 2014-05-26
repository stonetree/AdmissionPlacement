#include "StdAfx.h"
#include "cVirtualMachine.h"


cVirtualMachine::cVirtualMachine(void)
{
}


cVirtualMachine::~cVirtualMachine(void)
{
}


cVirtualMachine& cVirtualMachine::operator=(const cVirtualMachine& _vm)
{
	if (this != &_vm)
	{
		//type = _vm.type;
		//cpu_required = _vm.cpu_required;
		//mem_required = _vm.mem_required;
		//disk_required = _vm.disk_required;
		id = _vm.id;
		hosted_server_id = _vm.hosted_server_id;
		request_id = _vm.request_id;
		p_hosted_server = _vm.p_hosted_server;
		p_request = _vm.p_request;
	}

	return *this;
}