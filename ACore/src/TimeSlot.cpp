/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #16 $ 
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description :
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8

#include <ostream>
#include <boost/lexical_cast.hpp>

#include "TimeSlot.hpp"
#include "Str.hpp"
#include "Serialization.hpp"

using namespace boost::posix_time;

namespace ecf {

///////////////////////////////////////////////////////////////////////////////////////////

bool TimeSlot::operator<(const TimeSlot& rhs) const
{
   if (h_ <  rhs.hour()) return true;
   if (h_ == rhs.hour()) {
      return m_ < rhs.minute();
   }
   return false;
}

bool TimeSlot::operator>(const TimeSlot& rhs) const
{
   if (h_ >  rhs.hour()) return true;
   if (h_ == rhs.hour()) {
      return m_ > rhs.minute();
   }
   return false;
}

bool TimeSlot::operator<=( const TimeSlot& rhs ) const
{
   if (operator<(rhs)) return true;
   return operator==(rhs);
}

bool TimeSlot::operator>=( const TimeSlot& rhs ) const
{
   if (operator>(rhs)) return true;
   return operator==(rhs);
}

std::string TimeSlot::toString() const
{
   std::string ret;
   write(ret);
   return ret;
}

void TimeSlot::write(std::string& ret) const
{
   if (isNULL()) { ret += "00:00"; return;}

   if (h_ < 10)  ret += "0";
   ret += boost::lexical_cast<std::string>(h_);

   ret += Str::COLON();
   if (m_ < 10) ret += "0";
   ret += boost::lexical_cast<std::string>(m_);
}

boost::posix_time::time_duration TimeSlot::duration() const
{
	assert(!isNULL());
	return boost::posix_time::time_duration( hours(h_) + minutes(m_) ) ;
}

std::ostream& operator<<(std::ostream& os, const TimeSlot* d) {
	if (d) {
	   std::string s;
	   d->print(s);
	   os << s;
	   return os;
	}
	return os << "TimeSlot == NULL";
}
std::ostream& operator<<(std::ostream& os, const TimeSlot& d){std::string s; d.print(s); os << s; return os; }


template<class Archive>
void TimeSlot::serialize(Archive & ar) {
   ar( CEREAL_NVP(h_),
       CEREAL_NVP(m_ )
     );
}
CEREAL_TEMPLATE_SPECIALIZE(TimeSlot);

}
