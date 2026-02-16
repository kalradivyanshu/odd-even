cmake_minimum_required(VERSION 3.15)
project(wasm_core)

set(CMAKE_CXX_STANDARD 20)

set(EMSCRIPTEN_FEATURES "-msimd128 -fexceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FEATURES}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMSCRIPTEN_FEATURES}")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${EMSCRIPTEN_FEATURES}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")
include_directories("${EMSCRIPTEN_PATH}/upstream/emscripten/system/include/")

add_executable(${PROJECT_NAME} 
  ../wasm.cc
)

# Add this line
target_compile_options(${PROJECT_NAME} PRIVATE "-sUSE_ZLIB=1")

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--bind -sWASM=1 -sINITIAL_MEMORY=144179200 -sEXPORT_ES6=1 -O3 -msimd128 -fexceptions -sEXPORTED_RUNTIME_METHODS=['HEAPU8'] -sUSE_ZLIB=1")

target_link_libraries(${PROJECT_NAME})