## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

cmake_minimum_required (VERSION 2.6)

include(FindGTest)


#FIND_PACKAGE(GTest REQUIRED)
#INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

#FIND_PACKAGE(GMock REQUIRED)
#INCLUDE_DIRECTORIES(${GMOCK_INCLUDE_DIRS})


# GCC 4.4.6 does not support some C++11 features used by GMock 
add_definitions(-DGTEST_LANG_CXX11=0)

include_directories(
  $ENV{GTEST_ROOT}/googletest/include
  $ENV{GTEST_ROOT}/googlemock/include
)
    
set(GTEST_LIBRARY $ENV{GTEST_ROOT}/googlemock/make/gmock_main.a)
  