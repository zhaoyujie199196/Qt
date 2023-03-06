#添加.cpp文件
FILE(GLOB_RECURSE QtCore_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
LIST(REMOVE_ITEM QtCore_SRC "${CMAKE_CURRENT_SOURCE_DIR}/text/qchar.cpp")
LIST(REMOVE_ITEM QtCore_SRC "${CMAKE_CURRENT_SOURCE_DIR}/text/qunicodetables.cpp")
ADD_LIBRARY(QtCore SHARED ${QtCore_SRC})

#添加头文件搜索路径
TARGET_INCLUDE_DIRECTORIES(QtCore PUBLIC "${__qt6_qtbase_src_path}/include")

#没有返回值被当作错误处理
target_compile_options(QtCore PRIVATE -Werror=return-type)