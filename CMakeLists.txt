CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
SET(PROJ_NAME "SmartTools")
PROJECT(${PROJ_NAME})

SET(CMAKE_CXX_STANDARD 17)

# Check third party library
IF (NOT POSSDK_SUPPORT)
    MESSAGE(FATAL_ERROR "You need set POSSDK_SUPPORT, because this item to use it.")
ENDIF ()
IF (NOT BOOST_SUPPORT)
    MESSAGE(FATAL_ERROR "You need set BOOST_SUPPORT, because this item to use it.")
ENDIF ()
IF (NOT NDK_SUPPORT)
    MESSAGE(FATAL_ERROR "You need set NDK_SUPPORT, because this item to use it.")
ENDIF ()

# POSSDK
INCLUDE_DIRECTORIES(${POSSDK_SUPPORT}/include)
SET(POSSDK_SUPPORT_LIB -L${POSSDK_SUPPORT}/lib -lpossdk -lbinder -lcutils -lutils)

# Boost support
INCLUDE_DIRECTORIES(${BOOST_SUPPORT}/include)
SET(BOOST_SUPPORT_LIB -L${BOOST_SUPPORT}/lib -lboost_system)

# NDK support
INCLUDE_DIRECTORIES(${NDK_SUPPORT}/include)

# Utils module
ADD_SUBDIRECTORY(smart_test_resouce)
ADD_LIBRARY(${PROJ_NAME} SHARED library.cpp)
TARGET_LINK_LIBRARIES(${PROJ_NAME} ${POSSDK_SUPPORT_LIB} ${BOOST_SUPPORT_LIB})
