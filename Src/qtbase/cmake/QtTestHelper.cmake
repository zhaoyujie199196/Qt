function(qt_add_test_helper targetName cppPath )
    SET(RUNNER_NAME ${targetName}_Runner)
    PROJECT(${targetName})
    ADD_EXECUTABLE(${targetName} ${cppPath})
    TARGET_LINK_LIBRARIES(${targetName} QtCore)
    TARGET_LINK_LIBRARIES(${targetName} QtTest)
    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${__qt6_root_src_path}/qtbase/include)
    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${__qt6_root_src_path}/qtbase/include/QtCore)
    TARGET_INCLUDE_DIRECTORIES(${targetName} PUBLIC ${__qt6_root_src_path}/qtbase/include/QtTest)
    ADD_TEST(NAME ${RUNNER_NAME} COMMAND ${targetName})
endfunction()