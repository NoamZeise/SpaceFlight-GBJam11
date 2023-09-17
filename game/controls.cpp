#include "controls.h"

GbInput::GbInput() {
    for(int i = 0; i < GB::Right + 1; i++)
	current[i] = prev[i] = false;
}

void GbInput::update(gamehelper::Input &input) {
    for(int i = 0; i < GB::Right + 1; i++)
	prev[i] = current[i];

    current[GB::A] = input.kb.hold(GLFW_KEY_Z)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_A);
    current[GB::B] = input.kb.hold(GLFW_KEY_X)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_B);
    current[GB::Start] = input.kb.hold(GLFW_KEY_ESCAPE)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_START);
    current[GB::Select] = input.kb.hold(GLFW_KEY_ENTER)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_GUIDE);
    current[GB::Up] = input.kb.hold(GLFW_KEY_UP)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_UP);
    current[GB::Down] = input.kb.hold(GLFW_KEY_DOWN)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
    current[GB::Left] = input.kb.hold(GLFW_KEY_LEFT)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
    current[GB::Right] = input.kb.hold(GLFW_KEY_RIGHT)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);
}

bool GbInput::hold(GB b) {
    return current[b];
}

bool GbInput::press(GB b) {
    return current[b] && !prev[b];
}
