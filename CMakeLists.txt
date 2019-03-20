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
FILE(GLOB_RECURSE EXAMPLE_SRCS01 ${MAINFOLDER}/examples/*01.cpp )
<<<<<<< HEAD
FILE(GLOB_RECURSE EXAMPLE_SRCS02 ${MAINFOLDER}/examples/*02.cpp )
=======
FILE(GLOB_RECURSE EXAMPLE_SRCS02 ${MAINFOLDER}/examples/*02a_1thread.cpp )
FILE(GLOB_RECURSE EXAMPLE_SRCS03 ${MAINFOLDER}/examples/*02b_multiThread.cpp )
FILE(GLOB_RECURSE EXAMPLE_SRCS04 ${MAINFOLDER}/examples/*02c_mTNoMutex.cpp )
>>>>>>> kiyodev
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
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -std=c++1z -pthread")

#thread sanitizer
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -fsanitize=thread -g -O1")

#release
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -std=c++1z -pthread")


set(BUILD_TESTING true CACHE BOOL "Enable building of tests.")

## Creating Binaries for examples
if(SFML_FOUND)
SET(KGEPROJECT_LIBS ${CMAKE_DL_LIBS} ${SFML_LIBRARIES} )

SET(EXAMPLE1_NAME "sfmlExample01")
SET(EXAMPLE1_BIN ${EXAMPLE1_NAME})
ADD_EXECUTABLE(${EXAMPLE1_BIN} ${MAINFOLDER}/examples/sfmlExample01.cpp ${EXAMPLE_SRCS01})

<<<<<<< HEAD
SET(EXAMPLE2_NAME "sfmlExample02")
SET(EXAMPLE2_BIN ${EXAMPLE2_NAME})
ADD_EXECUTABLE(${EXAMPLE2_BIN} ${MAINFOLDER}/examples/sfmlExample02.cpp ${EXAMPLE_SRCS02})

TARGET_LINK_LIBRARIES(${EXAMPLE1_BIN} ${KGEPROJECT_LIBS})
TARGET_LINK_LIBRARIES(${EXAMPLE2_BIN} ${KGEPROJECT_LIBS})
=======
SET(EXAMPLE2_NAME "sfmlExample02a")
SET(EXAMPLE2_BIN ${EXAMPLE2_NAME})
ADD_EXECUTABLE(${EXAMPLE2_BIN} ${MAINFOLDER}/examples/sfmlExample02a_1thread.cpp ${EXAMPLE_SRCS02})

SET(EXAMPLE3_NAME "sfmlExample02b")
SET(EXAMPLE3_BIN ${EXAMPLE3_NAME})
ADD_EXECUTABLE(${EXAMPLE3_BIN} ${MAINFOLDER}/examples/sfmlExample02b_mtMutex.cpp ${EXAMPLE_SRCS03})

SET(EXAMPLE4_NAME "sfmlExample02c")
SET(EXAMPLE4_BIN ${EXAMPLE4_NAME})
ADD_EXECUTABLE(${EXAMPLE4_BIN} ${MAINFOLDER}/examples/sfmlExample02c_mTNoMutex.cpp ${EXAMPLE_SRCS04})

TARGET_LINK_LIBRARIES(${EXAMPLE1_BIN} ${KGEPROJECT_LIBS})
TARGET_LINK_LIBRARIES(${EXAMPLE2_BIN} ${KGEPROJECT_LIBS})
TARGET_LINK_LIBRARIES(${EXAMPLE3_BIN} ${KGEPROJECT_LIBS})
TARGET_LINK_LIBRARIES(${EXAMPLE4_BIN} ${KGEPROJECT_LIBS})
>>>>>>> kiyodev
INSTALL(TARGETS ${EXAMPLE_BIN} DESTINATION bin)
else()
SET(KGEPROJECT_LIBS ${CMAKE_DL_LIBS} )
   message(WARNING "-> not building sfml examples")
endif()

macro(create_test TARGET_NAME SOURCE DEPENDENCIES)
    add_executable(${TARGET_NAME} ${SOURCE})
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
    create_test(pointLineTest ./tests/pointLineTest.cpp ${KGEPROJECT_LIBS})
    create_test(mainLoopTest ./tests/mainLoopTest.cpp ${KGEPROJECT_LIBS})
    create_test(sysem_entCompHandlerTest ./tests/sysem_entCompHandlerTest.cpp ${KGEPROJECT_LIBS})
    create_test(threadPoolTest ./tests/threadPoolTest.cpp ${KGEPROJECT_LIBS})
endif (BUILD_TESTING)
