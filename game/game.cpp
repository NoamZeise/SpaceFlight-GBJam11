#include "game.h"
#include "render.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <graphics/glm_helper.h>
#include <iostream>
#include <resource_loader/stb_image.h>
#include <logger.h>
#include <stdio.h>

Game::Game(RenderFramework defaultFramework) {
    ManagerState state;
    state.conf.target_resolution[0] = GB_WIDTH;
    state.conf.target_resolution[1] = GB_HEIGHT;    
    state.conf.mip_mapping = false;
    state.conf.multisampling = false;
    state.conf.texture_filter_nearest = true;
    state.cursor = cursorState::hidden;
  
    manager = new Manager(defaultFramework, state);

    loadAssets();
    screenMat = glmhelper::calcMatFromRect(glm::vec4(0, 0, GB_WIDTH, GB_HEIGHT), 0,
					   // should be most far away thing
					   state.conf.depth_range_2D[0] + 0.01f);
    fpcam = camera::FirstPerson(glm::vec3(3.0f, 0.0f, 2.0f));
    finishedDrawSubmit = true;
    manager->render->setLightDirection(lightDir);
    manager->render->setPalette(menu.getPalette().toShaderPalette());
}

Game::~Game() {
  if (submitDraw.joinable())
    submitDraw.join();
  delete manager;
}

void Game::loadAssets() {
    pixel = manager->render->LoadTexture("textures/pixel.png");
    gameFont = manager->render->LoadFont("textures/dogicapixel.ttf");
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
  
  switch(state) {
  case GameState::Game:
      manager->render->DrawString(gameFont, "GAMEPLAY!",
				  glm::vec2(40.0f, 60.0f),
				  GB_TEXT_SIZE, 0.0f, COL1);
      break;
  case GameState::Menu:
      menu.Draw(manager->render);
      break;
  }

  //clear background
  manager->render->DrawQuad(pixel, screenMat, COL3);
  pFrameworkSwitch(manager->render,
		   submitDraw = std::thread(
			   &Render::EndDraw, manager->render, std::ref(finishedDrawSubmit)),
		   {
		       manager->render->EndDraw(finishedDrawSubmit);
		       finishedDrawSubmit = true;
		   }
  );
}
