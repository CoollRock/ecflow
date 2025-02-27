/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #10 $
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
#include <iostream>
#include <boost/test/unit_test.hpp>

#include "Flag.hpp"

using namespace std;
using namespace ecf;

BOOST_AUTO_TEST_SUITE( NodeTestSuite )

BOOST_AUTO_TEST_CASE( test_node_flags )
{
	cout << "ANode:: ...test_node_flags\n";

	Flag flag;
	std::string expected_flags =
	         "force_aborted,user_edit,task_aborted,edit_failed,ecfcmd_failed,killcmd_failed,statuscmd_failed,no_script,killed,status,late,message,by_rule,queue_limit,task_waiting,locked,zombie,no_reque,archived,restored,threshold,sigterm,log_error,checkpt_error";

	/// Set the flags
	std::vector<Flag::Type> flag_list =  Flag::list();
	for (auto & i : flag_list) {
	   flag.set( i );
	   BOOST_REQUIRE_MESSAGE(flag.is_set( i ), "Expected flag to be set");
	}
   BOOST_REQUIRE_MESSAGE(flag.to_string() == expected_flags, "Expected string '" << expected_flags << "' but found '" << flag.to_string() << "'");
	for (auto & i : flag_list) {
	   BOOST_REQUIRE_MESSAGE(flag.is_set( i ), "Expected flag to be set");
	}

	/// clears the flags
	for (auto & i : flag_list) {
	   flag.clear( i );
	   BOOST_REQUIRE_MESSAGE(!flag.is_set( i ), "Expected flag to be clear");
	}
	for (auto & i : flag_list) {
	   BOOST_REQUIRE_MESSAGE(!flag.is_set( i ), "Expected flag to be clear");
	}

	/// set all flags
 	for (auto & i : flag_list) { flag.set( i );}

	// reset, all flags should be clear
	flag.reset();
	for (auto & i : flag_list) {
 	 	BOOST_REQUIRE_MESSAGE(!flag.is_set( i ), "Expected flag to be clear");
	}
}

BOOST_AUTO_TEST_CASE( test_node_flags_parsing )
{
   cout << "ANode:: ...test_node_flags_parsing\n";

   /// Set the flags
   Flag flag;
   std::vector<Flag::Type> flag_list =  Flag::list();
   for (auto & i : flag_list) flag.set( i );

   Flag flag2;
   flag2.set_flag(flag.to_string());
   BOOST_REQUIRE_MESSAGE(flag == flag2, "Flags should be equal.\nflag1:" << flag.to_string() << "\nflag2:" << flag2.to_string());
}

BOOST_AUTO_TEST_SUITE_END()
