/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #5 $
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
// Description
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace boost;
using namespace std;

//#define DEBUG_ME 1

class MyType {
public:
   MyType(std::string str) : mName(std::move(str)) {
#ifdef DEBUG_ME
      std::cout << "MyType::MyType " << mName << " my_int:" << my_int_ << '\n';
#endif
   }

   ~MyType() {
#ifdef DEBUG_ME
      std::cout << "MyType::~MyType " << mName << " my_int:" << my_int_ << '\n';
#endif
   }

   MyType(const MyType& other) : mName(other.mName) {
#ifdef DEBUG_ME
      std::cout << "MyType::MyType(const MyType&) " << mName << " my_int:" << my_int_ << '\n';
#endif
   }

   MyType(MyType&& other) noexcept : mName(std::move(other.mName)) { // vector needs noexcept to call move copy constructor during resize.
#ifdef DEBUG_ME
      std::cout << "MyType::MyType(MyType&&) " << mName << " my_int:" << my_int_ << '\n';
#endif
   }

   MyType& operator=(const MyType& other) {
      if (this != &other)  mName = other.mName;
#ifdef DEBUG_ME
      std::cout << "MyType::operator=(const MyType&) " << mName << " my_int:" << my_int_ << '\n';
#endif
      return *this;
   }

   MyType& operator=(MyType&& other) noexcept {
      if (this != &other)  mName = std::move(other.mName);
#ifdef DEBUG_ME
      std::cout << "MyType::operator=(MyType&&) " << mName << " my_int:" << my_int_ << '\n';
#endif
      return *this;
   }

   int my_int() const { return my_int_;}
   int* my_ptr() const { return my_ptr_;}

private:
   std::string mName;
   int my_int_{3};          //  show be applied for all constructor types
   int* my_ptr_{nullptr};   //  show be applied for all constructor types
};


BOOST_AUTO_TEST_SUITE( CoreTestSuite )

BOOST_AUTO_TEST_CASE( test_class_data_member_init )
{
   cout << "ACore:: ...test_class_data_member_init \n" ;

//    // MyType needs noexcept on move copy constructor, during vec resize.
//	std::vector<MyType> vec;
//	for(int i =0; i < 4; i++) {
//		//vec.push_back(MyType(boost::lexical_cast<string>(i)));
//		vec.emplace_back(boost::lexical_cast<string>(i));
//		cout << vec.size() << " " << vec.capacity() << endl;
//	}


   {
      MyType type("ABC");
      auto tmoved = std::move(type);
      BOOST_CHECK_MESSAGE(tmoved.my_int() == 3 ,"Error found");
      BOOST_CHECK_MESSAGE(tmoved.my_ptr() == nullptr ,"Error found");
   }
   {
      MyType type("ABC");
      auto tcopied = MyType(type);
      BOOST_CHECK_MESSAGE(tcopied.my_int() == 3 ,"Error found");
      BOOST_CHECK_MESSAGE(tcopied.my_ptr() == nullptr ,"Error found");
   }
   {
      MyType tassigned("XYZ");
      MyType type("ABC");
      tassigned = type;
      BOOST_CHECK_MESSAGE(tassigned.my_int() == 3 ,"Error found");
      BOOST_CHECK_MESSAGE(tassigned.my_ptr() == nullptr ,"Error found");
   }
   {
      MyType tassigned("XYZ");
      MyType temp("ABC");
      tassigned = std::move(temp);
      BOOST_CHECK_MESSAGE(tassigned.my_int() == 3 ,"Error found");
      BOOST_CHECK_MESSAGE(tassigned.my_ptr() == nullptr ,"Error found");
   }
}

BOOST_AUTO_TEST_SUITE_END()
