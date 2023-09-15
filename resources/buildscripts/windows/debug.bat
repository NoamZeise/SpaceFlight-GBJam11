SETLOCAL
del build\Debug\Graphics-Environment.exe
cd build
call vcvars64
cmake .. -G"Ninja Multi-Config" -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build . --config Debug
cd ..
Xcopy resources\audio build\Debug\audio /i /c /e /r /y
Xcopy resources\models build\Debug\models /i /c /e /r /y
Xcopy resources\shaders build\Debug\shaders /i /c /e /r /y
Xcopy resources\textures build\Debug\textures /i /c /e /r /y
Xcopy resources\windows-dlls build\Debug\ /i /c /e /r /y
cd build\Debug
Graphics-Environment
ENDLOCAL
