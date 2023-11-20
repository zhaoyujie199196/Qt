function(qt_add_test_helper targetName )
    SET(RUNNER_NAME ${targetName}_Runner)
    PROJECT(${targetName})
    file(GLOB PROJECT_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

    ADD_EXECUTABLE(${targetName} ${PROJECT_SOURCE})

    TARGET_LINK_LIBRARIES(${targetName} QtCore)
    TARGET_LINK_LIBRARIES(${targetName} QtTest)
    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${__qt6_root_src_path}/qtbase/include)
    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${__qt6_root_src_path}/qtbase/include/QtCore)
    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${__qt6_root_src_path}/qtbase/include/QtTest)
    ADD_TEST(NAME ${RUNNER_NAME} COMMAND ${targetName})
endfunction()