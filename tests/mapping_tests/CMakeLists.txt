cmake_minimum_required(VERSION 3.10)

enable_testing()

find_package(Boost COMPONENTS unit_test_framework)
find_package(LAPACK)

project(mapping_tests)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/../..)	

# for Visual Studio
if(MSVC) 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

if(NOT UNIX)
    set(BOOST_LINK_DIRS "${BOOST_ROOT}/stage/lib")
endif()

# mapping tests
add_executable(mapping_tests mapping_tests.cpp)

target_include_directories(mapping_tests PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(mapping_tests PRIVATE ${Boost_LIBRARIES})
if(CMAKE_SYSTEM_NAME MATCHES Windows)
    target_link_directories(mapping_tests PRIVATE ${BOOST_LINK_DIRS})
endif()

if(UNIX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    set_target_properties(mapping_tests PROPERTIES CXX_STANDARD 17)
    target_compile_options(mapping_tests PRIVATE -DBOOST_TEST_DYN_LINK)
    target_link_libraries(mapping_tests PRIVATE ${LAPACK_LIBRARIES})
endif(UNIX)

add_test(NAME mapping_tests COMMAND mapping_tests)

# KOC tests

add_executable(koc_tests kohonen_outliers_clustering_tests.cpp)

target_include_directories(koc_tests PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(koc_tests PRIVATE ${Boost_LIBRARIES})
if(CMAKE_SYSTEM_NAME MATCHES Windows)
    target_link_directories(koc_tests PRIVATE ${BOOST_LINK_DIRS})
endif()


if(UNIX)
	set_target_properties(koc_tests PROPERTIES CXX_STANDARD 17)
	target_compile_options(koc_tests PRIVATE -pthread)
	set_target_properties(koc_tests PROPERTIES LINK_FLAGS -pthread)
endif(UNIX)

if(LAPACK_LIBRARIES)
	target_link_libraries(koc_tests PRIVATE ${LAPACK_LIBRARIES})	
endif(LAPACK_LIBRARIES)
 
if(CMAKE_SYSTEM_NAME MATCHES Windows)
	target_link_directories(koc_tests PRIVATE ${PROJECT_SOURCE_DIR}/../../mkl/lib/intel64_win)
	target_link_libraries(koc_tests PRIVATE mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
	target_link_libraries(koc_tests PRIVATE ${Boost_LIBRARIES})
endif()

add_test(NAME koc_tests COMMAND mapping_tests)

#

