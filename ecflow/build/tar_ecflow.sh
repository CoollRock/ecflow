#!/bin/sh

## Copyright 2009-2012 ECMWF. 
## This software is licensed under the terms of the Apache Licence version 2.0 
## which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
## In applying this licence, ECMWF does not waive the privileges and immunities 
## granted to it by virtue of its status as an intergovernmental organisation 
## nor does it submit to any jurisdiction. 

# Tar up ecflow  
# Assumes $WK(root workspace) is defined

set -e # stop the shell on first error 
set -u # fail when using an undefined variable 

# ===============================================================================
# Test/data/ECF_HOME is used by Test to recursively generate directory structure
# and populate with defs file, and job output, etc, hence exclude this directory
# ===============================================================================
cd $WK
rm -rf Test/data/ECF_HOME_debug
rm -rf Test/data/ECF_HOME_release
rm -rf view/data/ECF_HOME_debug
rm -rf view/data/ECF_HOME_release
rm -rf AParser/test/data/single_defs/mega.def_log
rm -rf Pyext/test.def

# ================================================================================
# Remove generated files before taring ecFlow  
# Be careful with *.txt extension. i.e do not to delete CMakeList.txt file.
# ================================================================================
cd $WK
 
# exclude log files from hidden files/directories like .metadata
find . \( ! -regex '.*/\..*' \) -type f -name  \*.log -exec rm -rf \*.log {} \; -print

find . -name \*~ -exec rm -rf \*~ {} \; -print
find . -name \*.mk -exec rm -rf \*.mk {} \; -print
find . -name \*.so -exec rm -rf \*.so {} \; -print
find . -name \*.tmp -exec rm -rf \*.tmp {} \; -print
find . -name \*.job\* -exec rm -rf \*.job\* {} \; -print
find . -name \*.check -exec rm -rf \*.check {} \; -print
find . -name \*.flat -exec rm -rf \*.flat {} \; -print
find . -name \*.depth -exec rm -rf \*.depth {} \; -print
find . -name \*.out -exec rm -rf \*.out {} \; -print
find . -name \*.pyc -exec rm -rf \*.pyc {} \; -print
find . -name t\*.1 -exec rm -rf t\*.1 {} \; -print
find . -name gmon.out -exec rm -rf gmon.out {} \; -print
find . -name gnuplot.dat -exec rm -rf gnuplot.dat {} \; -print
find . -name gnuplot.script -exec rm -rf gnuplot.script {} \; -print
find . -name ecflow.html -exec rm -rf ecflow.html {} \; -print
find . -name core -exec rm -rf core {} \; -print
find . -name `hostname`.*.ecf.* -exec rm -rf `hostname`.*.ecf.* {} \; -print
find . -name callgrind.out.\* -exec rm -rf callgrind.out.\* {} \; -print
find . -name massif.out.\* -exec rm -rf massif.out.* {} \; -print
rm -rf rtt.dat

# remove any defs file at the workspace level. There should not be any
rm -rf *.def


# ======================================================================
# Create the tar file name based on the version
# Determine ecflow build directory name: see ACore/doc/extracting_version_number.ddoc
# ========================================================================
cd $WK
release=$(grep "Version::release_" ACore/src/Version.cpp  | cut -d= -s -f2 | sed 's/;//g' | sed 's/ //g')
major=$(grep   "Version::major_"   ACore/src/Version.cpp  | cut -d= -s -f2 | sed 's/;//g' | sed 's/ //g')
minor=$(grep   "Version::minor_"   ACore/src/Version.cpp  | cut -d= -s -f2 | sed 's/;//g' | sed 's/ //g' | sed 's/"//g' )
ECFLOW_WS_DIR=ecflow_${release}_${major}_${minor}

# ================================================================================
# Create ecflow tar file
# ================================================================================
cd $WK
cd ..

# remove old tar files
ECFLOWTAR=$ECFLOW_WS_DIR.tar
rm -rf $ECFLOWTAR.gz
rm -rf $ECFLOWTAR

# temporarily create a symbolic link ECFLOW_WS_DIR, so that tar file directory name has version number in it
ln -s MyProject $ECFLOW_WS_DIR

# Exclusions from tar file: 
# o/ .pydevproject used by Pydev eclipse plug-in
# o/ .metadata, .cproject, .project, .settings, csettings are eclipse dir
# o/ --exclude=SCRATCH  - has code for recording stand alone bugs.
# o/ bin directory contains the build object files and exe's
# o/ Debug is the eclipse/make dir
# o/ operations: this consists of the mega defs file used in operations for reference only /AParser/test/data/operations
# o/ Pyext/ecf/ecflow.so is ecFlow python extension
# o/ Exclude ddoc file's
# o/ Exclude *CERTAIN* Documentation directory , this includes the sphinx generated file '_build' ~4 Mb
#    ** STILL NEED Doc/online as this used in the python unit tests.
#    ** STILL NEED user-manual as this is installed
# o/ Exclude check point and back up check point files, --exclude=*.check --exclude=*.check.b
# o/ Exclude build/include
# o/ RemoteSystemsTempFile dir generated by eclipse
# o/ Xcdp, xcdp old ecflowview dirs
# o/ exclude ecbuild used for CMAKE. Still needs sorting out
# o/ exclude Pyext/test/data/CUSTOMER
# o/ gcc4.5 this directory is under build/, used for CMAKE
tar --exclude=*.check --exclude=*.check.b \
    --exclude=*.ddoc \
    --exclude=.p4config \
    --exclude=SCRATCH \
    --exclude=_build --exclude=func_spec --exclude=misc --exclude=newsletter --exclude=presentations --exclude=seminar --exclude=tac --exclude=Thumbs.db \
    --exclude=.pydevproject \
    --exclude=.metadata --exclude=.cproject --exclude=.project --exclude=.settings --exclude=.csettings \
    --exclude=ecbuild \
    --exclude=bin \
    --exclude=gcc4.5 \
    --exclude=Debug \
    --exclude=operations \
    --exclude=ecflow.so \
    --exclude=profile.sh \
    --exclude=CUSTOMER \
    --exclude=massive.sh \
    --exclude=val.sh \
    --exclude=include \
    --exclude=test_bench \
    --exclude=nightly \
    --exclude=RemoteSystemsTempFiles \
    --exclude=Xcdp \
    --exclude=xcdp \
    -cf $ECFLOWTAR $ECFLOW_WS_DIR/.

# Remove the link
rm $ECFLOW_WS_DIR  

ls -lh $ECFLOWTAR
gzip $ECFLOWTAR
