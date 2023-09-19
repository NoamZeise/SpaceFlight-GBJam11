#include "game.h"
#include "render.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <graphics/glm_helper.h>
#include <iostream>
#include <resource_loader/stb_image.h>
#include <logger.h>
#include <stdio.h>

const float SPECULAR_INTENSITY = 1.0f;
const float FAR_CLIP_3D = 100000000000000.0f;

Game::Game(RenderFramework defaultFramework) {
    ManagerState state;
    state.conf.target_resolution[0] = GB_WIDTH;
    state.conf.target_resolution[1] = GB_HEIGHT;    
    state.conf.mip_mapping = false;
    state.conf.multisampling = false;
    state.conf.texture_filter_nearest = true;
    state.conf.depth_range_3D[0] = 0.5f;
    state.conf.depth_range_3D[1] = FAR_CLIP_3D;
    state.cursor = cursorState::disabled;
    
    manager = new Manager(defaultFramework, state);
    
    loadAssets();
    screenMat = glmhelper::calcMatFromRect(glm::vec4(0, 0, GB_WIDTH, GB_HEIGHT), 0,
					   // should be most far away thing
					   state.conf.depth_range_2D[0] + 0.01f);
    fpcam = camera::FirstPerson(glm::vec3(3.0f, 0.0f, 2.0f));
    fpcam.setSpeed(0.001f);
    finishedDrawSubmit = true;
    manager->render->setLightDirection(lightDir);
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
    gameFont = manager->render->LoadFont("textures/dogicapixel.ttf");

    planet = manager->render->Load3DModel("models/planet.obj");
    planetMat = glm::translate(glm::scale(glm::mat4(1.0f),
					  glm::vec3(3)),
			       glm::vec3(5, 0, 0));
    planetNorm = glm::inverseTranspose(planetMat);
    
    menu = MainMenu(manager->render);
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
  }

  //debug controls

  if(manager->input.kb.press(GLFW_KEY_F1))
      fpcam.toggleFasterCam();
  if(manager->input.kb.press(GLFW_KEY_F2))
      glfwSetWindowShouldClose(manager->window, GLFW_TRUE);
  
  fpcam.update(manager->input, manager->timer);

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

}

void Game::postUpdate() {
    manager->render->set3DViewMatrixAndFov(fpcam.getViewMatrix(), fpcam.getZoom(),
					   glm::vec4(fpcam.getPos(), 0.0));
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
  manager->render->DrawQuad(pixel, screenMat, COL3);

  
  if(state == GameState::Game) {
      manager->render->Begin3DDraw();

      manager->render->DrawModel(planet, planetMat, planetNorm);
  
      manager->render->Begin2DDraw();
  }
  
  switch(state) {
  case GameState::Game:
      
      break;
  case GameState::Menu:
      menu.Draw(manager->render);
      break;
  }




  
  pFrameworkSwitch(manager->render,
		   submitDraw = std::thread(
			   &Render::EndDraw, manager->render, std::ref(finishedDrawSubmit)),
		   {
		       manager->render->EndDraw(finishedDrawSubmit);
		       finishedDrawSubmit = true;
		   }
  );
}
