message("Parse Begin : qtbase/CMakeLists.txt")

PROJECT(QtBase)
add_subdirectory(src)
#ADD_LIBRARY(QtBase SHARED ${QT_BASE_SOURCE_FILES})

if (QT_BUILD_TESTS)
    add_subdirectory(tests)
endif()

message("Parse Finish : qtbase/CMakeLists.txt")