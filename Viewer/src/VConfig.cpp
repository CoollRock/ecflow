//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#include "VConfig.hpp"

#include "ServerFilter.hpp"
#include "VFilter.hpp"

//==============================================
//
// VConfig
//
//==============================================

VConfig::VConfig()
{
	server_=new ServerFilter(this);
	state_=new StateFilter(this);
	attr_=new AttributeFilter(this);
	icon_=new IconFilter(this);
}

VConfig::~VConfig()
{
	delete server_;
	delete state_;
	delete attr_;
	delete icon_;
}

void VConfig::changed(ServerFilter* f)
{
	for(std::vector<VConfigObserver*>::iterator it=observers_.begin(); it != observers_.end(); it++)
	{
		(*it)->notifyConfigChanged(f);
	}
}

void VConfig::changed(StateFilter* f)
{
	for(std::vector<VConfigObserver*>::iterator it=observers_.begin(); it != observers_.end(); it++)
	{
		(*it)->notifyConfigChanged(f);
	}
}

void VConfig::changed(IconFilter* f)
{
	for(std::vector<VConfigObserver*>::iterator it=observers_.begin(); it != observers_.end(); it++)
	{
		(*it)->notifyConfigChanged(f);
	}
}

void VConfig::changed(AttributeFilter* f)
{
	for(std::vector<VConfigObserver*>::iterator it=observers_.begin(); it != observers_.end(); it++)
	{
		(*it)->notifyConfigChanged(f);
	}
}

void VConfig::addObserver(VConfigObserver* obs)
{
	observers_.push_back(obs);
}

void  VConfig::removeObserver(VConfigObserver* obs)
{
	std::vector<VConfigObserver*>::iterator it=std::find(observers_.begin(),observers_.end(),obs);
	if(it != observers_.end())
		observers_.erase(it);
}
