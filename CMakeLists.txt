cmake_minimum_required(VERSION 3.12)
project(DungeonSlaveEngine)

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

find_package(dxsdk-d3dx CONFIG REQUIRED)

add_executable(${PROJECT_NAME}  
    main.cpp
    log.cpp objparser.cpp
    object.cpp camera.cpp world.cpp
    light.cpp
    window.cpp directx.cpp    
)

set(RESOURCES_SOURCE_DIR ${CMAKE_SOURCE_DIR}/resources)
set(RESOURCES_DEST_DIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/resources)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${RESOURCES_SOURCE_DIR} ${RESOURCES_DEST_DIR})

add_custom_target(CopyResources ALL DEPENDS ${RESOURCES_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE include/)

target_link_libraries(${PROJECT_NAME} PRIVATE dxguid dxgi d3d10 Microsoft::D3DX10)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)