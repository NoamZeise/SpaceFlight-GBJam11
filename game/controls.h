#ifndef GB_CONTROLS_H
#define GB_CONTROLS_H

#include <GameHelper/input.h>

enum class GB {
    A,
    B,
    Select,
    Start,
    Up,
    Down,
    Left,
    Right,
};

class GbInput {
 public:
    GbInput();
    bool press(GB b);
    bool hold(GB b);
    void update(gamehelper::Input &input);
 private:
    bool current[(int)GB::Right + 1];
    bool prev[(int)GB::Right + 1];
};

#endif
