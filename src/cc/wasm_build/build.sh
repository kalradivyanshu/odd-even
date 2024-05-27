mkdir -p wasm_build/build
cd wasm_build/build
export CMAKE_TOOLCHAIN_FILE="$EMSCRIPTEN_PATH/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
echo "CMAKE_TOOLCHAIN_FILE: $CMAKE_TOOLCHAIN_FILE"
emcmake cmake -DBUILD_WASM=ON -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE ../ 
emmake make -j 6

cp ./wasm_core.js ../../compiled/
cp ./wasm_core.wasm ../../compiled/
