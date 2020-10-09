
SET(EXECUTABLE_OUTPUT_PATH ../)

cmake_minimum_required(VERSION 3.13)

project(sa)

set(SOURCE_EXE cApp.cpp)

set(SOURCE_LIB cMain.cpp)

add_compile_options(-fpermissive -D _LINUX)

add_library(cMain STATIC ${SOURCE_LIB})

add_executable(sa ${SOURCE_EXE})

find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

target_link_libraries(sa cMain ${wxWidgets_LIBRARIES} ${CURL_LIBRARIES})

