set(srcs
    DirectoryHandler.cpp
    IconProvider.cpp
    LocalSocketServer.cpp
    LogConsumer.hpp
    LogData.cpp    
    LogModel.cpp
    LogTruncator.cpp
    LogView.cpp
    MessageLabel.cpp
    Palette.cpp
    TextFormat.cpp
    UIDebug.cpp
    UiLog.cpp
    UserMessage.cpp   
    )

if(ECFLOW_LOGVIEW)
    list(APPEND srcs LogLoadData.cpp)
endif()

if(ECFLOW_QT)
    include_directories(${ECFLOW_QT_INCLUDE_DIR})
#else()
#    include(${QT_USE_FILE})
#    set(ECFLOW_QT_LIBRARIES ${QT_LIBRARIES})
endif()


set(moc_files
    LocalSocketServer.hpp
    LogModel.hpp
    LogTruncator.hpp
    LogView.hpp
    MessageLabel.hpp
)

if(ECFLOW_LOGVIEW)
    list(APPEND moc_files LogLoadData.hpp)
endif()


#if(ECFLOW_LOGVIEW)
#    list(APPEND moc_files LogLoadWidget.hpp)
#endif()

#set(wrap_ui_files
#    LogLoadWidget.ui
#    )

#if(ECFLOW_LOGVIEW)
#    set(wrap_ui_files
#        LogLoadWidget.ui
#        )
#endif()

if(ECFLOW_QT6)
  QT6_WRAP_CPP(LIBVIEWER_MOC ${moc_files} )
  QT6_WRAP_UI       (LIBVIEWER_FORMS_HEADERS ${wrap_ui_files})
elseif(ECFLOW_QT5)
  QT5_WRAP_CPP(LIBVIEWER_MOC ${moc_files} )
  QT5_WRAP_UI       (LIBVIEWER_FORMS_HEADERS ${wrap_ui_files})
endif()

# add all the images as dependencies of the resource file so that it is
# automatically recompiled when an image changes
#file( GLOB image_files "${CMAKE_CURRENT_SOURCE_DIR}/../images/*.*" )
#ADD_CUSTOM_TARGET(Qt_resource_cpp DEPENDS ${VIEWER_RES})
#ADD_DEPENDENCIES(Qt_resource_cpp ${image_files})

add_definitions( -DECFLOW_SHARED_DIR="${CMAKE_INSTALL_PREFIX}/share/ecflow" )

ecbuild_add_library( TARGET viewer
                     NOINSTALL
                     TYPE     STATIC
                     SOURCES  ${srcs} ${LIBVIEWER_MOC} ${LIBVIEWER_RES} ${LIBVIEWER_FORMS_HEADERS}
                    )
target_include_directories( viewer PRIVATE  .
                                            ../../../ACore/src
                                            ${Boost_INCLUDE_DIRS}
                                            ${CMAKE_CURRENT_BINARY_DIR}
) 

                    
