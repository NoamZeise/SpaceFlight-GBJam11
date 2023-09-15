#include "game.h"
#include "render.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <graphics/glm_helper.h>
#include <iostream>

Game::Game(RenderFramework defaultFramework) {
    ManagerState state;
    manager = new Manager(defaultFramework, state);

    loadAssets();

    fpcam = camera::FirstPerson(glm::vec3(3.0f, 0.0f, 2.0f));
    finishedDrawSubmit = true;
    manager->render->setLightDirection(lightDir);
    manager->audio.Play("audio/test.wav", false, 1.0f);
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
  
  manager->render->DrawQuad(test, glmhelper::calcMatFromRect(glm::vec4(10, 10, 100, 100), 0.0f));

  pFrameworkSwitch(manager->render,
		   submitDraw = std::thread(
			   &Render::EndDraw, manager->render, std::ref(finishedDrawSubmit)),
		   {
		       manager->render->EndDraw(finishedDrawSubmit);
		       finishedDrawSubmit = true;
		   }
  );
}
