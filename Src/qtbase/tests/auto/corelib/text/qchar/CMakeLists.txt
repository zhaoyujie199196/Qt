message("text/qchar/cmakelists.txt begin")
PROJECT(TST_QCHAR)
ADD_EXECUTABLE(TST_QCHAR ${CMAKE_CURRENT_SOURCE_DIR}/tst_qchar.cpp)
TARGET_LINK_LIBRARIES(TST_QCHAR QtCore)
TARGET_LINK_LIBRARIES(TST_QCHAR QtTest)
TARGET_INCLUDE_DIRECTORIES(TST_QCHAR PUBLIC ${__qt6_root_src_path}/qtbase/include)
TARGET_INCLUDE_DIRECTORIES(TST_QCHAR PUBLIC ${__qt6_root_src_path}/qtbase/include/QtCore)
TARGET_INCLUDE_DIRECTORIES(TST_QCHAR PUBLIC ${__qt6_root_src_path}/qtbase/include/QtTest)
ADD_TEST(NAME tst_qchar_run1 COMMAND TST_QCHAR)

#没有返回值被当作错误处理
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Werror=return-type)
