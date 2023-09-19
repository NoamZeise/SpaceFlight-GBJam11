#include "controls.h"

GbInput::GbInput() {
    for(int i = 0; i < (int)GB::Right + 1; i++)
	current[i] = prev[i] = false;
}

void GbInput::update(gamehelper::Input &input) {
    for(int i = 0; i < (int)GB::Right + 1; i++)
	prev[i] = current[i];

    current[(int)GB::A] = input.kb.hold(GLFW_KEY_Z)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_A);
    current[(int)GB::B] = input.kb.hold(GLFW_KEY_X)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_B);
    current[(int)GB::Start] = input.kb.hold(GLFW_KEY_ESCAPE)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_START);
    current[(int)GB::Select] = input.kb.hold(GLFW_KEY_ENTER)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_GUIDE);
    current[(int)GB::Up] = input.kb.hold(GLFW_KEY_UP)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_UP);
    current[(int)GB::Down] = input.kb.hold(GLFW_KEY_DOWN)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
    current[(int)GB::Left] = input.kb.hold(GLFW_KEY_LEFT)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
    current[(int)GB::Right] = input.kb.hold(GLFW_KEY_RIGHT)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);
}

bool GbInput::hold(GB b) {
    return current[(int)b];
}

bool GbInput::press(GB b) {
    return current[(int)b] && !prev[(int)b];
}
