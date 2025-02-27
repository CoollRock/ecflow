//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision: #24 $
//
// Copyright 2009-2017 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
// Description :
//============================================================================

#include <boost/test/unit_test.hpp>

// **************************************************************************************************************
// IMPORTANT: Str:split uses StringSplitter:
//            to run these tests, please switch off Str.cpp::USE_STRINGSPLITTER
// **************************************************************************************************************
//#define STRING_SPLIT_IMPLEMENTATIONS_PERF_CHECK_ 1;

#ifdef STRING_SPLIT_IMPLEMENTATIONS_PERF_CHECK_
#include <iostream>
#include <fstream>

#include <boost/timer/timer.hpp>

#include "Str.hpp"
#include "StringSplitter.hpp"
#include "File.hpp"

using namespace std;
using namespace ecf;
using namespace boost;
#endif


BOOST_AUTO_TEST_SUITE( CoreTestSuite )

#ifdef STRING_SPLIT_IMPLEMENTATIONS_PERF_CHECK_

std::vector<std::string> split_using_getline(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter)) { tokens.push_back(token);}
   return tokens;
}

BOOST_AUTO_TEST_CASE( test_str_split_perf )
{
   cout << "ACore:: ...test_str_split_perf\n";
//   Time for istreamstream 1000000 times = 2.59404
//   Time for std::getline 1000000 times = 1.83148
//   Time for boost::split 1000000 times = 1.18149
//   Time for Str::split_orig 1000000 times = 1.3015
//   Time for make_split_iterator::split 1000000 times = 4.1821
//   Time for boost::string_view 1000000 times =  0.975816
//   Time for boost::string_view(2) 1000000 times = 0.780003

   std::string line = "This is a long string that is going to be used to test the performance of splitting with different Implementations the fastest times wins ";
   size_t times = 1000000;
   cout << " This test will split a line " << times << " times: '" << line << "'\n";

   string reconstructed;reconstructed.reserve(line.size());

   { // istreamstream    https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
      boost::timer::cpu_timer timer;  // measures CPU, replace with cpu_timer with boost > 1.51, measures cpu & elapsed
      for (size_t i = 0; i < times; i++) {
         std::istringstream iss(line);
         std::vector<std::string> result((std::istream_iterator<std::string>(iss)),
                                          std::istream_iterator<std::string>());
         reconstructed.clear();
         for(const std::string& s: result) { reconstructed += s;  reconstructed += " "; }
      }
      cout << " Time for istreamstream " << times << "                 times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(line==reconstructed,"\n'" << line << "'\n'" << reconstructed << "'");
   }

   { // split using std::get_line    https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
      boost::timer::cpu_timer timer;  // measures CPU, replace with cpu_timer with boost > 1.51, measures cpu & elapsed
      for (size_t i = 0; i < times; i++) {
         std::vector<std::string> result = split_using_getline(line,' ');
         reconstructed.clear();
         for(const std::string& s: result) { reconstructed += s;  reconstructed += " "; }
      }
      cout << " Time for std::getline " << times << "                  times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(line==reconstructed,"\n'" << line << "'\n'" << reconstructed << "'");
   }


   { // BOOST SPLIT
      std::vector<std::string> result;
      boost::timer::cpu_timer timer;  // measures CPU, replace with cpu_timer with boost > 1.51, measures cpu & elapsed
      for (size_t i = 0; i < times; i++) {
         result.clear();
         //boost::algorithm::split(result, line, boost::is_any_of(" \t"),boost::algorithm::token_compress_on);
         boost::split(result, line, [](char c){return c == ' ';});

         reconstructed.clear();
         for(const std::string& s: result) { reconstructed += s;  reconstructed += " "; }
      }
      cout << " Time for boost::split " << times << "                  times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      //BOOST_CHECK_MESSAGE(line==reconstructed,"\n'" << line << "'\n'" << reconstructed << "'"); // add extra space
   }

   { // Str::split_orig
      std::vector<std::string> result;
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {
         result.clear(); Str::split_orig(line,result);

         reconstructed.clear();
         for(const std::string& s: result) { reconstructed += s;  reconstructed += " "; }
      }
      cout << " Time for Str::split_orig " << times << "               times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(line==reconstructed," error");
   }
   { // Str::split_orig1
      std::vector<std::string> result;
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {
         result.clear(); Str::split_orig1(line,result);

         reconstructed.clear();
         for(const std::string& s: result) { reconstructed += s;  reconstructed += " "; }
      }
      cout << " Time for Str::split_orig1 " << times << "              times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(line==reconstructed," error");
   }
   { // Str::split_using_string_view2
      std::vector<std::string> result;
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {
         result.clear(); Str::split_using_string_view2(line,result);

         reconstructed.clear();
         for(const std::string& s: result) { reconstructed += s;  reconstructed += " "; }
      }
      cout << " Time for Str::split_using_string_view2 " << times << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(line==reconstructed," error");
   }
   { // Str::split_using_string_view
      std::vector<std::string> result;
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {
         result.clear(); Str::split_using_string_view(line,result);

         reconstructed.clear();
         for(const std::string& s: result) { reconstructed += s;  reconstructed += " "; }
      }
      cout << " Time for Str::split_using_string_view " << times << "  times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(line==reconstructed," error");
   }

   typedef boost::split_iterator<string::const_iterator> split_iter_t;
   { // boost::make_split_iterator
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {

         split_iter_t tokens = Str::make_split_iterator(line);

         std::stringstream ss;
         for(; !tokens.eof(); ++tokens ) {
             boost::iterator_range<string::const_iterator> range = *tokens;
             ss << range << " ";
         }
         reconstructed = ss.str();
      }
      cout << " Time for make_split_iterator::split " << times << "    times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      // BOOST_CHECK_MESSAGE(line==reconstructed,"\n'" << line << "'\n'" << reconstructed << "'");
   }

   { // boost::string_view
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {
         StringSplitter string_splitter(line);

         reconstructed.clear();
         while(!string_splitter.finished())  {
            boost::string_view sv = string_splitter.next();
            reconstructed += std::string(sv.begin(),sv.end());
            reconstructed += " ";
         }
      }
      cout << " Time for boost::string_view " << times << "            times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(line==reconstructed,"\n'" << line << "'\n'" << reconstructed << "'");
   }

   { // boost::string_view
      boost::timer::cpu_timer timer;
      std::vector<boost::string_view> result;
      for (size_t i = 0; i < times; i++) {

         result.clear();
         StringSplitter::split2(line,result);

         reconstructed.clear();
         for( const auto& s: result) {
            reconstructed += std::string(s.begin(),s.end());
            reconstructed += " ";
         }
      }
      cout << " Time for boost::string_view(2) " << times << "         times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(line==reconstructed,"\n'" << line << "'\n'" << reconstructed << "'");
   }
}

BOOST_AUTO_TEST_CASE( test_str_split_perf_with_file )
{
   cout << "ACore:: ...test_str_split_perf_with_file\n";
//   Time for istreamstream 2001774 times = 1.81123
//   Time for std::getline 2001774 times = 2.89138
//   Time for boost::split 2001774 times = 1.98556
//   Time for Str::split_orig 2001774 times = 0.947959
//   Time for boost::make_split_iterator 2001774 times = 3.921
//   Time for boost::string_view 2001774 times = 0.765805
//   Time for boost::string_view(2) 2001774 times = 0.752472

   // Now test performance of splitting with a big DEFS file
   char* ecf_test_defs_dir = getenv("ECF_TEST_DEFS_DIR");
   if (!ecf_test_defs_dir) {
      std::cout << "Ingoring test, since directory defined by environment variable(ECF_TEST_DEFS_DIR) is missing";
      return;
   }
   std::string path = std::string(ecf_test_defs_dir) + "/vsms2.31415.def";
   if (!fs::exists(path)) {
      std::cout << "Ingoring test, since file defined by environment variable(ECF_TEST_DEFS_DIR) " << path << " is missing";
      return;
   }

   std::vector<std::string> file_contents;
   if (File::splitFileIntoLines(path,file_contents,true/* ignore empty lines*/)) {

      cout << " This test will split each line in file " << path << "\n";

      std::vector<std::string> result;result.reserve(300);

      auto reconstruct_line = [](const std::vector<std::string>& result) {
         string reconstructed;
         for(const auto& s : result) {  reconstructed += s;  reconstructed += " "; }
      };


      { // istreamstream
         boost::timer::cpu_timer timer;
          for(size_t i = 0; i < file_contents.size(); i++) {
             std::istringstream iss(file_contents[i] );
             std::vector<std::string> result1((std::istream_iterator<std::string>(iss)),
                                              std::istream_iterator<std::string>());
             reconstruct_line(result1);
          }
          cout << " Time for istreamstream " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }
      { // std::getline
         boost::timer::cpu_timer timer;
          for(size_t i = 0; i < file_contents.size(); i++) {
             reconstruct_line( split_using_getline(file_contents[i],' '));
          }
          cout << " Time for std::getline " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }
      {
         boost::timer::cpu_timer timer;
         for(size_t i = 0; i < file_contents.size(); i++) {
            result.clear();
            //boost::algorithm::split(result,file_contents[i], boost::is_any_of(" \t"),boost::algorithm::token_compress_on);
            boost::split(result, file_contents[i], [](char c){return c == ' ';});

            reconstruct_line(result);
         }
         cout << " Time for boost::split " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }
      {
         boost::timer::cpu_timer timer;
         for(size_t i = 0; i < file_contents.size(); i++) {
            result.clear();
            Str::split_orig(file_contents[i],result);

            reconstruct_line(result);
         }
         cout << " Time for Str::split_orig " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }
      {
         boost::timer::cpu_timer timer;
         for(size_t i = 0; i < file_contents.size(); i++) {
            result.clear();
            Str::split_orig1(file_contents[i],result);

            reconstruct_line(result);
         }
         cout << " Time for Str::split_orig1 " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }
      {
         boost::timer::cpu_timer timer;
         for(size_t i = 0; i < file_contents.size(); i++) {
            result.clear();
            Str::split_using_string_view(file_contents[i],result);

            reconstruct_line(result);
         }
         cout << " Time for Str::split_using_string_view " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }
      {
         boost::timer::cpu_timer timer;
         for(size_t i = 0; i < file_contents.size(); i++) {
            result.clear();
            Str::split_using_string_view2(file_contents[i],result);

            reconstruct_line(result);
         }
         cout << " Time for Str::split_using_string_view2 " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }
      {
         typedef boost::split_iterator<string::const_iterator> split_iter_t;
         boost::timer::cpu_timer timer;
         for(size_t i = 0; i < file_contents.size(); i++) {

            std::stringstream ss;
            split_iter_t tokens = Str::make_split_iterator(file_contents[i]);

            for(; !tokens.eof(); ++tokens ) {
                boost::iterator_range<string::const_iterator> range = *tokens;
                ss << range << " ";
            }
            string reconstructed = ss.str();
         }
         cout << " Time for boost::make_split_iterator " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }

      {
         boost::timer::cpu_timer timer;
         for(size_t i = 0; i < file_contents.size(); i++) {

            StringSplitter string_splitter(file_contents[i]);

            string reconstructed;
            while(!string_splitter.finished())  {
               boost::string_view sv = string_splitter.next();
               reconstructed += std::string(sv.begin(),sv.end());
               reconstructed += " ";
            }
         }
         cout << " Time for boost::string_view " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }

      {
         std::vector<boost::string_view> result1;
         boost::timer::cpu_timer timer;
         for(size_t i = 0; i < file_contents.size(); i++) {

            StringSplitter::split2(file_contents[i],result1);

            string reconstructed;
            for(const auto& s: result1) {
               reconstructed += std::string(s.begin(),s.end());
               reconstructed += " ";
            }
            result1.clear();
         }
         cout << " Time for boost::string_view(2) " << file_contents.size() << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      }
   }
}

BOOST_AUTO_TEST_CASE( test_str_get_token_perf )
{
   cout << "ACore:: ...test_str_get_token_perf\n";

   std::vector<std::string> result;
   std::string line = "This is a long string that is going to be used to test the performance of splitting with different Implementations the fastest times wins ";
   size_t times = 250000;
   cout << " This test will split a line " << times << " times: '" << line << "'\n";

   Str::split_using_string_view(line,result);
   size_t result_size = result.size();
   std::string token;

   {
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {
         for(size_t r=0; r < result_size; r++){
            token.clear();
            (void) StringSplitter::get_token(line,r,token);
         }
      }
      cout << " Time for StringSplitter::get_token " << times << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(true,"Keep compiler happy");
   }
   {
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {
         for(size_t r=0; r < result_size; r++){
            token.clear();
            (void) Str::get_token(line,r,token);
         }
      }
      cout << " Time for Str::get_token            " << times << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(true,"Keep compiler happy");
   }
   {
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {
         for(size_t r=0; r <  result_size; r++){
            token.clear();
            (void) Str::get_token2(line,r,token);
         }
      }
      cout << " Time for Str::get_token2           " << times << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(true,"Keep compiler happy");
   }
   {
      boost::timer::cpu_timer timer;
      for (size_t i = 0; i < times; i++) {
         for(size_t r=0; r < result_size; r++){
            token.clear();
            (void) Str::get_token3(line,r,token);
         }
      }
      cout << " Time for Str::get_token3           " << times << " times = " << timer.format(3,Str::cpu_timer_format()) << "\n";
      BOOST_CHECK_MESSAGE(true,"Keep compiler happy");
   }
}

#endif

BOOST_AUTO_TEST_SUITE_END()
