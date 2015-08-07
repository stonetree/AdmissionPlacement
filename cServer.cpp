#include "StdAfx.h"
#include "cServer.h"


cServer::cServer(void)
{
}


cServer::~cServer(void)
{
}

cServer::cServer(ID _id, resource_unit _cpu_capacity, resource_unit _mem_capacity, resource_unit _disk_capacity)
{
	id = _id;
	cpu_capacity = _cpu_capacity;
	mem_capacity = _mem_capacity;
	disk_capacity = _disk_capacity;

	cpu_residual = _cpu_capacity;
	mem_residual = _mem_capacity;
	disk_residual = _disk_capacity;

}

cServer::cServer(const cServer& _server)
{
	this->operator=(_server);
}

cServer& cServer::operator=(const cServer& _server)
{
	if (this != &_server)
	{
		id = _server.id;
		cpu_capacity = _server.cpu_capacity;
		mem_capacity = _server.mem_capacity;
		disk_capacity = _server.disk_capacity;

		cpu_residual = _server.cpu_residual;
		mem_residual = _server.mem_residual;
		disk_residual = _server.disk_residual;

		hosted_vm_list.clear();
		hosted_vm_list.assign(_server.hosted_vm_list.begin(),_server.hosted_vm_list.end());
	}

	return *this;
}

bool cServer::operator ==(const cServer& _server)
{
	return (cpu_capacity == _server.cpu_capacity)&&(mem_capacity == _server.mem_capacity)&&(disk_capacity == _server.disk_capacity)&& \
		(cpu_residual == _server.cpu_residual)&&(mem_residual == _server.mem_residual)&&(disk_residual == _server.disk_residual);
}

void cServer::allocateVirtMach(cVirtualMachine* _vmine)
{
	//Allocate VM on this server.

	cpu_residual -= _vmine->getcpuRequired();
	mem_residual -= _vmine->getmemRequired();
	disk_residual -= _vmine->getdiskRequired();

	hosted_vm_list.push_back(_vmine);
	
	
	return;
}

void cServer::releaseVirtMach(cVirtualMachine* _vmine)
{
	//Release VM as well as the resources occupied.
	list<cVirtualMachine*>::iterator iter_vm_list = find(hosted_vm_list.begin(),hosted_vm_list.end(),_vmine);

	//Check if the VM to be released has already been allocated on this server.
	//If not, maybe some kinds of operations have been carried out.
	if (iter_vm_list == hosted_vm_list.end())
	{
		cout<<"Error!!! Releasing a VM that doesn't exist."<<endl;
		exit(0);
	}
	
	cpu_residual += _vmine->getcpuRequired();
	mem_residual += _vmine->getmemRequired();
	disk_residual += _vmine->getdiskRequired();

	hosted_vm_list.erase(iter_vm_list);
	
	return;
}