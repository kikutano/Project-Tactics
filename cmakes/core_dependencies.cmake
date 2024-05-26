macro(core_dependencies)
    # Set up SDL2 dependency by configuring a wrapper library for simplified linking with other targets
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}"/thirdparty/prebuilt/sdl2/cmake")
    find_package(SDL2 CONFIG REQUIRED)
    # Create a wrapper library for SDL2
    add_library(sdl2_wrapper INTERFACE)
    # Link the wrapper library with SDL2 and SDL2main
    target_link_libraries(sdl2_wrapper INTERFACE SDL2::SDL2 SDL2::SDL2main)

    # Set up OpenGL
    find_package(OpenGL REQUIRED)

    # Set up Lua
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}"/thirdparty/prebuilt/lua/cmake")
    find_package(LUA CONFIG REQUIRED)
    # Create a wrapper library for lua
    add_library(lua_wrapper INTERFACE)
    # Link the wrapper library with lua51
    target_link_libraries(lua_wrapper INTERFACE lua::lua sol2)

    # Set up assimp
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}"/thirdparty/prebuilt/assimp/cmake")
    find_package(assimp CONFIG REQUIRED)
    # Create a wrapper library for assimp
    add_library(assimp_wrapper INTERFACE)
    target_link_libraries(assimp_wrapper INTERFACE assimp::assimp ${ASSIMP_LIBRARIES})
    target_include_directories(assimp_wrapper INTERFACE ${ASSIMP_INCLUDE_DIRS})

    include(FetchContent)

    FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
    FetchContent_MakeAvailable(json)
    add_library(json_wrapper INTERFACE)
    target_link_libraries(json_wrapper INTERFACE nlohmann_json::nlohmann_json)
endmacro()
