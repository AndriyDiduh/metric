find_package (Boost COMPONENTS unit_test_framework serialization REQUIRED)
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    )


AUX_SOURCE_DIRECTORY(. TEST_SRCS)

#Run through each source
foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)

        #Add compile target
        add_executable(${testName} ${testSrc})

        #link to Boost libraries AND your targets and dependencies
        target_link_libraries(${testName} ${Boost_LIBRARIES})
        target_link_libraries(${testName} ${LAPACK_LIBRARIES})
        #I like to move testing binaries into a testBin directory
        set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})
        #Finally add it to test execution - 
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName} 
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                 COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${testName} )
endforeach(testSrc)
