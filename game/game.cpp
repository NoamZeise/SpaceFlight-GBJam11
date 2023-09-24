#include "game.h"
#include "render.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <graphics/glm_helper.h>
#include <iostream>
#include <resource_loader/stb_image.h>
#include <logger.h>
#include <stdio.h>

#include "gb_consts.h"
#include "collision.h"

const float SPECULAR_INTENSITY = 10.0f;

const float CAM_START_X = 160.0f;

Game::Game(RenderFramework defaultFramework) {
    ManagerState state;
    state.conf.target_resolution[0] = GB_WIDTH;
    state.conf.target_resolution[1] = GB_HEIGHT;    
    state.conf.mip_mapping = false;
    state.conf.multisampling = false;
    state.conf.texture_filter_nearest = true;
    state.conf.depth_range_3D[0] = NEAR_CLIP_3D;
    state.conf.depth_range_3D[1] = FAR_CLIP_3D;
    state.conf.depth_range_2D[0] = -50.0f;
    state.conf.depth_range_2D[1] = 50.0f;
    state.cursor = cursorState::hidden;
    
    manager = new Manager(defaultFramework, state);
    
    loadAssets();
    screenMat = glmhelper::calcMatFromRect(glm::vec4(0, 0, GB_WIDTH, GB_HEIGHT), 0,
					   // should be furthest in the background
					   state.conf.depth_range_2D[0] + 0.01f);
    fpcam = camera::FirstPerson(glm::vec3(CAM_START_X, 0.0f, 0.0f));
    fpcam.setSpeed(0.001f);
    finishedDrawSubmit = true;
    manager->render->setPalette(menu.getPalette().toShaderPalette());
    Lighting3D light;
    light.specular.w = SPECULAR_INTENSITY;
    manager->render->setLighting3D(light);
}

Game::~Game() {
  if (submitDraw.joinable())
    submitDraw.join();
  delete manager;
}

void Game::loadAssets() {
    pixel = manager->render->LoadTexture("textures/pixel.png");
    menu = MainMenu(manager->render);
    system = System(manager->render);
    ship = Ship(manager->render, glm::vec3(CAM_START_X, 0.0f, 0.0f));
    stars = manager->render->Load3DModel("models/stars.obj");
    stars.useShading = false;
	
    manager->render->LoadResourcesToGPU();
    manager->render->UseLoadedResources();
}

void Game::run() {
  while (!glfwWindowShouldClose(manager->window)) {
    update();
    if (manager->winWidth != 0 && manager->winHeight != 0)
      draw();
  }
}

void Game::update() {
#ifdef TIME_APP_DRAW_UPDATE
  auto start = std::chrono::high_resolution_clock::now();
#endif
  manager->update();
  input.update(manager->input);
  
  switch(state) {
  case GameState::Game:
      gameUpdate();
      break;
  case GameState::Menu:
      menuUpdate();
      break;
  case GameState::Debug:
      fpcam.update(manager->input, manager->timer);
      if(manager->input.kb.hold(GLFW_KEY_LEFT_ALT))
	  fpcam.setSpeed(0.5f);
      else
	  fpcam.setSpeed(0.05f);
      break;
  }

  //debug controls

  if(manager->input.kb.press(GLFW_KEY_F1)) {
      if(state != GameState::Debug) {
	  state = GameState::Debug;
	  glfwSetInputMode(manager->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	  ship.setPos(fpcam.getPos());
      }
      else {
	  state = GameState::Game;
	  fpcam.setPos(ship.getPos());
      }
      fpcam.toggleFasterCam();
  }
  if(manager->input.kb.press(GLFW_KEY_F2))
      glfwSetWindowShouldClose(manager->window, GLFW_TRUE);

  postUpdate();
#ifdef TIME_APP_DRAW_UPDATE
    monitored_update_stats = "update: " + std::to_string(
	  std::chrono::duration_cast<std::chrono::microseconds>(
		  std::chrono::high_resolution_clock::now() - start).count() / 1000.0) + " ms";
#endif
}

void Game::menuUpdate() {
    if (input.press(GB::Start))
	state = GameState::Game;
    MenuState state = menu.Update(input);
    if(state.play)
	this->state = GameState::Game;
    if(state.fullscreen)
	manager->toggleFullscreen();
    if(state.quit)
	glfwSetWindowShouldClose(manager->window, GLFW_TRUE);
    if(state.paletteChanged)
	manager->render->setPalette(menu.getPalette().toShaderPalette());
}

void Game::gameUpdate() {
    if (input.press(GB::Start))
	state = GameState::Menu;
    system.Update(manager->timer);
    ship.Update(input, manager->timer);

    Collision c = system.planetCollisions(ship.getPos());
    ship.PlanetCollision(c, manager->timer);

    /*switch(s) {
	case CollisionState::None:
	    LOG("no collisions");
	    break;
    case CollisionState::Close:
	LOG("CLOSE");
	break;
    case CollisionState::Collide:
	LOG("collided");
	break;
	}*/
}

void Game::postUpdate() {
    camera::FirstPerson *cam = nullptr;
    switch(state) {
    case GameState::Debug:
	cam = &fpcam;
	break;
    case GameState::Game:
	cam = &ship;
	break;
    }
    if(cam != nullptr) {
	manager->render->set3DViewMatrixAndFov(cam->getViewMatrix(), cam->getZoom(),
					       glm::vec4(cam->getPos(), 0.0));
	starModel = glm::scale(glm::translate(glm::mat4(1.0f), cam->getPos()),
			       glm::vec3(10000));
    }
}

void Game::draw() {
#ifdef TIME_APP_DRAW_UPDATE
    auto start = std::chrono::high_resolution_clock::now();
#endif

#ifdef MULTI_UPDATE_ON_SLOW_DRAW
    if (!finishedDrawSubmit)
	return;
    finishedDrawSubmit = false;
#endif
    if (submitDraw.joinable())
	submitDraw.join();

    manager->render->Begin2DDraw();

    //clear background
    manager->render->DrawQuad(pixel, screenMat, COL0);
  
    switch(state) {
    case GameState::Game:
    case GameState::Debug:
	manager->render->Begin3DDraw();
	//3D
	manager->render->DrawModel(stars, starModel, glm::mat4(1.0f));
	system.Draw(manager->render);
	
	manager->render->Begin2DDraw();
	//2D foreground
	ship.Draw(manager->render);
	break;
    case GameState::Menu:
	menu.Draw(manager->render);
	break;
    }
  
    pFrameworkSwitch(
	    manager->render,
	    submitDraw = std::thread(
		    &Render::EndDraw, manager->render, std::ref(finishedDrawSubmit)),{
		manager->render->EndDraw(finishedDrawSubmit);
		finishedDrawSubmit = true;
	    });
}
