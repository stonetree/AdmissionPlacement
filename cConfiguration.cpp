#include "StdAfx.h"
#include "cConfiguration.h"


cConfiguration::cConfiguration(void)
{
}


cConfiguration::~cConfiguration(void)
{
}

cConfiguration::cConfiguration(const cConfiguration& _config)
{
	this->operator=(_config);
}

cConfiguration& cConfiguration::operator=(const cConfiguration& _config)
{
	if (this != &_config)
	{
		conf_workload_rate = _config.conf_workload_rate;
		conf_low_arrival_rate = _config.conf_low_arrival_rate;
		conf_high_arrival_rate = _config.conf_high_arrival_rate;
		conf_xhigh_arrival_rate = _config.conf_xhigh_arrival_rate;
		conf_runing_policy = _config.conf_runing_policy;
		num_sample_path = _config.num_sample_path;
		num_request = _config.num_request;
	}

	return *this;
}