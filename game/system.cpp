#include "system.h"

Planet::Planet(Resource::Model model, glm::mat4 modelMat) {
    this->model = model;
    this->modelMat = modelMat;
    this->normMat = glm::inverseTranspose(modelMat);
}

void Planet::Draw(Render *render) {
    render->DrawModel(model, modelMat, normMat);
}

System::System(Render *render) {
    Resource::Model planet = render->Load3DModel("models/planet.obj");
    Resource::Texture tex = render->LoadTexture("textures/error.png");
    glm::mat4 middle = glm::scale(
	    glm::mat4(1.0f),
	    glm::vec3(10));
    planet.useShading = false;
    sun = Planet(planet, middle);
    planet.useShading = true;
    middle = glm::scale(glm::mat4(1.0f),
			glm::vec3(5.0f));
    planets.push_back(
	    Planet(planet, glm::translate(middle, glm::vec3(10, 0, 0))));
    planets.push_back(
	    Planet(planet, glm::translate(middle, glm::vec3(30, 20, 0))));
    planet.overrideTexID = (int)tex.ID;
    planets.push_back(
	    Planet(planet, glm::translate(middle, glm::vec3(0, -20, 0))));
}

void System::Draw(Render *render) {
    sun.Draw(render);
    for(auto &p: planets)
	p.Draw(render);
}
