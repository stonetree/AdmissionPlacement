#include "stdafx.h"
#include "cServer.h"
#include "cService.h"
#include "cEvent.h"
#include "cRequest.h"
#include "common.h"
#include "cPolity.h"

const double step_size = 0.2;

map<string,int> policy_counting;//for basis function use only

void initialBasisFuncParameters(const vector<cServer>& _server_vec)
{
	vector<cServer>::const_iterator const_iter_server_vec = _server_vec.begin();
	for (;const_iter_server_vec != _server_vec.end();const_iter_server_vec++)
	{
		basisFuncParameter.push_back(0);
	}
	return;
}

static double getRequestStateValue(const cRequest* _request)
{
	double state_value = 0;
	vector<cVirtualMachine>::const_iterator const_iter_vm = _request->vm_vec.begin();
	double cpu_capacity,mem_capacity,disk_capacity;
	for (;const_iter_vm != _request->vm_vec.end();const_iter_vm++)
	{
		if (const_iter_vm->getHostedServID() == 0)
		{
			return 0;
		}
		else
		{
			cpu_capacity = const_iter_vm->getHostedServerPoint()->getcpuCapacity();
			mem_capacity = const_iter_vm->getHostedServerPoint()->getmemCapacity();
			disk_capacity = const_iter_vm->getHostedServerPoint()->getdiskCapacity();
			state_value += const_iter_vm->getcpuRequired()/cpu_capacity * const_iter_vm->getmemRequired()/mem_capacity * const_iter_vm->getdiskRequired()/disk_capacity * basisFuncParameter[const_iter_vm->getHostedServID() - 1];
		}
	}	
	return state_value;
}

static double getStateValueBasisFunc(const vector<cServer>& _server_vec)
{
	//Here we will get the value of V(s) based on the basis function method
	//To simplify the calculation, we define the basis function given by 
	//f(s) = (f_1(s),f_2(s),...,f_3(s)), where f_1(s) refers to the multiplication of the number of residual resources
	//corresponding to different types on server 1 in state s.

	double state_value = 0;
	vector<cServer>::const_iterator const_iter_server_vec = _server_vec.begin();
	vector<double>::const_iterator const_iter_basis_func_parameter = basisFuncParameter.begin();
	for (;const_iter_server_vec != _server_vec.end();const_iter_server_vec++)
	{
		state_value += *const_iter_basis_func_parameter * (const_iter_server_vec->getcpuResidual() * const_iter_server_vec->getmemResidual() * const_iter_server_vec->getdiskResidual()/const_iter_server_vec->getcpuCapacity()/const_iter_server_vec->getmemCapacity()/const_iter_server_vec->getdiskCapacity());
	}

	return state_value;
}

double obtainDeploymentProfitsBasisFunc(vector<cServer>& _server_vec,cRequest* _request,\
	map<requesttype,unsigned int>& _hosted_requests_type_num_map,\
	map<ID,cRequest*>& _hosted_request_map)
{
 	/****************************************************
 	* Here the continue Markov chain will be calculated *
 	*****************************************************/
		
	//calculate the total transition rate
	double total_transition_rate = 0;
	map<requesttype,pair<double,double>>::iterator iter_request_type = request_type_map.begin();
	for (;iter_request_type != request_type_map.end();iter_request_type++)
	{
		total_transition_rate += iter_request_type->second.first;
	}

	map<requesttype,unsigned int>::iterator iter_hosted_requests_type_num_map = _hosted_requests_type_num_map.begin();
	for (;iter_hosted_requests_type_num_map != _hosted_requests_type_num_map.end();iter_hosted_requests_type_num_map++)
	{
		total_transition_rate += (double)(iter_hosted_requests_type_num_map->second) * (request_type_map[iter_hosted_requests_type_num_map->first]).second;
	}

	//calculate the expected value 
	//firstly, iterate the possible request arrival event
	double state_value = 0;
	map<pair<requesttype,double>,double>::iterator iter_find_state_value;
	iter_request_type = request_type_map.begin();
	for (;iter_request_type != request_type_map.end();iter_request_type++)
	{		
		state_value += iter_request_type->second.first / total_transition_rate * (getStateValueBasisFunc(_server_vec) - getRequestStateValue(_request));
	}

	//calculate the expected value
	//secondly, iterate the possible request departure event
	map<ID,cRequest*>::iterator iter_hosted_request_map = _hosted_request_map.begin();
	for (;iter_hosted_request_map != _hosted_request_map.end();iter_hosted_request_map++)
	{
		state_value += request_type_map[iter_hosted_request_map->second->getRequestType()].second / total_transition_rate * (getStateValueBasisFunc(_server_vec) + getRequestStateValue(_request));
	}
	
	if (_request->getAccepted())
	{
		(service_type_map[_request->getServiceType()])->getUnitReward()	;
		obtainCommuCost(_request);
		return state_value + ((service_type_map[_request->getServiceType()])->getUnitReward() - obtainCommuCost(_request)) * _request->getDurationTime();	
	}
	else
	{
		return state_value - (service_type_map[_request->getServiceType()])->getUnitPenalty() * _request->getDurationTime();
	}
}


//obtain the optimal action over the action space (e.g.,whether should accept the arriving request or not and which polity should be take such that optimal
//profit can be obtained)
bool obtainOptimalActionBasicFunc(cEvent* _event,vector<cServer>& _server_vec,
	map<requesttype,unsigned int>& _hosted_requests_type_num_map,\
	map<ID,cRequest*>& _hosted_request_map)
{
	vector<pair<string,placementfunction>>::iterator iter_placement_vec;
	double profit,max_profit = -10000000;

	vector<cServer*> optimal_deployment;
	string name_optimal_policy;

	cRequest* request = _event->getRequest();

	bool accepted_arriving_request = false;

	bool find_solution = false;


	for (iter_placement_vec = policy_vec.begin();iter_placement_vec != policy_vec.end();iter_placement_vec++)
	{
		if (vmDeployment(_server_vec,request,*iter_placement_vec))
		{
			find_solution = true;
			//the placement func processes correctly


			//check whether profits under the current placement func
			//if larger profit can be obtained, update the profit and save the placement solution to be used to allocate resources
			profit = obtainDeploymentProfitsBasisFunc(_server_vec,request,_hosted_requests_type_num_map,_hosted_request_map);
			if(profit > max_profit)
			{
				//find a better placement solution with more profits to be obtained
				//save the placement solution
				max_profit = profit;
				name_optimal_policy = iter_placement_vec->first;
				optimal_deployment.clear();

				//if the request is intentionally rejected, we just need empty the optimal solution vec;
				if (!request->getAccepted())
				{
					accepted_arriving_request  = false;
					continue;
				}

				vector<cVirtualMachine>::iterator iter_vm_vec = request->vm_vec.begin();
				for (;iter_vm_vec != request->vm_vec.end();iter_vm_vec++)
				{
					optimal_deployment.push_back(iter_vm_vec->getHostedServerPoint());
				}

			}
		}
	}

	//finally find a optimal placement solution
	if (!optimal_deployment.empty())
	{			

		request->setAccepted(true);
		vector<cVirtualMachine>::iterator iter_vm_vec = request->vm_vec.begin();
		vector<cServer*>::iterator iter_optimal_depolyment_vec = optimal_deployment.begin();
		for (;iter_vm_vec != request->vm_vec.end();iter_vm_vec++,iter_optimal_depolyment_vec++)
		{
			iter_vm_vec->setHostedServPoint(*iter_optimal_depolyment_vec);
			iter_vm_vec->setHostedServID((*iter_optimal_depolyment_vec)->getID());
		}

		accepted_arriving_request  = true;

	}

	if (find_solution)
	{		
		vector<double>::iterator iter_basis_func_parameter = basisFuncParameter.begin();
		vector<cServer>::const_iterator const_iter_server = _server_vec.begin();
		vector<double> tem_value_parameter;
		for (;const_iter_server != _server_vec.end();const_iter_server++)
		{
			tem_value_parameter.push_back(step_size * (max_profit - getStateValueBasisFunc(_server_vec)) * (const_iter_server->getcpuResidual() * const_iter_server->getmemResidual() * const_iter_server->getdiskResidual()/const_iter_server->getcpuCapacity()/const_iter_server->getmemCapacity()/const_iter_server->getdiskCapacity()));
		}

		vector<double>::iterator iter_tem_value_parameter = tem_value_parameter.begin();
		for (;iter_basis_func_parameter != basisFuncParameter.end();iter_basis_func_parameter++,iter_tem_value_parameter++)
		{
			*iter_basis_func_parameter += *iter_tem_value_parameter;
		}
		
		//update state value and corresponding state policy
		if (request->getID() == 1)
		{
			map<string,int>::iterator find_iter_policy_counting = policy_counting.find(name_optimal_policy);

			if (find_iter_policy_counting != policy_counting.end())
			{
				policy_counting[name_optimal_policy]++;
			}
			else
			{
				policy_counting.insert(make_pair(name_optimal_policy,1));
			}
		}

	}

	//the arriving will not be accepted
	return accepted_arriving_request;
}

void outputResultsBasisFunct()
{
	ofstream output_file;

	output_file.open("output.txt",ios::app);
	double state_value = 0;

	vector<double>::const_iterator const_iter_basis_func_para = basisFuncParameter.begin();
	for (;const_iter_basis_func_para != basisFuncParameter.end();const_iter_basis_func_para++)
	{
		state_value += (54 * 80 * 500) * (*const_iter_basis_func_para);
	}

	output_file<<"The value of V(s_0) is "<<state_value<<endl;

	map<string,int>::const_iterator const_iter_policy_counting = policy_counting.begin();
	for (;const_iter_policy_counting != policy_counting.end();const_iter_policy_counting++)
	{
		output_file<<const_iter_policy_counting->first<<"--"<<const_iter_policy_counting->second<<endl;
	}

	output_file<<"The average number of accepted requests is "<<accepted_requests_num/(total_request * sample_request_num)<<endl;
	output_file<<"\n"<<endl;

	output_file.close();
	return ;
}