# 最低CMake版本要求
cmake_minimum_required(VERSION 4.0)


if(WIN32)
        message(STATUS "Current Platform is Windows.")
        set(CMAKE_GNUtoMS_VCVARS "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\vcvarsall.bat" CACHE STRING "txfly")
        set(CMAKE_GNUtoMS ON)
else()
        message(STATUS "Current Platform is Linux.")
endif()

#set(CMAKE_GNUtoMS_VCVARS "D:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\vcvarsall.bat" CACHE STRING "txfly")

#set(CMAKE_GNUtoMS ON)
# 项目名称，编译好dll或exe的名称
project(MTPorgramResistor)

set(PRODUCT_NAME "MTPorgramResistor")
set(LegalCopyRight  "Copyright(C) 2023/12/20")
set(COMPANY_NAME "VTECHSEMI")
set(MY_PRODUCT_NUMBER 1)
set(MY_PRODUCT_VERSION 0)
set(MY_BUILD_NUMBER 2036)
set(PRODUCT_VERSION ${MY_PRODUCT_NUMBER}.${MY_PRODUCT_VERSION}.${MY_BUILD_NUMBER})

# 设置C/C++标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# system .h
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# current .h
file(GLOB_RECURSE INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
# current .cpp
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# 定义编译选项
option(DEVUTILS "Enable DEVUTILS feature" OFF)
option(DEBUG1 "Enable DEBUG1 feature" OFF)
# 根据选项设置宏
if (DEVUTILS)
    add_definitions(-DDEVUTILS)
endif()

if (DEBUG1)
    add_definitions(-DDEBUG1)
endif()
if(WIN32)

		ADD_DEFINITIONS( /Zi )
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/doc/version.rc @ONLY)
        #set visa path
        set (VISA_ROOT "$ENV{LIBRARYFILES}/Libraries_tester/visa")
        # visa .h
        include_directories(${VISA_ROOT}/include)
        set (VISA_LIBRARIES)
        # create .dll
        add_library(MTPorgramResistor SHARED ${INCLUDES} ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/doc/version.rc)

        if(CMAKE_CL_64)
                message(STATUS "Start win64 cmake...")
                list(APPEND VISA_LIBRARIES "${VISA_ROOT}/lib64/visa64.lib")
                target_link_libraries(MTPorgramResistor ${LINK_LIBS} ${VISA_LIBRARIES})
                INSTALL(TARGETS MTPorgramResistor DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR}/lib)
                # set target param
                SET_TARGET_PROPERTIES(MTPorgramResistor PROPERTIES LINKER_LANGUAGE C
                                ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin_64
                                LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin_64
                                RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin_64
                                OUTPUT_NAME "MTPorgramResistor"
                                PREFIX "")
        else()
                message(STATUS "Start win32 cmake...")
                list(APPEND VISA_LIBRARIES "${VISA_ROOT}/lib/visa32.lib")
                target_link_libraries(MTPorgramResistor ${LINK_LIBS} ${VISA_LIBRARIES})
                INSTALL(TARGETS MTPorgramResistor DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR}/lib)
                # set target param
                SET_TARGET_PROPERTIES(MTPorgramResistor  PROPERTIES LINKER_LANGUAGE C
                                ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
                                LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
                                RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
                                OUTPUT_NAME "MTPorgramResistor"
                                PREFIX "")
        endif()
if (DEVUTILS)
    set_target_properties(MTPorgramResistor PROPERTIES
        OUTPUT_NAME "MTPorgramResistorDevUtils" )
else()
    set_target_properties(MTPorgramResistor PROPERTIES
        OUTPUT_NAME "MTPorgramResistor")
endif()
else()
        message(STATUS "Start Linux cmake...")
        # system .h
        include_directories(/usr/include/ni-visa)
        # set link path
        link_directories(/usr/lib/x86_64-linux-gnu)
        #find_package(Qt5 COMPONENTS Core REQUIRED)
        # create .so
        add_library(MTPorgramResistor SHARED ${INCLUDES} ${SOURCES})
        target_link_libraries(MTPorgramResistor PRIVATE visa)
        #target_link_libraries(MTPorgramResistor PRIVATE Qt5::Core visa)
        target_compile_definitions(MTPorgramResistor PRIVATE MTPorgramResistor_LIBRARY)
        SET_TARGET_PROPERTIES(MTPorgramResistor PROPERTIES VERSION ${PRODUCT_VERSION} SOVERSION 1)
endif()

set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations ")
