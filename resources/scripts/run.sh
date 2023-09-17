# run from build folder
cmake --build .
rm -r game/textures/
cp -r ../resources/textures/ game/textures
rm -r game/shaders/
cp -r ../resources/shaders/ game/shaders
cd game/textures/palettes
./gen_palettes_list.sh
cd ../../shaders/vulkan/
./cmpShader.sh
cd ../../
./GBjam11
