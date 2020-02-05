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
#include <algorithm>

#include "ClientToServerCmd.hpp"
#include "AbstractServer.hpp"
#include "AbstractClientEnv.hpp"
#include "CtsApi.hpp"
#include "Defs.hpp"
#include "Suite.hpp"
#include "SuiteChanged.hpp"
#include "Log.hpp"

using namespace ecf;
using namespace std;
using namespace boost;
namespace po = boost::program_options;

PathsCmd::PathsCmd(Api api,const std::string& absNodePath, bool force)
: api_(api),force_(force)
{
   if (!absNodePath.empty()) paths_.push_back(absNodePath);
}

std::ostream& PathsCmd::print(std::ostream& os) const
{
   return my_print(os,paths_);
}

std::ostream& PathsCmd::print_short(std::ostream& os) const
{
   std::vector<std::string> paths;
   if (!paths_.empty()) paths.emplace_back(paths_[0]);
   my_print_only(os,paths);
   if (paths_.size() > 1) os << " : truncated : " << paths_.size() -1 << " paths *not* shown";
   return os;
}

std::ostream& PathsCmd::print_only(std::ostream& os) const
{
   return my_print_only(os,paths_);
}

std::ostream& PathsCmd::print(std::ostream& os, const std::string& path) const
{
   std::vector<std::string> paths(1,path);
   return my_print(os,paths);
}

std::ostream& PathsCmd::my_print(std::ostream& os,const std::vector<std::string>& paths) const
{
   switch (api_) {
      case PathsCmd::SUSPEND:            return user_cmd(os,CtsApi::to_string(CtsApi::suspend(paths))); break;
      case PathsCmd::RESUME:             return user_cmd(os,CtsApi::to_string(CtsApi::resume(paths))); break;
      case PathsCmd::KILL:               return user_cmd(os,CtsApi::to_string(CtsApi::kill(paths))); break;
      case PathsCmd::STATUS:             return user_cmd(os,CtsApi::to_string(CtsApi::status(paths))); break;
      case PathsCmd::CHECK:              return user_cmd(os,CtsApi::to_string(CtsApi::check(paths))); break;
      case PathsCmd::EDIT_HISTORY:       return user_cmd(os,CtsApi::to_string(CtsApi::edit_history(paths))); break;
      case PathsCmd::ARCHIVE:            return user_cmd(os,CtsApi::to_string(CtsApi::archive(paths,force_))); break;
      case PathsCmd::RESTORE:            return user_cmd(os,CtsApi::to_string(CtsApi::restore(paths))); break;
      case PathsCmd::NO_CMD:       break;
      default: assert(false);break;
   }
   return os;
}
std::ostream& PathsCmd::my_print_only(std::ostream& os,const std::vector<std::string>& paths) const
{
   switch (api_) {
      case PathsCmd::SUSPEND:            os << CtsApi::to_string(CtsApi::suspend(paths)); break;
      case PathsCmd::RESUME:             os << CtsApi::to_string(CtsApi::resume(paths)); break;
      case PathsCmd::KILL:               os << CtsApi::to_string(CtsApi::kill(paths)); break;
      case PathsCmd::STATUS:             os << CtsApi::to_string(CtsApi::status(paths)); break;
      case PathsCmd::CHECK:              os << CtsApi::to_string(CtsApi::check(paths)); break;
      case PathsCmd::EDIT_HISTORY:       os << CtsApi::to_string(CtsApi::edit_history(paths)); break;
      case PathsCmd::ARCHIVE:            os << CtsApi::to_string(CtsApi::archive(paths,force_)); break;
      case PathsCmd::RESTORE:            os << CtsApi::to_string(CtsApi::restore(paths)); break;
      case PathsCmd::NO_CMD:       break;
      default: assert(false);break;
   }
   return os;
}


bool PathsCmd::equals(ClientToServerCmd* rhs) const
{
   auto* the_rhs = dynamic_cast< PathsCmd* > ( rhs );
   if ( !the_rhs ) return false;
   if (api_ != the_rhs->api()) return false;
   if (paths_ != the_rhs->paths()) return false;
   if (force_ != the_rhs->force()) return false;
   return UserCmd::equals(rhs);
}

bool PathsCmd::isWrite() const
{
   switch (api_) {
      case PathsCmd::SUSPEND:           return true;  break; // requires write privilege
      case PathsCmd::RESUME:            return true;  break; // requires write privilege
      case PathsCmd::KILL:              return true;  break; // requires write privilege, modifies Flag::KILLCMD_FAILED
      case PathsCmd::STATUS:            return true;  break; // requires write privilege, modifies Flag::STATUSCMD_FAILED
      case PathsCmd::CHECK:             return false; break; // read only
      case PathsCmd::EDIT_HISTORY:      return false; break; // read only
      case PathsCmd::ARCHIVE:           return true;  break; // requires write privilege
      case PathsCmd::RESTORE:           return true;  break; // requires write privilege
      case PathsCmd::NO_CMD: break;
      default: break;
   }
   assert(false);
   return false;
}

const char* PathsCmd::theArg() const
{
   switch (api_) {
      case PathsCmd::SUSPEND:            return CtsApi::suspend_arg(); break;
      case PathsCmd::RESUME:             return CtsApi::resume_arg(); break;
      case PathsCmd::KILL:               return CtsApi::kill_arg(); break;
      case PathsCmd::STATUS:             return CtsApi::statusArg(); break;
      case PathsCmd::CHECK:              return CtsApi::check_arg(); break;
      case PathsCmd::EDIT_HISTORY:       return CtsApi::edit_history_arg(); break;
      case PathsCmd::ARCHIVE:            return CtsApi::archive_arg(); break;
      case PathsCmd::RESTORE:            return CtsApi::restore_arg(); break;
      case PathsCmd::NO_CMD: break;
      default: break;
   }
   assert(false);
   return nullptr;
}

STC_Cmd_ptr PathsCmd::doHandleRequest(AbstractServer* as) const
{
   // LogTimer timer(" PathsCmd::doHandleRequest");

   defs_ptr defs = as->defs();
   std::stringstream ss;
   switch (api_) {

      case PathsCmd::SUSPEND: {
         use_EditHistoryMgr_ = false; // will add edit history ourselves. Quicker than EditHistoryMgr when we have > 200000 paths

         as->update_stats().node_suspend_++;
         size_t vec_size = paths_.size();
         for(size_t i = 0; i < vec_size; i++) {
            node_ptr theNode = defs->findAbsNode(paths_[i]);
            if (!theNode.get()) {
               ss << "PathsCmd:Suspend: Could not find node at path '" << paths_[i] << "'\n";
               LOG(Log::ERR,"Suspend: Could not find node at path " << paths_[i]);
               continue;
            }
            SuiteChanged0 changed(theNode);
            theNode->suspend();
            theNode->flag().set(ecf::Flag::MESSAGE);
            add_edit_history(as,paths_[i]);
            assert(isWrite()); // should only add edit history for write-able commands
         }
         break;
      }

      case PathsCmd::RESUME: {
         use_EditHistoryMgr_ = false; // will add edit history ourselves. Quicker than EditHistoryMgr when we have > 200000 paths

         // At the end of resume, we need to traverse node tree, and do job submission
         as->update_stats().node_resume_++;
         size_t vec_size = paths_.size();
         for(size_t i = 0; i < vec_size; i++) {
            node_ptr theNode = defs->findAbsNode(paths_[i]);
            if (!theNode.get()) {
               ss << "PathsCmd:Resume: Could not find node at path '" << paths_[i] << "'\n";
               LOG(Log::ERR,"Resume: Could not find path " << paths_[i]);
               continue;
            }
            SuiteChanged0 changed(theNode);
            theNode->resume();
            theNode->flag().set(ecf::Flag::MESSAGE);
            add_edit_history(as,paths_[i]);
            assert(isWrite()); // should only add edit history for write-able commands

            as->increment_job_generation_count(); // in case we throw below
         }
         break;
      }

      case PathsCmd::KILL: {
         as->update_stats().node_kill_++;
         size_t vec_size = paths_.size();
         for(size_t i = 0; i < vec_size; i++) {
            node_ptr theNode = find_node_for_edit_no_throw(as,paths_[i]);
            if (!theNode.get()) {
               ss << "PathsCmd:Kill: Could not find node at path '" << paths_[i] << "'\n";
               LOG(Log::ERR,"Kill: Could not find node at path " << paths_[i]);
               continue;
            }
            SuiteChanged0 changed(theNode);
            theNode->kill();  // this can throw std::runtime_error
         }
         break;
      }


      case PathsCmd::EDIT_HISTORY: {
         as->update_stats().node_edit_history_++;
         if (paths_.empty()) throw std::runtime_error( "No paths specified for edit history") ;
         // Only first path used
         return PreAllocatedReply::string_vec_cmd( as->defs()->get_edit_history(paths_[0]) );
      }

      case PathsCmd::ARCHIVE: {
         use_EditHistoryMgr_ = false; // will add edit history ourselves

         as->update_stats().node_archive_++;
         if (paths_.empty()) throw std::runtime_error( "No paths specified for archive") ;

         // make sure paths don't overlap, Should not find same path up the hierarchy, which is also in paths_
         std::vector<NodeContainer*> containers_to_archive; containers_to_archive.reserve(paths_.size());
         for(size_t i = 0; i < paths_.size(); i++) {
            node_ptr theNode = defs->findAbsNode(paths_[i]);
            if (!theNode.get()) {
               ss << "PathsCmd:ARCHIVE: Could not find node at path '" << paths_[i] << "'\n";
               LOG(Log::ERR,"ARCHIVE: Could not find node at path " << paths_[i]);
               continue;
            }
            NodeContainer* container = theNode->isNodeContainer();
            if (!container) continue;

            bool unique = true;
            Node* parent = theNode->parent();
            while(parent) {
               std::string abs_node_path = parent->absNodePath();
               if (find(paths_.begin(),paths_.end(),abs_node_path) != paths_.end()) {
                  unique = false; break; // parent also in paths, so don't archive child
               }
               parent = parent->parent();
            }
            if (unique) containers_to_archive.push_back(container);
         }

         size_t vec_size = containers_to_archive.size();
         for(size_t i = 0; i < vec_size; i++) {
            NodeContainer* the_container = containers_to_archive[i];

            if (!force_) DeleteCmd::check_for_active_or_submitted_tasks(as, the_container);
            else         as->zombie_ctrl().add_user_zombies(the_container,CtsApi::archive_arg());

            SuiteChanged1 changed(the_container->suite());

            the_container->flag().set(ecf::Flag::MESSAGE);
            add_edit_history(as,the_container->absNodePath());
            assert(isWrite()); // should only add edit history for write-able commands

            the_container->archive();  // this can throw std::runtime_error
         }
         break;
      }

      case PathsCmd::RESTORE: {
         as->update_stats().node_restore_++;
         if (paths_.empty()) throw std::runtime_error( "No paths specified for restore");
         size_t vec_size = paths_.size();
         for(size_t i = 0; i < vec_size; i++) {
            node_ptr theNode = find_node_for_edit_no_throw(as,paths_[i]);
            if (!theNode.get()) {
               ss << "PathsCmd:RESTORE: Could not find node at path '" << paths_[i] << "'\n";
               LOG(Log::ERR,"RESTORE: Could not find node at path " << paths_[i]);
               continue;
            }
            NodeContainer* container = theNode->isNodeContainer();
            if (!container) continue;

            SuiteChanged1 changed(container->suite());
            container->restore();  // this can throw std::runtime_error
         }
         break;
      }

      case PathsCmd::STATUS: {
         as->update_stats().node_status_++;
         size_t vec_size = paths_.size();
         for(size_t i = 0; i < vec_size; i++) {
            node_ptr theNode = find_node_for_edit_no_throw(as,paths_[i]);
            if (!theNode.get()) {
               ss << "PathsCmd:Status: Could not find node at path '" << paths_[i] << "'\n";
               LOG(Log::ERR,"Status: Could not find node at path " << paths_[i]);
               continue;
            }
            if (!theNode->suite()->begun()) {
               std::stringstream mss;
               mss << "Status failed. For " << paths_[i] << " The suite " << theNode->suite()->name() << " must be 'begun' first\n";
               throw std::runtime_error( mss.str() ) ;
            }
            SuiteChanged0 changed(theNode);
            theNode->status();   // this can throw std::runtime_error
         }
         break;
      }

      case PathsCmd::CHECK:  {
         as->update_stats().check_++;

         if (  paths_.empty() ) {
            // check all the defs,
            std::string error_msg,warning_msg;
            if (!defs->check(error_msg,warning_msg)) {
               error_msg += "\n";
               error_msg += warning_msg;
               return PreAllocatedReply::string_cmd(error_msg);
            }
            return PreAllocatedReply::string_cmd(warning_msg); // can be empty
         }
         else {
            std::string acc_warning_msg;
            size_t vec_size = paths_.size();
            for(size_t i = 0; i < vec_size; i++) {

               node_ptr theNodeToCheck = defs->findAbsNode(paths_[i]);
               if (!theNodeToCheck.get()) {
                  ss << "PathsCmd:Check: Could not find node at path '" << paths_[i] << "'\n";
                  LOG(Log::ERR,"Check: Could not find node at path " << paths_[i]);
                  continue;
               }

               std::string error_msg,warning_msg;
               if (!theNodeToCheck->check(error_msg,warning_msg)) {
                  error_msg += "\n";
                  error_msg += warning_msg;
                  return PreAllocatedReply::string_cmd(error_msg);
               }
               acc_warning_msg += warning_msg;
            }
            std::string paths_not_fnd_error_msg = ss.str();
            if (!paths_not_fnd_error_msg.empty()) throw std::runtime_error( paths_not_fnd_error_msg );
            return PreAllocatedReply::string_cmd(acc_warning_msg);
         }
         break;
      }

      case PathsCmd::NO_CMD: assert(false); break;

      default: assert(false); break;
   }

   std::string error_msg = ss.str();
   if (!error_msg.empty()) {
      throw std::runtime_error( error_msg ) ;
   }

   if ( PathsCmd::RESUME  == api_) {
      // After resume we need to do job submission.
      return doJobSubmission(as);
   }

   return PreAllocatedReply::ok_cmd();
}

bool PathsCmd::authenticate(AbstractServer* as, STC_Cmd_ptr& cmd) const
{
   return do_authenticate(as,cmd,paths_);
}

static const char* get_check_desc() {
   return
         "Checks the expression and limits in the server. Will also check trigger references.\n"
         "Trigger expressions that reference paths that don't exist, will be reported as errors.\n"
         "(Note: On the client side unresolved paths in trigger expressions must\n"
         "have an associated 'extern' specified)\n"
         "  arg = [ _all_ | / | list of node paths ]\n"
         "Usage:\n"
         "  --check=_all_           # Checks all the suites\n"
         "  --check=/               # Checks all the suites\n"
         "  --check=/s1 /s2/f1/t1   # Check suite /s1 and task t1"
         ;
}

static const char* get_kill_desc() {
   return
            "Kills the job associated with the node.\n"
            "If a family or suite is selected, will kill hierarchically.\n"
            "Kill uses the ECF_KILL_CMD variable. After variable substitution it is invoked as a command.\n"
            "The command should be written in such a way that the output is written to %ECF_JOB%.kill\n"
            "as this allow the --file command to report the output: .e.e.\n"
            " /home/ma/emos/bin/ecfkill %USER% %HOST% %ECF_RID% %ECF_JOB% > %ECF_JOB%.kill 2>&1::\n"
            "Usage::\n"
            "   --kill=/s1/f1/t1 /s1/f2/t2 # kill the jobs for tasks t1 and t2\n"
            "   --file=/s1/f1/t1 kill      # write to standard out the '.kill' file for task /s1/f1/t1"
            ;
}
const char*  get_status_desc(){
   return
            "Shows the status of a job associated with a task.\n"
            "If a family or suite is selected, will invoke status command hierarchically.\n"
            "Status uses the ECF_STATUS_CMD variable. After variable substitution it is invoked as a command.\n"
            "The command should be written in such a way that the output is written to %ECF_JOB%.stat\n"
            "This will allow the output of status command to be shown by the --file command\n"
            "i.e /home/ma/emos/bin/ecfstatus  %USER% %HOST% %ECF_RID% %ECF_JOB% > %ECF_JOB%.stat 2>&1::\n"
            "Usage::\n"
            "   --status=/s1/f1/t1     # ECF_STATUS_CMD should output to %ECF_JOB%.stat\n"
            "   --file=/s1/f1/t1 stat  # Return contents of %ECF_JOB%.stat file"
            ;
}
const char* get_edit_history_desc(){
   return
            "Returns the edit history associated with a Node.\n"
             "Usage::\n"
            "   --edit_history=/s1/f1/t1\n"
            ;
}
const char* suspend_desc(){
   return
            "Suspend the given node. This prevents job generation for the given node, or any child node.\n"
            "Usage::\n"
            "   --suspend=/s1/f1/t1   # suspend task s1/f1/t1\n"
            "   --suspend=/s1 /s2     # suspend suites /s1 and /s2\n"
            ;
}
const char* resume_desc(){
   return
            "Resume the given node. This allows job generation for the given node, or any child node.\n"
            "Usage::\n"
            "   --resume=/s1/f1/t1   # resume task s1/f1/t1\n"
            "   --resume=/s1 /s2     # resume suites /s1 and /s2\n"
            ;
}
const char* archive_desc(){
   return
            "Archives suite or family nodes *IF* they have child nodes(otherwise does nothing).\n"
            "Saves the suite/family nodes to disk, and then removes the child nodes from the definition.\n"
            "This saves memory in the server, when dealing with huge definitions that are not needed.\n"
            "It improves time taken to checkpoint and reduces network bandwidth.\n"
            "If the node is re-queued or begun, the child nodes are automatically restored.\n"
            "Use --restore to reload the archived nodes manually\n"
            "Care must be taken if you have trigger reference to the archived nodes\n"
            "The nodes are saved to ECF_HOME/ECF_NAME.check, where '/' has been replace with ':' in ECF_NAME\n\n"
            "Nodes like (family and suites) can also to automatically archived by using,\n"
            "the 'autoarchive' attribute. The attribute is only applied once the node is complete\n\n"
            "suite autoarchive\n"
            " family f1\n"
            "    autoarchive +01:00 # archive one hour after complete\n"
            "    task t1\n"
            " endfamily\n"
            " family f2\n"
            "     autoarchive 01:00 # archive at 1 am in morning after complete\n"
            "    task t1\n"
            " endfamily\n"
            " family f3\n"
            "    autoarchive 10     # archive 10 days after complete\n"
            "    task t1\n"
            " endfamily\n"
            " family f4\n"
            "    autoarchive 0      # archive immediately (upto 60s) after complete\n"
            "    task t1\n"
            "  endfamily\n"
            "endsuite\n"
            "\n"
            "Usage::\n"
            "   --archive=/s1           # archive suite s1\n"
            "   --archive=/s1/f1 /s2    # archive family /s1/f1 and suite /s2\n"
            "   --archive=force /s1 /s2 # archive suites /s1,/s2 even if they have active tasks"
            ;
}
const char* restore_desc(){
   return
         "Manually restore archived nodes.\n"
         "Restore will fail if:\n"
         "  - Node has not been archived\n"
         "  - Node has children, (since archived nodes have no children)\n"
         "  - If the file ECF_HOME/ECF_NAME.check does not exist\n"
         "Nodes can be restored manually(as in this command) but also automatically\n\n"
         "Automatic restore is done using the 'autorestore' attribute.\n"
         "Once the node containing the 'autorestore' completes, the attribute is applied\n\n"
         " suite s\n"
         "   family farchive_now\n"
         "     autoarchive 0      # archive immediately after complete, may have to wait 60 seconds\n"
         "     task t1\n"
         "   endfamily\n"
         "   family frestore_from_task\n"
         "     task t1\n"
         "        edit SLEEP 60\n"
         "        autorestore ../farchive_now  # call autorestore when t1 completes\n"
         "   endfamily\n"
         " endsuite\n\n"
         "In this example when '/s/frestore_from_task/t1' completes, it will check to see 'farchive_now'\n"
         "is complete and archived, in which case it will be automatically restored\n"
         "Usage::\n"
         "   --restore=/s1/f1   # restore family /s1/f1\n"
         "   --restore=/s1 /s2  # restore suites /s1 and /s2"
         ;
}

void PathsCmd::addOption(boost::program_options::options_description& desc) const
{
   switch (api_) {
      case PathsCmd::CHECK:{
         desc.add_options()(CtsApi::check_arg(),po::value< vector<string> >()->multitoken(),get_check_desc());
         break;
      }
      case PathsCmd::SUSPEND:{
         desc.add_options()( CtsApi::suspend_arg(), po::value< vector<string> >()->multitoken(),suspend_desc());
         break;
      }
      case PathsCmd::RESUME: {
         desc.add_options()( CtsApi::resume_arg(), po::value< vector<string> >()->multitoken(),resume_desc());
         break;
      }
      case PathsCmd::KILL: {
         desc.add_options()( CtsApi::kill_arg(), po::value< vector<string> >()->multitoken(),get_kill_desc());
         break;
      }
      case PathsCmd::STATUS: {
         desc.add_options()( CtsApi::statusArg(), po::value< vector<string> >()->multitoken(), get_status_desc());
         break;
      }
      case PathsCmd::EDIT_HISTORY: {
         desc.add_options()( CtsApi::edit_history_arg(), po::value< vector<string> >()->multitoken(), get_edit_history_desc());
         break;
      }
      case PathsCmd::ARCHIVE: {
         desc.add_options()( CtsApi::archive_arg(), po::value< vector<string> >()->multitoken(), archive_desc());
         break;
      }
      case PathsCmd::RESTORE: {
          desc.add_options()( CtsApi::restore_arg(), po::value< vector<string> >()->multitoken(), restore_desc());
          break;
       }
      case PathsCmd::NO_CMD:  assert(false); break;
      default: assert(false); break;
   }
}

void PathsCmd::create(   Cmd_ptr& cmd,
         boost::program_options::variables_map& vm,
         AbstractClientEnv* ac ) const
{
   assert( api_ != PathsCmd::NO_CMD);

   vector<string> args = vm[  theArg() ].as< vector<string> >();
   if (ac->debug()) dumpVecArgs( theArg(), args);

   std::vector<std::string> options,paths;
   split_args_to_options_and_paths(args,options,paths); // relative order is still preserved

   bool force = false;
   if (api_ == PathsCmd::CHECK) {

      bool all = false;
      size_t vec_size = options.size();
      for(size_t i = 0; i < vec_size; i++) {
         if (args[i] == "_all_") all = true;
      }
      if (!all && paths.empty()) {
         std::stringstream ss;
         ss << "Check: Please specify one of [ _all_ | / | /<path/to/anode> ]. Paths must begin with a leading '/' character\n";
         throw std::runtime_error( ss.str() );
      }
      if (paths.size() == 1 && paths[0] == "/") {
         // treat as _all_
         paths.clear();
      }
   }
   else {
      if (paths.empty()) {
         std::stringstream ss;
         ss << theArg() << ":  No paths specified. Paths must begin with a leading '/' character\n";
         throw std::runtime_error( ss.str() );
      }
   }

   cmd = std::make_shared<PathsCmd>( api_ , paths, force);
}

std::ostream& operator<<(std::ostream& os, const PathsCmd& c) { return c.print(os); }
