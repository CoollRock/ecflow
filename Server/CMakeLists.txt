 # =======================================================
 # LIB
 # to list all sources to build use:
 #    cd $WK/Server
 #    find src -name \*.cpp  -print | sort
 # =======================================================
list( APPEND srcs
   src/BaseServer.cpp
   src/CheckPtSaver.cpp
   src/NodeTreeTraverser.cpp
   src/Server.cpp
   src/ServerEnvironment.cpp
   src/ServerOptions.cpp
   src/TcpBaseServer.cpp
   src/TcpServer.cpp
)

if (OPENSSL_FOUND)
	list( APPEND srcs 
          src/SslServer.cpp
          src/SslTcpServer.cpp
        )
endif()


ecbuild_add_library( TARGET libserver
                     NOINSTALL
                     TYPE     STATIC
                     SOURCES  ${srcs}
                     )
target_link_libraries(libserver base node nodeattr core pthread )
target_include_directories(libserver PRIVATE src
                                             ../ACore/src 
                                             ../ANattr/src
                                             ../ANode/src
                                             ../Base/src
                                             ../Base/src/cts
                                             ../Base/src/stc 
)

# =======================================================
# EXE ecflow_server, if OpenSSL not enabled ${OPENSSL_LIBRARIES}, is empty

ecbuild_add_executable( TARGET   ecflow_server
                        SOURCES  src/ServerMain.cpp
                        LIBS     libserver ${OPENSSL_LIBRARIES}
                        INCLUDES ${Boost_INCLUDE_DIRS} 
                       )

# Override default behaviour that add RPATHS during install
# The only thing that seem to work is set INSTALL_RPATH to ""
# Using SKIP_BUILD_RPATH,BUILD_WITH_INSTALL_RPATH,INSTALL_RPATH_USE_LINK_PATH
# had no effect
#          
SET_TARGET_PROPERTIES(ecflow_server PROPERTIES 
                                    INSTALL_RPATH ""
                     )
                         
# use, i.e. don't skip the full RPATH for the build tree
#SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
#SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

# the RPATH to be used when installing
#SET(CMAKE_INSTALL_RPATH "")

# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

                          
# ================================================================================                         
# TEST
# libboost_unit_test_framework  undefined reference to `clock_gettime', ${LIBRT} needed for boost 1.71
ecbuild_add_test( TARGET       u_server
                  SOURCES      test/TestServerEnvironment.cpp 
                               test/TestServer1.cpp
                  INCLUDES     src ${Boost_INCLUDE_DIRS}
                  LIBS         libserver ${OPENSSL_LIBRARIES}
                               ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_TEST_EXEC_MONITOR_LIBRARY} ${LIBRT}
                  DEFINITIONS  ${BOOST_TEST_DYN_LINK}
                  TEST_DEPENDS u_base
              )

# ===================================================================
# install
# ===================================================================
install (TARGETS ecflow_server DESTINATION bin)
