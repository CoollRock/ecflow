//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "UpdateTimer.hpp"

void UpdateTimer::drift(int dValSec, int maxValMin)
{
     double v=interval() + dValSec*1000;
     if( v > maxValMin*1000*60)
        v=maxValMin*1000*60;

     setInterval(v);
}

