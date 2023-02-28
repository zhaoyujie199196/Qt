#添加.cpp文件
FILE(GLOB_RECURSE QtBase_TestLib_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
ADD_LIBRARY(QtTest SHARED ${QtBase_TestLib_SRC})

TARGET_LINK_LIBRARIES(QtTest QtCore)

#添加头文件搜索路径
TARGET_INCLUDE_DIRECTORIES(QtTest PUBLIC "${__qt6_qtbase_src_path}/include")
TARGET_INCLUDE_DIRECTORIES(QtTest PUBLIC "${__qt6_qtbase_src_path}/include/QtCore")