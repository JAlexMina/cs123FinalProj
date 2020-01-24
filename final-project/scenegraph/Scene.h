#ifndef SCENE_H
#define SCENE_H

#include "lib/CS123SceneData.h"
#include <QList>

class Camera;
class CS123ISceneParser;

struct PrimitiveAndMatrix {
   glm::mat4x4 transformation;

   CS123ScenePrimitive primitive;

};

/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged() {}

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);

    const std::vector<PrimitiveAndMatrix> &getObjectData();
    const std::vector<CS123SceneLightData> &getLights();
    const CS123SceneGlobalData getGlobals();

protected:

    // Adds a primitive to the scene.
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    // Helper function for parse
    static void parseHelper(Scene *sceneToFill, CS123SceneNode *node, glm::mat4x4 composed_matrix);

    static glm::mat4x4 transformation2Matrix(CS123SceneTransformation *transformation);

    std::vector<PrimitiveAndMatrix> object_data;

    std::vector<CS123SceneLightData> lights;

    CS123SceneGlobalData m_global;

};

#endif // SCENE_H
