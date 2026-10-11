cmake_minimum_required(VERSION 3.25)
project(Ashita4MenuManager LANGUAGES CXX)

# The Ashita v4 SDK folder (<Ashita>\plugins\sdk), from -DASHITA4_SDK_PATH or the environment variable of the same name
set(ASHITA4_SDK_PATH "$ENV{ASHITA4_SDK_PATH}" CACHE PATH "Ashita v4 SDK folder (<Ashita>/plugins/sdk)")
if(NOT EXISTS "${ASHITA4_SDK_PATH}/Ashita.h")
    message(FATAL_ERROR "ASHITA4_SDK_PATH (${ASHITA4_SDK_PATH}) has no Ashita.h. Point it at <Ashita>/plugins/sdk.")
endif()
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "Ashita v4 plugins are 32-bit: configure with an x86 toolchain.")
endif()

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(menumanager STATIC ${SOURCES})
add_library(menumanager::menumanager ALIAS menumanager)
target_include_directories(menumanager PUBLIC include)
target_include_directories(menumanager SYSTEM PUBLIC "${ASHITA4_SDK_PATH}" "${ASHITA4_SDK_PATH}/d3d8/includes" "${ASHITA4_SDK_PATH}/ffxi")
target_compile_features(menumanager PUBLIC cxx_std_23)
if(MSVC)
    target_compile_options(menumanager PRIVATE /W4 /utf-8)
endif()

include(cmake/CPM.cmake)

# Optional: hides the game's drawing of chosen windows. Its own target, so the base library has no hooks
# MinHook 1.3.4, BSD-2-Clause
CPMAddPackage(
    NAME minhook
    VERSION 1.3.4
    URL https://codeload.github.com/TsudaKageyu/minhook/tar.gz/refs/tags/v1.3.4
    URL_HASH SHA256=1aebeae4ca898330c507860acc2fca2eb335fe446a3a2b8444c3bf8b2660a14e
    EXCLUDE_FROM_ALL YES
    SYSTEM YES
)
if(MSVC)
    target_compile_options(minhook PRIVATE /w)
endif()
file(GLOB SUPPRESS_SOURCES CONFIGURE_DEPENDS src/suppress/*.cpp)
add_library(menumanager_suppress STATIC ${SUPPRESS_SOURCES})
add_library(menumanager::suppress ALIAS menumanager_suppress)
target_link_libraries(menumanager_suppress PUBLIC menumanager PRIVATE minhook)
if(MSVC)
    target_compile_options(menumanager_suppress PRIVATE /W4 /utf-8)
endif()

option(MENUMANAGER_TESTS "Build the unit tests" ${PROJECT_IS_TOP_LEVEL})
if(MENUMANAGER_TESTS)
    CPMAddPackage(
        NAME doctest
        VERSION 2.4.12
        URL https://codeload.github.com/doctest/doctest/tar.gz/refs/tags/v2.4.12
        URL_HASH SHA256=73381c7aa4dee704bd935609668cf41880ea7f19fa0504a200e13b74999c2d70
        DOWNLOAD_ONLY YES
    )
    file(GLOB TESTS CONFIGURE_DEPENDS tests/*.cpp)
    add_executable(menumanager_tests ${TESTS})
    target_include_directories(menumanager_tests SYSTEM PRIVATE "${doctest_SOURCE_DIR}")
    target_include_directories(menumanager_tests PRIVATE src)
    target_link_libraries(menumanager_tests PRIVATE menumanager menumanager_suppress)
    enable_testing()
    add_test(NAME menumanager_tests COMMAND menumanager_tests)
endif()
