#include "controls.h"

GbInput::GbInput() {
    for(int i = 0; i < (int)GB::Right + 1; i++)
	current[i] = prev[i] = false;
}

#include <iostream>

void GbInput::update(gamehelper::Input &input) {
    for(int i = 0; i < (int)GB::Right + 1; i++)
	prev[i] = current[i];

    const float DEADZONE = 0.7f;
    
    current[(int)GB::A] = input.kb.hold(GLFW_KEY_Z)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_A)
	|| input.c.axis(0, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) > DEADZONE;
    
    current[(int)GB::B] = input.kb.hold(GLFW_KEY_X)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_B)
	|| input.c.axis(0, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER) > DEADZONE;
    
    current[(int)GB::Start] = input.kb.hold(GLFW_KEY_ESCAPE)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_BACK);
    
    current[(int)GB::Select] = input.kb.hold(GLFW_KEY_ENTER)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_START);
    
    current[(int)GB::Up] = input.kb.hold(GLFW_KEY_UP)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_UP)
	|| input.c.axis(0, GLFW_GAMEPAD_AXIS_LEFT_Y) < -DEADZONE;
    
    current[(int)GB::Down] = input.kb.hold(GLFW_KEY_DOWN)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_DOWN)
    	|| input.c.axis(0, GLFW_GAMEPAD_AXIS_LEFT_Y) > DEADZONE;
    
    current[(int)GB::Left] = input.kb.hold(GLFW_KEY_LEFT)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_LEFT)
	|| input.c.axis(0, GLFW_GAMEPAD_AXIS_LEFT_X) < -DEADZONE;
    
    current[(int)GB::Right] = input.kb.hold(GLFW_KEY_RIGHT)
	|| input.c.hold(0, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT)
	|| input.c.axis(0, GLFW_GAMEPAD_AXIS_LEFT_X) > DEADZONE;
}

bool GbInput::hold(GB b) {
    return current[(int)b];
}

bool GbInput::press(GB b) {
    return current[(int)b] && !prev[(int)b];
}
