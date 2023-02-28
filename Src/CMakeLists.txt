#cmake项目需要的版本号
CMAKE_MINIMUM_REQUIRED(VERSION 3.16...3.20)
#c++版本号
SET(CMAKE_CXX_STANDARD 20)

#Qt版本号
SET(__qt6_root_src_path "${CMAKE_CURRENT_SOURCE_DIR}")
SET(__qt6_qtbase_src_path "${CMAKE_CURRENT_SOURCE_DIR}/qtbase")
INCLUDE("${__qt6_qtbase_src_path}/.cmake.conf")

#运行testing
SET(QT_BUILD_TESTS ON)
ENABLE_TESTING()

#Qt项目，指示版本号，使用语言为C++,C,ASM(汇编 Assembly Language)
PROJECT(Qt
        VERSION "${QT_REPO_MODULE_VERSION}"
        LANGUAGES CXX C ASM
        )

#添加main.cpp用作测试
ADD_EXECUTABLE(Qt ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
#添加头文件搜索路径
TARGET_INCLUDE_DIRECTORIES(Qt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/qtbase/include")
TARGET_INCLUDE_DIRECTORIES(Qt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/qtbase/include/QtCore")

#添加动态库
TARGET_LINK_LIBRARIES(Qt QtCore)

#添加子目录,会查找子目录下的CMakeLists.txt
ADD_SUBDIRECTORY(qtbase)