# Install script for directory: /home/xivien/velma_sim/src/omnivelma/src/omnivelma_msgs

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/xivien/velma_sim/src/omnivelma/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/omnivelma_msgs/msg" TYPE FILE FILES
    "/home/xivien/velma_sim/src/omnivelma/src/omnivelma_msgs/msg/Vels.msg"
    "/home/xivien/velma_sim/src/omnivelma/src/omnivelma_msgs/msg/Encoders.msg"
    "/home/xivien/velma_sim/src/omnivelma/src/omnivelma_msgs/msg/EncodersStamped.msg"
    "/home/xivien/velma_sim/src/omnivelma/src/omnivelma_msgs/msg/Relative.msg"
    "/home/xivien/velma_sim/src/omnivelma/src/omnivelma_msgs/msg/RelativeStamped.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/omnivelma_msgs/srv" TYPE FILE FILES
    "/home/xivien/velma_sim/src/omnivelma/src/omnivelma_msgs/srv/SetFriction.srv"
    "/home/xivien/velma_sim/src/omnivelma/src/omnivelma_msgs/srv/SetInertia.srv"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/omnivelma_msgs/cmake" TYPE FILE FILES "/home/xivien/velma_sim/src/omnivelma/build/omnivelma_msgs/catkin_generated/installspace/omnivelma_msgs-msg-paths.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/xivien/velma_sim/src/omnivelma/devel/include/omnivelma_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/roseus/ros" TYPE DIRECTORY FILES "/home/xivien/velma_sim/src/omnivelma/devel/share/roseus/ros/omnivelma_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/common-lisp/ros" TYPE DIRECTORY FILES "/home/xivien/velma_sim/src/omnivelma/devel/share/common-lisp/ros/omnivelma_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gennodejs/ros" TYPE DIRECTORY FILES "/home/xivien/velma_sim/src/omnivelma/devel/share/gennodejs/ros/omnivelma_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND "/usr/bin/python2" -m compileall "/home/xivien/velma_sim/src/omnivelma/devel/lib/python2.7/dist-packages/omnivelma_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages" TYPE DIRECTORY FILES "/home/xivien/velma_sim/src/omnivelma/devel/lib/python2.7/dist-packages/omnivelma_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/xivien/velma_sim/src/omnivelma/build/omnivelma_msgs/catkin_generated/installspace/omnivelma_msgs.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/omnivelma_msgs/cmake" TYPE FILE FILES "/home/xivien/velma_sim/src/omnivelma/build/omnivelma_msgs/catkin_generated/installspace/omnivelma_msgs-msg-extras.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/omnivelma_msgs/cmake" TYPE FILE FILES
    "/home/xivien/velma_sim/src/omnivelma/build/omnivelma_msgs/catkin_generated/installspace/omnivelma_msgsConfig.cmake"
    "/home/xivien/velma_sim/src/omnivelma/build/omnivelma_msgs/catkin_generated/installspace/omnivelma_msgsConfig-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/omnivelma_msgs" TYPE FILE FILES "/home/xivien/velma_sim/src/omnivelma/src/omnivelma_msgs/package.xml")
endif()

