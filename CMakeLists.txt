## Project Properties
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(kiyosGameEngine)

SET(APPLICATION_NAME "kiyosGameEngine")
SET(APPLICATION_VERSION_STRING 1.0)

## Project Output Paths
SET(MAINFOLDER ${PROJECT_SOURCE_DIR})
SET(EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin")

## Project Search Paths
LIST(APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}")
SET(CMAKE_MODULE_PATH "${MAINFOLDER}/cmake")
INCLUDE_DIRECTORIES("${MAINFOLDER}/kge")

INCLUDE_DIRECTORIES("${MAINFOLDER}/thirdParty/catch") #catch.hpp

## example project Sources
FILE(GLOB_RECURSE EXAMPLE_SRCS ${MAINFOLDER}/examples/sdl2Example.cpp )

if(WIN32)
  include(cmake/Conan.cmake)
  run_conan()
  set(SDL2_FOUND TRUE)
else()
  find_package(SDL2 REQUIRED)
  if(SDL2_FOUND)
	include_directories(${SDL2_INCLUDE_DIR})
  else()
	message(WARNING "-> Make sure the SDL libraries are installed")
  endif()
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
## Use compiler debugging flags per default
INCLUDE(CxxFlags)
#debug
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -std=c++17 -pthread")

#thread sanitizer
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fsanitize=thread -g -O1")

#release
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -pthread")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -march=native -pthread")
endif()

set(BUILD_TESTING true CACHE BOOL "Enable building of tests.")

## Creating Binaries for examples
if(SDL2_FOUND)
SET(KGEPROJECT_LIBS ${CMAKE_DL_LIBS} ${SDL2_LIBRARIES})
if(WIN32)
    SET(KGEPROJECT_LIBS "${KGEPROJECT_LIBS}CONAN_PKG::sdl2" )
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



SET(EXAMPLE_NAME "sdl2Example")
SET(EXAMPLE_BIN ${EXAMPLE_NAME})
ADD_EXECUTABLE(${EXAMPLE_BIN} ${MAINFOLDER}/examples/sdl2Example.cpp ${EXAMPLE_SRCS})

TARGET_LINK_LIBRARIES(${EXAMPLE_BIN} ${KGEPROJECT_LIBS})


INSTALL(TARGETS ${EXAMPLE_BIN} DESTINATION bin)
else()
SET(KGEPROJECT_LIBS ${CMAKE_DL_LIBS} )
   message(WARNING "-> not building sdl2 examples")
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
    ADD_LIBRARY(catchMain ./test/00CatchMain.cpp)
    enable_testing()
    create_test(pointLineTest ./test/pointLineTest.cpp ${KGEPROJECT_LIBS})
    create_test(mainLoopTest ./test/mainLoopTest.cpp ${KGEPROJECT_LIBS})
    create_test(ECSTest ./test/ECSTest.cpp ${KGEPROJECT_LIBS})
    create_test(threadPoolTest ./test/threadPoolTest.cpp ${KGEPROJECT_LIBS})
endif (BUILD_TESTING)
