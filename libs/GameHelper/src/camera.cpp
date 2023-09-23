#include <GameHelper/camera.h>

#include <GLFW/glfw3.h>

namespace camera {
  FirstPerson::FirstPerson(glm::vec3 position) {
      _position = position;
      calculateVectors();
  }

  glm::mat4 FirstPerson::getViewMatrix() {
      updateView();
      return view;
  }

  float FirstPerson::getZoom() {
      return _zoom;
  }

  void FirstPerson::toggleFasterCam() {
      useFaster = !useFaster;
      if(useFaster)
	  _speed = _fasterSpeed;
      else
	  _speed = _slowerSpeed;
	  
  }

  void FirstPerson::setPos(glm::vec3 pos) {
      _position = pos;
  }
  
  void FirstPerson::update(gamehelper::Input &input, gamehelper::Timer &timer) {
      viewUpdated = true;
      //keyboard
      float velocity = _speed * timer.FrameElapsed();
      if(input.kb.hold(GLFW_KEY_W))
	  _position += _front * velocity;
      if(input.kb.hold(GLFW_KEY_A))
	  _position -= _left * velocity;
      if(input.kb.hold(GLFW_KEY_S))
	  _position -= _front * velocity;
      if(input.kb.hold(GLFW_KEY_D))
	  _position += _left * velocity;
      
      if(input.kb.hold(GLFW_KEY_SPACE))
	  _position += _worldUp * velocity;
      if(input.kb.hold(GLFW_KEY_LEFT_SHIFT))
	  _position -= _worldUp * velocity;

      //mouse
      _pitch   -= input.m.dy() * _sensitivity;
      _yaw 	 -= input.m.dx() * _sensitivity;

      if(input.c.connected(0)) {
	  glm::vec2 controller(input.c.axis(0, GLFW_GAMEPAD_AXIS_LEFT_X),
			       input.c.axis(0, GLFW_GAMEPAD_AXIS_LEFT_Y));
	  
	  controller.x = abs(controller.x) > 0.15 ? controller.x : 0;
	  controller.y = abs(controller.y) > 0.15 ? controller.y : 0;
	  _position += _front * velocity * -controller.y;
	  _position += _left * velocity * controller.x;
	  
	  controller = glm::vec2(input.c.axis(0, GLFW_GAMEPAD_AXIS_RIGHT_X),
				 input.c.axis(0, GLFW_GAMEPAD_AXIS_RIGHT_Y));
	  
	  controller.x = abs(controller.x) > 0.15 ? controller.x : 0;
	  controller.y = abs(controller.y) > 0.15 ? controller.y : 0;

	  if(input.c.hold(0, GLFW_GAMEPAD_BUTTON_A))
	      _position += _worldUp * velocity;
	  if(input.c.hold(0, GLFW_GAMEPAD_BUTTON_B))
	      _position -= _worldUp * velocity;
	  
	  _pitch -= controller.y;
	  _yaw -= controller.x;

      }

      if(_pitch > 89.0)
	  _pitch = 89.0;
      if(_pitch < -89.0)
	  _pitch = -89.0;

      //scroll
      _zoom -= input.m.scroll() * timer.FrameElapsed();
      if(_zoom < 1.0)
	  _zoom = 1.0;
      if(_zoom > 100.0)
	  _zoom = 100.0;

      calculateVectors();
      //std::cout << "X:" << _position.x << " Y:" << _position.y << " Z:" << _position.z << std::endl;
      updateView();
  }

  void FirstPerson::calculateVectors() {
      _front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
      _front.y = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
      _front.z = sin(glm::radians(_pitch));
      _front = glm::normalize(_front);

      _left = glm::normalize(glm::cross(_front, _worldUp));
      _up = glm::normalize(glm::cross(_left, _front));
  }

} //namespace end
