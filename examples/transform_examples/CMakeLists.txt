cmake_minimum_required(VERSION 3.10)

project(transform_examples)

find_package(Boost COMPONENTS serialization iostreams program_options)
find_package(ZLIB)
find_package(Threads)
find_package(LAPACK)

include_directories(
    ${PROJECT_SOURCE_DIR}
    )

include_directories(${Boost_INCLUDE_DIRS}) 

file(GLOB EXAMPLE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp) 

file(COPY "assets" DESTINATION ".")

#Run through each source
foreach(exampleSrc ${EXAMPLE_SRCS})

	#Extract the filename without an extension (NAME_WE)
	get_filename_component(exampleName ${exampleSrc} NAME_WE)

	#Add compile target
	add_executable(${exampleName} ${exampleSrc}) 
 
	#link to Boost libraries AND your targets and dependencies
	target_link_libraries(${exampleName} ${Boost_LIBRARIES})
	target_link_libraries(${exampleName} ${ZLIB_LIBRARIES})
	target_link_libraries(${exampleName} Threads::Threads)
    if(LAPACK_LIBRARIES)
		target_link_libraries(${exampleName} ${LAPACK_LIBRARIES})
	endif(LAPACK_LIBRARIES)
	set_target_properties(${exampleName} PROPERTIES CXX_STANDARD 17)
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
	    target_link_libraries(${exampleName} stdc++fs)
	endif()
endforeach(exampleSrc)