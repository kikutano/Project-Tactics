# Ensure the minimum required version of CMake is available, otherwise, throw an error
cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

set(SOURCES
    OpenTacticaPrototypeApplication.cpp
    main.cpp
    Component/CharacterComponent.cpp
    DataSet/CharacterDataSet.cpp
    Overlay/PrototypeOverlay.cpp
    State/GameState.cpp
    State/InitializationState.cpp
    State/UninitializationState.cpp)
    
set(HEADERS
    Component/CharacterComponent.h
    DataSet/CharacterDataSet.h
    Overlay/PrototypeOverlay.h
    State/GameState.h
    State/InitializationState.h
    State/UninitializationState.h)

set(TARGET_NAME OpenTacticaPrototype)
set(POST_BUILD_TARGET_NAME ${TARGET_NAME}PostBuild)
set(DATA_TARGET_NAME opentacticaprototype_data)
add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET_NAME} PUBLIC tactics_engine)
add_to_apps_folder(${TARGET_NAME})

# Set the working directory for visual studio
set_target_properties(
    ${TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${TARGET_NAME}>
)

# Copy all the dependencies ( dll ) to the executable file directory
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${TARGET_NAME}> $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>
  COMMAND_EXPAND_LISTS
)
# Copy all the dependencies ( dll ) and the data folder to the executable file directory
add_custom_target(${POST_BUILD_TARGET_NAME} ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources/${TARGET_NAME}/data $<TARGET_FILE_DIR:${TARGET_NAME}>/data
  COMMAND_EXPAND_LISTS
)

# Make the post-build command depend on data target
add_dependencies(${POST_BUILD_TARGET_NAME} ${DATA_TARGET_NAME})

# Make the main target depend on the post-build command target
add_dependencies(${TARGET_NAME} ${POST_BUILD_TARGET_NAME})

set_property(TARGET ${POST_BUILD_TARGET_NAME} PROPERTY FOLDER "_cmakeAndPostBuild")

unset(SOURCES)
unset(HEADERS)
unset(TARGET_NAME)