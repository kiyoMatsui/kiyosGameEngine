## Project Properties
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(kiyosGameEngine)

SET(APPLICATION_NAME "kiyosGameEngine")
SET(APPLICATION_VERSION_STRING 0.7)

## Project Output Paths
SET(MAINFOLDER ${PROJECT_SOURCE_DIR})
SET(EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin")

## Project Search Paths
LIST(APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}")
SET(CMAKE_MODULE_PATH "${MAINFOLDER}/cmake")
INCLUDE_DIRECTORIES("${MAINFOLDER}/kge")

INCLUDE_DIRECTORIES("${MAINFOLDER}/thirdParty/catch") #catch.hpp

## example project Sources
FILE(GLOB_RECURSE EXAMPLE_SRCS ${MAINFOLDER}/examples/*.cpp )

## Project Dependencies

find_package(SFML 2 COMPONENTS audio graphics window system network)
# If found, include and link; otherwise output error message
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
else()
	message(WARNING "-> Make sure the SFML libraries with the same configuration (Release/Debug, Static/Dynamic) exist")
endif()



## Use compiler debugging flags per default
INCLUDE(CxxFlags)

#debug
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -std=c++1z")

#release
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -std=c++1z")


set(BUILD_TESTING true CACHE BOOL "Enable building of tests.")

## Creating Binaries for examples
if(SFML_FOUND)
SET(KGEPROJECT_LIBS ${CMAKE_DL_LIBS} ${SFML_LIBRARIES} )

SET(EXAMPLE1_NAME "sfmlExample")
SET(EXAMPLE1_BIN ${EXAMPLE1_NAME})
ADD_EXECUTABLE(${EXAMPLE1_BIN} ${MAINFOLDER}/examples/sfmlExample.cpp ${EXAMPLE_SRCS})

TARGET_LINK_LIBRARIES(${EXAMPLE1_BIN} ${KGEPROJECT_LIBS})
INSTALL(TARGETS ${EXAMPLE_BIN} DESTINATION bin)
else()
SET(KGEPROJECT_LIBS ${CMAKE_DL_LIBS} )
   message(WARNING "-> not building sfml example")
endif()

macro(create_test TARGET_NAME SOURCE DEPENDENCIES)
    add_executable(${TARGET_NAME} ${SOURCE})
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "testsDoesNotDoAnything/tests")
    target_link_libraries(
        ${TARGET_NAME}
        ${DEPENDENCIES}
        catchMain
        )
endmacro()

if (BUILD_TESTING)
    ADD_LIBRARY(catchMain ./tests/00CatchMain.cpp)
    enable_testing()
    create_test(baseEntityTest ./tests/baseEntityTest.cpp ${KGEPROJECT_LIBS})
    create_test(typeTriggerTest ./tests/typeTriggerTest.cpp ${KGEPROJECT_LIBS})
    create_test(pointTest ./tests/pointTest.cpp ${KGEPROJECT_LIBS})
    create_test(mainLoopTest ./tests/mainLoopTest.cpp ${KGEPROJECT_LIBS})
    create_test(sysem_entCompHandlerTest ./tests/sysem_entCompHandlerTest.cpp ${KGEPROJECT_LIBS})
endif (BUILD_TESTING)
