#include "Scene.h"
#include "lib/CS123ISceneParser.h"

#include "glm/gtx/transform.hpp"


Scene::Scene()
{
}

Scene::Scene(Scene &scene)
{
    // We need to set the global constants to one when we duplicate a scene,
    // otherwise the global constants will be double counted (squared)
    CS123SceneGlobalData global = { 1, 1, 1, 1};
    setGlobal(scene.m_global);
    // TODO [INTERSECT]
    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.
    for (int i = 0; i < scene.lights.size(); i++) {
        this->addLight(scene.lights[i]);
    }

    for (int i = 0; i < scene.object_data.size(); i++) {
        this->addPrimitive(scene.object_data[i].primitive, scene.object_data[i].transformation);
    }

}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

glm::mat4x4 Scene::transformation2Matrix(CS123SceneTransformation *transformation) {
    glm::mat4x4 mat;
    switch(transformation->type) {
        case TRANSFORMATION_SCALE:
            mat = glm::scale(transformation->scale);
            break;
        case TRANSFORMATION_TRANSLATE:
            mat = glm::translate(transformation->translate);
            break;
        case TRANSFORMATION_ROTATE:
            mat = glm::rotate(transformation->angle, transformation->rotate);
            break;
        case TRANSFORMATION_MATRIX:
            mat = transformation->matrix;
            break;
    }
    return mat;
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and
    //   finishParsing()

    //get and store global data
    CS123SceneGlobalData new_global;
    parser->getGlobalData(new_global);
    sceneToFill->m_global = new_global;

    //get and store light data
    int num_lights = parser->getNumLights();
    for (int i = 0; i < num_lights; i++) {
        CS123SceneLightData new_light;
        parser->getLightData(i,new_light);
        sceneToFill->addLight(new_light);
    }

    //get and store primitive data with transformations
    CS123SceneNode *root = parser->getRootNode();
    parseHelper(sceneToFill, root, glm::mat4x4());
}

void Scene::parseHelper(Scene *sceneToFill, CS123SceneNode *node, glm::mat4x4 composed_matrix) {
    for (int i = 0; i < node->transformations.size(); i++) {
        composed_matrix = composed_matrix * transformation2Matrix(node->transformations[i]);
    }

    for (int i = 0; i < node->primitives.size(); i++) {
        sceneToFill->addPrimitive(*node->primitives[i], composed_matrix);
    }

    for (int i = 0; i < node->children.size(); i++) {
        parseHelper(sceneToFill, node->children[i], composed_matrix);
    }
}

void Scene::addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix) {
    PrimitiveAndMatrix new_primitive = {matrix, scenePrimitive};
    object_data.push_back(new_primitive);
}

void Scene::addLight(const CS123SceneLightData &sceneLight) {
    lights.push_back(sceneLight);
}

void Scene::setGlobal(const CS123SceneGlobalData &global) {
    m_global = global;
}

const std::vector<PrimitiveAndMatrix>& Scene::getObjectData() {
    return object_data;
}
const std::vector<CS123SceneLightData>& Scene::getLights() {
    return lights;
}
const CS123SceneGlobalData Scene::getGlobals() {
    return m_global;
}

