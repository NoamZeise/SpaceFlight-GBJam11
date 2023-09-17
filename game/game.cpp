#include "game.h"
#include "render.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <graphics/glm_helper.h>
#include <iostream>
#include <resource_loader/stb_image.h>
#include <logger.h>
#include "palette.h"
#include <stdio.h>

Game::Game(RenderFramework defaultFramework) {
    ManagerState state;
    state.conf.target_resolution[0] = GB_WIDTH;
    state.conf.target_resolution[1] = GB_HEIGHT;
    hexToColour(colour0, state.conf.clear_colour);
    
    Palette pea = loadPalette("textures/palettes/pea.png");
    loadAllPalettes("textures/palettes/palettes.txt");
    
    state.conf.mip_mapping = false;
    state.conf.multisampling = false;
    state.conf.texture_filter_nearest = true;
    state.cursor = cursorState::hidden;
    state.fixedWindowRatio = false;
    manager = new Manager(defaultFramework, state);

    loadAssets();
    fpcam = camera::FirstPerson(glm::vec3(3.0f, 0.0f, 2.0f));
    finishedDrawSubmit = true;
    manager->render->setLightDirection(lightDir);
    manager->render->setPalette(pea.toShaderPalette());
}

Game::~Game() {
  if (submitDraw.joinable())
    submitDraw.join();
  delete manager;
}

void Game::loadAssets() {
    test = manager->render->LoadTexture("textures/test.png");
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

  controls();
  
  fpcam.update(manager->input, manager->timer);

  postUpdate();
#ifdef TIME_APP_DRAW_UPDATE
    monitored_update_stats = "update: " + std::to_string(
	  std::chrono::duration_cast<std::chrono::microseconds>(
		  std::chrono::high_resolution_clock::now() - start).count() / 1000.0) + " ms";
#endif
}

void Game::controls() {
    if (manager->input.kb.press(GLFW_KEY_F))
	manager->toggleFullscreen();
    if (manager->input.kb.press(GLFW_KEY_ESCAPE)) {
	glfwSetWindowShouldClose(manager->window, GLFW_TRUE);
    }

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
  
  manager->render->DrawQuad(test, glmhelper::calcMatFromRect(glm::vec4(0, 0, GB_WIDTH, GB_HEIGHT),
							     0.0f));

  pFrameworkSwitch(manager->render,
		   submitDraw = std::thread(
			   &Render::EndDraw, manager->render, std::ref(finishedDrawSubmit)),
		   {
		       manager->render->EndDraw(finishedDrawSubmit);
		       finishedDrawSubmit = true;
		   }
  );
}
