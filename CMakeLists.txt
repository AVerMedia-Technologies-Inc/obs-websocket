project(obs-websocket-avt VERSION 1.0.1)
set(OBS_WEBSOCKET_RPC_VERSION 1)

option(ENABLE_WEBSOCKET "Enable building OBS with websocket plugin" ON)

if(NOT ENABLE_WEBSOCKET OR NOT ENABLE_UI)
  message(STATUS "OBS:  DISABLED   obs-websocket")
  return()
endif()

# Submodule deps check
if(NOT
   (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/deps/json/CMakeLists.txt
    AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/deps/websocketpp/CMakeLists.txt
    AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/deps/qr/cpp/QrCode.hpp
    AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/deps/asio/asio/include/asio.hpp))
  obs_status(FATAL_ERROR "obs-websocket submodule deps not available.")
endif()

# Plugin tests flag
option(PLUGIN_TESTS "Enable plugin runtime tests" OFF)

# Qt build stuff
set(CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON) # For resources.qrc

# Find Qt
find_qt(COMPONENTS Core Widgets Svg Network)

# Find nlohmann
set(JSON_BuildTests
    OFF
    CACHE INTERNAL "")
add_subdirectory(deps/json)

# Tell websocketpp not to use system boost
add_definitions(-DASIO_STANDALONE)

# Configure files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/plugin-macros.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin-macros.generated.h)

# Setup target
add_library(obs-websocket-avt MODULE)
add_library(OBS::websocket ALIAS obs-websocket-avt)

target_sources(
  obs-websocket-avt
  PRIVATE src/obs-websocket.cpp
	src/WSServer.cpp
	src/ConnectionProperties.cpp
	src/WSRequestHandler.cpp
	src/WSRequestHandler_General.cpp
	src/WSRequestHandler_Profiles.cpp
	src/WSRequestHandler_Recording.cpp
	src/WSRequestHandler_ReplayBuffer.cpp
	src/WSRequestHandler_SceneCollections.cpp
	src/WSRequestHandler_Scenes.cpp
	src/WSRequestHandler_SceneItems.cpp
	src/WSRequestHandler_Sources.cpp
	src/WSRequestHandler_Streaming.cpp
	src/WSRequestHandler_VirtualCam.cpp
	src/WSRequestHandler_StudioMode.cpp
	src/WSRequestHandler_Transitions.cpp
	src/WSRequestHandler_Outputs.cpp
	src/WSRequestHandler_MediaControl.cpp
	src/WSRequestHandler_AverMediaParseJson.cpp
	src/WSEvents.cpp
	src/Config.cpp
	src/Utils.cpp
	src/rpc/RpcRequest.cpp
	src/rpc/RpcResponse.cpp
	src/rpc/RpcEvent.cpp
	src/protocol/OBSRemoteProtocol.cpp
	src/forms/settings-dialog.cpp
    src/obs-websocket.h
	src/WSServer.h
	src/ConnectionProperties.h
	src/WSRequestHandler.h
	src/WSEvents.h
	src/Config.h
	src/Utils.h
	src/rpc/RpcRequest.h
	src/rpc/RpcResponse.h
	src/rpc/RpcEvent.h
	src/protocol/OBSRemoteProtocol.h
	src/forms/settings-dialog.h)

target_include_directories(
  obs-websocket-avt
  PRIVATE ${Qt5Core_INCLUDES} ${Qt5Widgets_INCLUDES} ${Qt5Svg_INCLUDES}
          ${Qt5Network_INCLUDES} "deps/asio/asio/include" "deps/websocketpp")

target_link_libraries(
  obs-websocket-avt
  PRIVATE OBS::libobs
          OBS::frontend-api
          Qt::Core
          Qt::Widgets
          Qt::Svg
          Qt::Network
          nlohmann_json::nlohmann_json)

target_compile_features(obs-websocket-avt PRIVATE cxx_std_17)

set_target_properties(obs-websocket-avt PROPERTIES FOLDER "plugins/obs-websocket-avt")

if(PLUGIN_TESTS)
  target_compile_definitions(obs-websocket-avt PRIVATE PLUGIN_TESTS)
endif()

# Random other things
if(WIN32)
  add_definitions(-D_WEBSOCKETPP_CPP11_STL_)
  target_compile_options(obs-websocket-avt PRIVATE /wd4267 /wd4996)
elseif(UNIX AND NOT APPLE)
  target_compile_options(
    obs-websocket-avt PRIVATE -Wall -Wextra -Wno-missing-field-initializers
                          -Wno-variadic-macros -Wno-error=format-overflow)
elseif(APPLE)
  target_compile_options(
    obs-websocket-avt PRIVATE -Wno-error=null-pointer-subtraction
                          -Wno-error=deprecated-declarations)
endif()

# Final CMake helpers
setup_plugin_target(obs-websocket-avt)
setup_target_resources(obs-websocket-avt "obs-plugins/obs-websocket-avt")
