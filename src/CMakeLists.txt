cmake_minimum_required(VERSION 3.6)
project(doom2d C)

string(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" "Emscripten" D2D_FOR_EMSCRIPTEN)
string(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" "Darwin" D2D_FOR_DARWIN)
string(COMPARE EQUAL "${CMAKE_SYSTEM_PROCESSOR}" "powerpc" D2D_FOR_POWERPC)

option(SYSTEM_DRIVER "Build with selected system driver" "SDL")
option(RENDER_DRIVER "Build with selected render driver" "OpenGL")
option(SOUND_DRIVER "Build with selected sound driver" "OpenAL")

string(TOUPPER "${SYSTEM_DRIVER}" SYSTEM_DRIVER)
string(COMPARE EQUAL "${SYSTEM_DRIVER}" "SDL" WITH_SDL)
string(COMPARE EQUAL "${SYSTEM_DRIVER}" "SDL2" WITH_SDL2)
string(COMPARE EQUAL "${SYSTEM_DRIVER}" "KOS32" WITH_KOS32)
string(COMPARE EQUAL "${SYSTEM_DRIVER}" "STUB" WITH_STUBSYS)
string(TOUPPER "${RENDER_DRIVER}" RENDER_DRIVER)
string(COMPARE EQUAL "${RENDER_DRIVER}" "OPENGL" WITH_OPENGL)
string(COMPARE EQUAL "${RENDER_DRIVER}" "SOFTWARE" WITH_SOFTWARE)
string(COMPARE EQUAL "${RENDER_DRIVER}" "STUB" WITH_STUBRENDER)
string(TOUPPER "${SOUND_DRIVER}" SOUND_DRIVER)
string(COMPARE EQUAL "${SOUND_DRIVER}" "OPENAL" WITH_OPENAL)
string(COMPARE EQUAL "${SOUND_DRIVER}" "SDLMIXER" WITH_SDLMIXER)
string(COMPARE EQUAL "${SOUND_DRIVER}" "STUB" WITH_STUBSOUND)

set(D2D_GAME_ROOT .)
set(D2D_SDL_ROOT ${D2D_GAME_ROOT}/sdl)
set(D2D_SDL2_ROOT ${D2D_GAME_ROOT}/sdl2)
set(D2D_KOS32_ROOT ${D2D_GAME_ROOT}/kos32)
set(D2D_STUBSYS_ROOT ${D2D_GAME_ROOT}/stubsys)
set(D2D_OPENGL_ROOT ${D2D_GAME_ROOT}/gl)
set(D2D_SOFTWARE_ROOT ${D2D_GAME_ROOT}/soft)
set(D2D_STUBRENDER_ROOT ${D2D_GAME_ROOT}/stubren)
set(D2D_SDL_ROOT ${D2D_GAME_ROOT}/sdl)
set(D2D_SDLMIXER_ROOT ${D2D_GAME_ROOT}/sdlmixer)
set(D2D_OPENAL_ROOT ${D2D_GAME_ROOT}/openal)
set(D2D_STUBSOUND_ROOT ${D2D_GAME_ROOT}/stubsnd)
set(D2D_COMMON_ROOT ${D2D_GAME_ROOT}/common)

aux_source_directory(${D2D_GAME_ROOT} D2D_GAME_SRC)
aux_source_directory(${D2D_SDL_ROOT} D2D_SDL_SRC)
aux_source_directory(${D2D_SDL2_ROOT} D2D_SDL2_SRC)
aux_source_directory(${D2D_KOS32_ROOT} D2D_KOS32_SRC)
aux_source_directory(${D2D_STUBSYS_ROOT} D2D_STUBSYS_SRC)
aux_source_directory(${D2D_OPENGL_ROOT} D2D_OPENGL_SRC)
aux_source_directory(${D2D_SOFTWARE_ROOT} D2D_SOFTWARE_SRC)
aux_source_directory(${D2D_STUBRENDER_ROOT} D2D_STUBRENDER_SRC)
aux_source_directory(${D2D_SDL_ROOT} D2D_SDL_SRC)
aux_source_directory(${D2D_SDLMIXER_ROOT} D2D_SDLMIXER_SRC)
aux_source_directory(${D2D_OPENAL_ROOT} D2D_OPENAL_SRC)
aux_source_directory(${D2D_STUBSOUND_ROOT} D2D_STUBSOUND_SRC)
aux_source_directory(${D2D_COMMON_ROOT} D2D_COMMON_SRC)

if(WITH_SDL)
  if(D2D_FOR_EMSCRIPTEN)
    set(D2D_SYSTEM_SRC "${D2D_SDL_SRC}")
    set(D2D_SYSTEM_INCLUDE_DIR "")
    set(D2D_SYSTEM_LIBRARY "-lSDL")
    set(D2D_SYSTEM_LINKFLAGS "-s USE_SDL=1")
  else()
    find_package(SDL REQUIRED)
    set(D2D_SYSTEM_SRC "${D2D_SDL_SRC}")
    set(D2D_SYSTEM_INCLUDE_DIR "${SDL_INCLUDE_DIR}")
    set(D2D_SYSTEM_LIBRARY "${SDL_LIBRARY}")
  endif()
elseif(WITH_SDL2)
  if(D2D_FOR_EMSCRIPTEN)
    set(D2D_SYSTEM_SRC "${D2D_SDL2_SRC}")
    set(D2D_SYSTEM_INCLUDE_DIR "")
    set(D2D_SYSTEM_LIBRARY "-lSDL2")
    set(D2D_SYSTEM_LINKFLAGS "-s USE_SDL=2")
  else()
    if(D2D_FOR_DARWIN AND D2D_FOR_POWERPC)
      # tigerbrew did not provide sdl2 module for cmake
      find_package(SDL2)
      if(NOT SDL2_FOUND)
        set(SDL2_INCLUDE_DIRS "/usr/local/include/SDL2")
        set(SDL2_LIBRARIES "-lSDL2")
      endif(NOT SDL2_FOUND)
    else()
      find_package(SDL2 REQUIRED)
    endif()
    set(D2D_SYSTEM_SRC "${D2D_SDL2_SRC}")
    set(D2D_SYSTEM_INCLUDE_DIR "${SDL2_INCLUDE_DIRS}")
    set(D2D_SYSTEM_LIBRARY "${SDL2_LIBRARIES}")
  endif()
elseif(WITH_KOS32)
  set(D2D_SYSTEM_SRC "${D2D_KOS32_SRC}")
  set(D2D_SYSTEM_INCLUDE_DIR "")
  set(D2D_SYSTEM_LIBRARY "-lck")
  set(D2D_SYSTEM_LINKFLAGS "")
elseif(WITH_STUBSYS)
  set(D2D_SYSTEM_SRC "${D2D_STUBSYS_SRC}")
  set(D2D_SYSTEM_INCLUDE_DIR "")
  set(D2D_SYSTEM_LIBRARY "")
  set(D2D_SYSTEM_LINKFLAGS "")
else()
  message(FATAL_ERROR "Select SYSTEM_DRIVER as 'SDL' or 'SDL2' or 'KOS32' or 'STUB'")
endif()

if(WITH_STUBRENDER)
  set(D2D_RENDER_SRC "${D2D_STUBRENDER_SRC}")
  set(D2D_RENDER_INCLUDE_DIR "")
  set(D2D_RENDER_LIBRARY "")
elseif(WITH_OPENGL)
  if(D2D_FOR_EMSCRIPTEN)
    set(D2D_RENDER_SRC "${D2D_OPENGL_SRC}")
    set(D2D_RENDER_INCLUDE_DIR "")
    set(D2D_RENDER_LIBRARY "-lGL")
    set(D2D_RENDER_LINKFLAGS "-s LEGACY_GL_EMULATION=1")
  else()
    find_package(OpenGL REQUIRED)
    set(D2D_RENDER_SRC "${D2D_OPENGL_SRC}")
    set(D2D_RENDER_INCLUDE_DIR "${OPENGL_INCLUDE_DIR}")
    set(D2D_RENDER_LIBRARY "${OPENGL_LIBRARY}")
  endif()
elseif(WITH_SOFTWARE)
  set(D2D_RENDER_SRC "${D2D_SOFTWARE_SRC}")
  set(D2D_RENDER_INCLUDE_DIR "")
  set(D2D_RENDER_LIBRARY "")
else()
  message(FATAL_ERROR "Select RENDER_DRIVER as 'OPENGL' or 'SOFTWARE' or 'STUB'")
endif()

if(WITH_STUBSOUND)
  set(D2D_SOUND_SRC "${D2D_STUBSOUND_SRC}")
  set(D2D_SOUND_INCLUDE_DIR "")
  set(D2D_SOUND_LIBRARY "")
elseif(WITH_OPENAL)
  find_package(OpenAL REQUIRED)
  set(D2D_SOUND_SRC "${D2D_OPENAL_SRC}")
  set(D2D_SOUND_INCLUDE_DIR "${OPENAL_INCLUDE_DIR}")
  set(D2D_SOUND_LIBRARY "${OPENAL_LIBRARY}")
elseif(WITH_SDLMIXER)
  find_package(SDL_mixer REQUIRED)
  set(D2D_SOUND_SRC "${D2D_SDLMIXER_SRC}")
  set(D2D_SOUND_INCLUDE_DIR "${SDLMIXER_INCLUDE_DIR}")
  set(D2D_SOUND_LIBRARY "${SDLMIXER_LIBRARY}")
else()
  message(FATAL_ERROR "Select SOUND_DRIVER as 'OPENAL' or 'SDLMIXER' or 'STUB'")
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_C_FLAGS "")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O3") # -DNDEBUG
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

message(STATUS "=== BUILD OPTIONS ===")
message(STATUS "BUILD:  " "${CMAKE_BUILD_TYPE}")
message(STATUS "CFLAGS: " "${CMAKE_C_FLAGS}")
message(STATUS "SYSTEM: " "${SYSTEM_DRIVER}")
message(STATUS "RENDER: " "${RENDER_DRIVER}")
message(STATUS "SOUND:  " "${SOUND_DRIVER}")

set(D2D_USED_SRC ${D2D_GAME_SRC} ${D2D_SYSTEM_SRC} ${D2D_RENDER_SRC} ${D2D_SOUND_SRC} ${D2D_COMMON_SRC})
set(D2D_USED_INCLUDE_DIR "${D2D_GAME_ROOT}" "${D2D_SYSTEM_INCLUDE_DIR}" "${D2D_RENDER_INCLUDE_DIR}" "${D2D_SOUND_INCLUDE_DIR}" "${D2D_LIBCP866_ROOT}")
set(D2D_USED_LIBRARY "${D2D_SYSTEM_LIBRARY}" "${D2D_RENDER_LIBRARY}" "${D2D_SOUND_LIBRARY}")
#message(STATUS "USED SRC: ${D2D_USED_SRC}")
#message(STATUS "USED INC: ${D2D_USED_INCLUDE_DIR}")
#message(STATUS "USED LIB: ${D2D_USED_LIBRARY}")
add_executable(doom2d ${D2D_USED_SRC})
target_include_directories(doom2d PRIVATE ${D2D_USED_INCLUDE_DIR})
if(D2D_FOR_EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  configure_file(${D2D_GAME_ROOT}/doom2d.wad doom2d.wad COPYONLY)
  set(D2D_DOOM2D_WAD doom2d.wad)
  configure_file(${D2D_GAME_ROOT}/default.cfg default.cfg COPYONLY)
  set(D2D_DEFAULT_CFG default.cfg)
  set(D2D_USED_LINKFLAGS "${D2D_SYSTEM_LINKFLAGS} ${D2D_RENDER_LINKFLAGS} ${D2D_SOUND_LINKFLAGS}")
  set_target_properties(doom2d PROPERTIES LINK_FLAGS "-s WASM=0 -s MINIFY_HTML=0 -s DISABLE_EXCEPTION_CATCHING=1 -s SAFE_HEAP=1 -s ASSERTIONS=1 -s GL_ASSERTIONS=0 ${D2D_USED_LINKFLAGS} --preload-file ${D2D_DOOM2D_WAD} --preload-file ${D2D_DEFAULT_CFG}")
else()
  target_link_libraries(doom2d ${D2D_USED_LIBRARY})
endif()
