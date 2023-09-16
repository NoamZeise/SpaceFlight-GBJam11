# run from build folder
cmake --build .
rm -r game/textures/
cp -r ../resources/textures/ game/textures
cd game/textures/palettes
./gen_palettes_list.sh
cd ../../
./GBjam11
