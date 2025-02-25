# Note:
# If new src or test cpp files are added make sure you touch this file
#
file( GLOB srcs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp" )

ecbuild_add_library( TARGET   nodeattr
                     NOINSTALL
                     TYPE     STATIC
                     SOURCES  ${srcs}
                    )
                    
target_link_libraries(nodeattr PRIVATE core)
target_include_directories(nodeattr PUBLIC src 
                                           ../ACore/src 
                                           ${Boost_INCLUDE_DIRS})


file( GLOB test_srcs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "test/*.cpp" )

# libboost_unit_test_framework  undefined reference to `clock_gettime', ${LIBRT} needed for boost 1.71
ecbuild_add_test( TARGET       u_anattr
                  SOURCES      ${test_srcs}
                  INCLUDES     src ${Boost_INCLUDE_DIRS}
                  LIBS         nodeattr ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_TEST_EXEC_MONITOR_LIBRARY} ${LIBRT}
                  DEFINITIONS  ${BOOST_TEST_DYN_LINK}
                  TEST_DEPENDS u_acore
                 )
