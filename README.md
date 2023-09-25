# GB Jam 11 Submission

A weeklong game jam to create a game with the graphical restrictions
of a nintendo gameboy. This means the game must have a resolution of 
```160 x 144```
and use 4 colours.

A 3D space flight sim about exploring a solar system and collection log files. Built using my [graphics environment](https://github.com/NoamZeise/Graphics-Environment.git)


## Controls

```
--------------------------------
| GB   | Keyboard | Controller |
--------------------------------
|  A   |    Z     |     A/LT   |
|  B   |    X     |     B/RT   |
| START|  ESC     |    BACK    |
|SELECT|   ENTER  |   START    |
|DIR   |  ARROWS  |   DPAD/JOY |
--------------------------------
```

## Compile Instructions

### Requirements:

* C++ compiler - ie visual studio(MSVC), gcc, clang
* [CMake](https://cmake.org/)
* [Vulkan SDK](https://vulkan.lunarg.com/)


### Compiling

* Clone this repo with the option of `--recurse-submodules`, ie
```
git clone https://github.com/NoamZeise/gbjam11.git --recurse-submodules
```
* make a build folder and enter it
```
cd gbjam11 
mkdir build
cd build
```
* run cmake with these options
```
cmake .. -DCMAKE_BUILD_TYPE=Release -DNO_AUDIO=true
```

* build the executable
```
cmake --build .
```

* The executable should now be in `gbjam11/build/game/` and is called `GBJam11`.
