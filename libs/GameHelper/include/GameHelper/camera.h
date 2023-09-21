#ifndef FREE_CAMERA_H
#define FREE_CAMERA_H

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>

#include "input.h"
#include "timer.h"

namespace camera {
  class FirstPerson {

  public:
      FirstPerson() { _position = glm::vec3(0.0f, 0.0f, 0.0f); };
      FirstPerson(glm::vec3 position);
      glm::mat4 getViewMatrix();
      float getZoom();
      glm::vec3 getPos() { return _position; }
      virtual void update(gamehelper::Input &input, gamehelper::Timer &timer);
      void setSpeed(float speed) { this->_slowerSpeed = speed;
	  useFaster = true;
	  this->toggleFasterCam();
      }
      void toggleFasterCam();
      void setPos(glm::vec3 pos);

  protected:
      glm::vec3 _position;
      glm::vec3 _front;
      glm::vec3 _up;
      glm::vec3 _left;
      glm::vec3 _worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
      glm::mat4 view = glm::mat4(1.0f);
      bool viewUpdated = true;

      double _yaw = 200.0;
      double _pitch = -20.0;
      double _roll = 0;

      float _speed = 0.01f;
      float _slowerSpeed = 0.01f;
      float _fasterSpeed = 0.05f;
      bool useFaster = false;
      double _sensitivity = 0.05;
      double _zoom = 45.0;


      void calculateVectors();
  };


}	//namesapce end



#endif
