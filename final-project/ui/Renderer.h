#ifndef RENDERER_H
#define RENDERER_H

#include "gl/shaders/CS123Shader.h"
#include "gl/shaders/Shader.h"
#include "scenegraph/Scene.h"
#include <memory>
#include "shapes/OpenGLShape.h"
#include "shapes/Mesh.h"
#include "gl/datatype/PointShadowFBO.h"
#include "gl/datatype/DirectionalLight.h"
#include "gl/datatype/GBuffer.h"
#include "gl/datatype/SSAOFBO.h"
#include "gl/datatype/BlurFBO.h"

class Renderer
{
public:
    Renderer(std::shared_ptr<Scene> scene, int screen_width, int screen_height);
    ~Renderer();

    void renderScene(glm::mat4x4 projection, glm::mat4x4 view, int screenwidth, int screenheight, float time_elapsed);

private:
    std::shared_ptr<Scene> m_scene;
    std::unique_ptr<CS123::GL::CS123Shader> m_shader;
    std::shared_ptr<CS123::GL::Shader> point_shadow_shader;
    std::shared_ptr<CS123::GL::Shader> dir_shadow_shader;
    std::shared_ptr<CS123::GL::Shader> quad_shader;
    std::shared_ptr<CS123::GL::Shader> g_buffer_shader;
    std::shared_ptr<CS123::GL::Shader> ssao_shader;
    std::shared_ptr<CS123::GL::Shader> blur_shader;
    std::shared_ptr<CS123::GL::CS123Shader> final_shader;

    std::map<PrimitiveType, std::unique_ptr<OpenGLShape>> shape_map;
    std::map<std::string, std::unique_ptr<Mesh>> mesh_map;
    std::map<std::string, std::unique_ptr<Texture2D>> filename2texture_map;

    std::vector<std::unique_ptr<PointShadowFBO>> shadow_fbos;
    std::vector<std::unique_ptr<DirectionalLight>> moving_lights;

    std::unique_ptr<GBuffer> gbuffer;

    std::unique_ptr<OpenGLShape> m_quad;

    std::vector<glm::vec3> ssao_kernel;
    std::vector<glm::vec3> ssao_noise;
    unsigned int noise_tex;

    std::unique_ptr<SSAOFBO> ssao_fbo;
    std::unique_ptr<BlurFBO> blur_fbo;

    void tessellate();
    void drawShape(PrimitiveType p);
    void drawMesh(std::string filename);
    void setStaticLights();
    void setMovingLights();
    void renderSceneForPointShadows();
    void renderSceneForDirShadows();
    void setShadowCubeMaps();
    void setDepthMaps();
    void applyTexture(CS123SceneMaterial m);
    void renderToGBuffer(glm::mat4x4 projection, glm::mat4x4 view);
    void renderSSAOTexture(glm::mat4x4 projection, glm::mat4x4 view);
    void blurSSAOTexture();
    void setStaticLightsForFinalShader();
    void setShadowCubeMapsForFinalShader();
    void setGBufferAndSSAOMapsForFinalShader();
    void renderFinalImage(glm::mat4x4 view);

    const int SHADOW_WIDTH = 2000;
    const int SHADOW_HEIGHT = 2000;

    const int SCREENWIDTH = 600;
    const int SCREENHEIGHT = 600;

    bool got_shadows;
};

#endif // RENDERER_H
