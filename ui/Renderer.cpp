#include "Renderer.h"
#include "lib/FileLoader.h"
#include "cs123_lib/resourceloader.h"
#include "shapes/Cone.h"
#include "shapes/Cylinder.h"
#include "shapes/Mesh.h"
#include "shapes/Cube.h"
#include "shapes/Sphere.h"
#include "shapes/Torus.h"
#include "Settings.h"
#include "gl/GLDebug.h"
#include <iostream>
#include <tuple>
#include <QImage>
#include "gl/textures/Texture2D.h"
#include "gl/datatype/DirectionalLight.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <random>

Renderer::Renderer(std::shared_ptr<Scene> scene, int width, int height) :
    m_scene(scene)
{

    //set up shader
    std::string shader_vertexSource = FileLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string shader_fragmentSource = FileLoader::loadResourceFileToString(":/shaders/default.frag");
    m_shader = std::make_unique<CS123::GL::CS123Shader>(shader_vertexSource, shader_fragmentSource);

    std::string shadow_vertexSource = FileLoader::loadResourceFileToString(":/shaders/shadow.vert");
    std::string shadow_geometrySource = FileLoader::loadResourceFileToString(":/shaders/shadow.geom");
    std::string shadow_fragmentSource = FileLoader::loadResourceFileToString(":/shaders/shadow.frag");
    point_shadow_shader = std::make_shared<CS123::GL::Shader>(shadow_vertexSource, shadow_geometrySource, shadow_fragmentSource);

    std::string dir_shadow_vertexSource = FileLoader::loadResourceFileToString(":/shaders/dir_shadow.vert");
    std::string dir_shadow_fragmentSource = FileLoader::loadResourceFileToString(":/shaders/dir_shadow.frag");
    dir_shadow_shader = std::make_shared<CS123::GL::Shader>(dir_shadow_vertexSource, dir_shadow_fragmentSource);

    std::string quad_vertexSource = FileLoader::loadResourceFileToString(":/shaders/quad.vert");
    std::string quad_fragmentSource = FileLoader::loadResourceFileToString(":/shaders/quad.frag");
    quad_shader = std::make_shared<CS123::GL::Shader>(quad_vertexSource, quad_fragmentSource);

    std::string gBuf_vertexSource = FileLoader::loadResourceFileToString(":/shaders/gbuffer.vert");
    std::string gBuf_fragmentSource = FileLoader::loadResourceFileToString(":/shaders/gbuffer.frag");
    g_buffer_shader = std::make_unique<CS123::GL::CS123Shader>(gBuf_vertexSource, gBuf_fragmentSource);

    std::string ssao_vertexSource = FileLoader::loadResourceFileToString(":/shaders/ssaoshader.vert");
    std::string ssao_fragmentSource = FileLoader::loadResourceFileToString(":/shaders/ssaoshader.frag");
    ssao_shader = std::make_unique<CS123::GL::CS123Shader>(ssao_vertexSource, ssao_fragmentSource);

    std::string blur_vertexSource = FileLoader::loadResourceFileToString(":/shaders/blur.vert");
    std::string blur_fragmentSource = FileLoader::loadResourceFileToString(":/shaders/blur.frag");
    blur_shader = std::make_unique<CS123::GL::CS123Shader>(blur_vertexSource, blur_fragmentSource);

    std::string final_vertexSource = FileLoader::loadResourceFileToString(":/shaders/final.vert");
    std::string final_fragmentSource = FileLoader::loadResourceFileToString(":/shaders/final.frag");
    final_shader = std::make_unique<CS123::GL::CS123Shader>(final_vertexSource, final_fragmentSource);

    //set up object data
    shape_map.insert({PrimitiveType::PRIMITIVE_CONE,
                      std::make_unique<Cone>(50, 50, 50, true)});
    shape_map.insert({PrimitiveType::PRIMITIVE_CUBE,
                      std::make_unique<Cube>(50, 50, 50)});
    shape_map.insert({PrimitiveType::PRIMITIVE_TORUS,
                      std::make_unique<Torus>(50, 50, 50)});
    shape_map.insert({PrimitiveType::PRIMITIVE_SPHERE,
                      std::make_unique<Sphere>(50, 50, 50)});
    shape_map.insert({PrimitiveType::PRIMITIVE_CYLINDER,
                      std::make_unique<Cylinder>(50, 50, 50, true)});

    const std::vector<PrimitiveAndMatrix> object_data = m_scene->getObjectData();
    for(int i = 0; i < object_data.size(); i++) {
        if (object_data[i].primitive.type == PrimitiveType::PRIMITIVE_MESH) {
            if (mesh_map.count(object_data[i].primitive.meshfile) == 0)
                mesh_map.insert({object_data[i].primitive.meshfile, std::make_unique<Mesh>(object_data[i].primitive.meshfile)});
        }
    }

    checkError();
    //create a frame buffer for every point light
    const std::vector<CS123SceneLightData> static_lights = m_scene->getLights();
    for(int i = 0; i < static_lights.size(); i++) {
        shadow_fbos.push_back(std::make_unique<PointShadowFBO>(SHADOW_WIDTH, SHADOW_HEIGHT, static_lights[i].pos.xyz()));
    }
    renderSceneForPointShadows();
    checkError();

    //create moving direcional lights
    moving_lights.push_back(std::make_unique<DirectionalLight>(glm::vec3(.9,.9,.9), glm::vec3(-2.0,3,1.3), glm::vec3(-2,.5,1.3)));

    //create fullscreen quad
    std::vector<GLfloat> quadData = {-1.0f,1.0f, 0.0f,
                                     0.0f, 0.0f,
                                     -1.0f,-1.0f,0.0f,
                                     0.0, 1.0f,
                                     1.0f, 1.0f, 0.0f,
                                     1.0f, 0.0f,
                                     1.0f, -1.0f, 0.0f,
                                     1.0f, 1.0f};
    m_quad = std::make_unique<OpenGLShape>(true);
    m_quad->setVertexData(quadData, quadData.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, 4);
    m_quad->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->setAttribute(ShaderAttrib::TEXCOORD0, 2, 3*sizeof(GLfloat), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->buildVAO();


    //create GBuffer
    gbuffer = std::make_unique<GBuffer>(SCREENWIDTH,SCREENHEIGHT);

    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    for (int i = 0; i < static_lights.size(); i++) {
        if(static_lights[i].type == LightType::LIGHT_POINT) {
            lightPositions.push_back(glm::vec3(static_lights[i].pos));
            lightColors.push_back(glm::vec3(static_lights[i].color.r, static_lights[i].color.g, static_lights[i].color.b));
        }
    }

    //create ssao kernel
    std::uniform_real_distribution<float> random_nums(0.0, 1.0);
    std::default_random_engine gen;
    for (int i = 0; i < 64; i++) {
        glm::vec3 sample(random_nums(gen) * 2.0 - 1.0, random_nums(gen) * 2.0 - 1.0, random_nums(gen));
        sample = glm::normalize(sample);
        sample *= random_nums(gen);
        float scale = (float)i / 64.0;
        scale = .1f + scale * scale * (1.0f - .1f);
        sample *= scale;
        ssao_kernel.push_back(sample);
    }

    //create ssao noise
    for (int i = 0; i < 16; i++) {
        glm::vec3 noise (random_nums(gen) * 2.0 - 1.0, random_nums(gen) * 2.0 - 1.0, 0.0f);
        ssao_noise.push_back(noise);
    }

    glGenTextures(1, &noise_tex);
    glBindTexture(GL_TEXTURE_2D, noise_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssao_noise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    ssao_fbo = std::make_unique<SSAOFBO>(SCREENWIDTH, SCREENHEIGHT);

    blur_fbo = std::make_unique<BlurFBO>(SCREENWIDTH, SCREENHEIGHT);
    checkError();
}

Renderer::~Renderer() {

}

void Renderer::renderToGBuffer(glm::mat4x4 projection, glm::mat4x4 view) {
    const std::vector<PrimitiveAndMatrix> object_data = m_scene->getObjectData();
    std::vector<CS123SceneLightData> static_lights = m_scene->getLights();
    CS123SceneGlobalData global = m_scene->getGlobals();
    checkError();
    g_buffer_shader->bind();
    gbuffer->bindGBuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    g_buffer_shader->setUniform("projection", projection);
    g_buffer_shader->setUniform("view", view);
    for (int i = 0; i < object_data.size(); i++) {
        PrimitiveAndMatrix p = object_data[i];
        g_buffer_shader->setUniform("model", p.transformation);
        if (p.primitive.material.textureMap.isUsed) {
            applyTexture(p.primitive.material);
            checkError();
        } else {
            g_buffer_shader->setUniform("use_texture", 0);
            checkError();
        }
        g_buffer_shader->setUniform("diffuse_color", global.kd*glm::vec3(p.primitive.material.cDiffuse.xyz()));
        checkError();
        if (p.primitive.type != PrimitiveType::PRIMITIVE_MESH) {
            drawShape(p.primitive.type);
        } else {
            drawMesh(p.primitive.meshfile);
        }
        checkError();
    }
    gbuffer->unbindGBuffer();
    g_buffer_shader->unbind();
}

void Renderer::renderSSAOTexture(glm::mat4x4 projection, glm::mat4x4 view) {
    ssao_shader->bind();
    ssao_fbo->bindFBO();
    glClear(GL_COLOR_BUFFER_BIT);

    //set position texture uniform
    GLint location = glGetUniformLocation(ssao_shader->getID(), "position_tex");
    glUniform1i(location, 0);
    glActiveTexture(GL_TEXTURE0);
    gbuffer->bindPositionTexture();

    //set normal texture uniform
    location = glGetUniformLocation(ssao_shader->getID(), "normal_tex");
    glUniform1i(location, 1);
    glActiveTexture(GL_TEXTURE1);
    gbuffer->bindNormalTexture();

    //set noise texture uniform
    location = glGetUniformLocation(ssao_shader->getID(), "noise_tex");
    glUniform1i(location, 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noise_tex);

    //set kernel sample uniforms
    for (int i = 0; i < 64; i++) {
        GLint location = glGetUniformLocation(ssao_shader->getID(), ("samples[" + std::to_string(i) + "]").c_str());
        glUniform3fv(location, 1, glm::value_ptr(ssao_kernel[i]));
    }

    ssao_shader->setUniform("projection", projection);
    ssao_shader->setUniform("screenwidth", SCREENWIDTH);
    ssao_shader->setUniform("screenheight", SCREENHEIGHT);

    ssao_shader->setUniform("radius", float(settings.radius));
    ssao_shader->setUniform("power", float(settings.power));

    //draw quad
    m_quad->draw();

    ssao_fbo->unbindFBO();
    ssao_shader->unbind();
    checkError();
}

void Renderer::blurSSAOTexture() {
    checkError();
    blur_shader->bind();
    blur_fbo->bindFBO();
    glClear(GL_COLOR_BUFFER_BIT);
    GLint location = glGetUniformLocation(blur_shader->getID(), "ssao_texture");
    checkError();
    glUniform1i(location, 0);
    glActiveTexture(GL_TEXTURE0);
    checkError();
    blur_shader->setUniform("texwidth", SCREENWIDTH);
    blur_shader->setUniform("texheight", SCREENHEIGHT);
    ssao_fbo->bindSSAOTexture();
    m_quad->draw();
    blur_fbo->unbindFBO();
    blur_shader->unbind();
    checkError();
}

void Renderer::renderSceneForPointShadows() {
    const std::vector<PrimitiveAndMatrix> object_data = m_scene->getObjectData();
    std::vector<CS123SceneLightData> static_lights = m_scene->getLights();
    point_shadow_shader->bind();
    //render scene to depth map for every light
    for (int i = 0; i < static_lights.size(); i++) {
        glViewport(0,0,shadow_fbos[i]->getWidth(), shadow_fbos[i]->getHeight());
        shadow_fbos[i]->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        //configure shader uniforms
        shadow_fbos[i]->setUpShader(point_shadow_shader);
        //render scene
        for (int i = 0; i < object_data.size(); i++) {
            point_shadow_shader->setUniform("model", object_data[i].transformation);
            PrimitiveAndMatrix p = object_data[i];
            if (p.primitive.type != PrimitiveType::PRIMITIVE_MESH) {
                drawShape(p.primitive.type);
            } else {
                drawMesh(p.primitive.meshfile);
            }
        }
        shadow_fbos[i]->unbind();
    }
    point_shadow_shader->unbind();
}

void Renderer::renderSceneForDirShadows() {
    const std::vector<PrimitiveAndMatrix> object_data = m_scene->getObjectData();
    std::vector<CS123SceneLightData> static_lights = m_scene->getLights();
    dir_shadow_shader->bind();
    checkError();
    //render scene to depth map for every light
    for (int i = 0; i < moving_lights.size(); i++) {
        moving_lights[i]->update(.01);
        moving_lights[i]->computeTransformation();
        glViewport(0, 0, 1024, 1024);
        moving_lights[i]->bindFBO();
        checkError();
        glClear(GL_DEPTH_BUFFER_BIT);
        //configure shader uniforms
        dir_shadow_shader->setUniform("light_mat", moving_lights[i]->getTransformation());
        checkError();
        //render scene
        for (int i = 0; i < object_data.size(); i++) {
            dir_shadow_shader->setUniform("model", object_data[i].transformation);
            checkError();
            PrimitiveAndMatrix p = object_data[i];
            if (p.primitive.type != PrimitiveType::PRIMITIVE_MESH) {
                drawShape(p.primitive.type);
                checkError();
            } else {
                drawMesh(p.primitive.meshfile);
                checkError();
            }
        }
        moving_lights[i]->unbindFBO();
    }
    dir_shadow_shader->unbind();
    checkError();
}

void Renderer::renderScene(glm::mat4x4 projection, glm::mat4x4 view, int screenwidth, int screenheight, float time_elapsed) {
    checkError();
    //std::cout << settings.power << std::endl;
    //std::cout << settings.radius << std::endl;

    renderToGBuffer(projection, view);
    renderSSAOTexture(projection, view);
    blurSSAOTexture();
    renderFinalImage(view);

    /*
    //render scene for each moving light
    if (settings.use_shadows) {
        renderSceneForDirShadows();
    }
    */

    //render scene to gbuffer

    /*
    //render scene normally
    glViewport(0,0,SCREENWIDTH, SCREENHEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader->bind();
    checkError();
    setStaticLights();
    //setMovingLights();
    if (settings.use_shadows) {
        m_shader->setUniform("use_shadows", 1);
        checkError();
        setShadowCubeMaps();
    } else {
        m_shader->setUniform("use_shadows", 0);
    }
    checkError();
    m_shader->setUniform("p", projection);
    m_shader->setUniform("v", view);
    for (int i = 0; i < object_data.size(); i++) {
        PrimitiveAndMatrix p = object_data[i];
        m_shader->setUniform("m", p.transformation);
        if (p.primitive.material.textureMap.isUsed) {
            applyTexture(p.primitive.material);
        } else {
            m_shader->setUniform("useTexture", 0);
            checkError();
        }
        p.primitive.material.cAmbient *= global.ka;
        p.primitive.material.cDiffuse *= global.kd;
        m_shader->applyMaterial(p.primitive.material);
        checkError();
        if (p.primitive.type != PrimitiveType::PRIMITIVE_MESH) {
            drawShape(p.primitive.type);
        } else {
            drawMesh(p.primitive.meshfile);
        }
        checkError();
    }
    m_shader->unbind();
    */

}

void Renderer::renderFinalImage(glm::mat4x4 view) {
    final_shader->bind();
    setStaticLightsForFinalShader();
    setGBufferAndSSAOMapsForFinalShader(); //texture units 0 through 3
    if (settings.use_shadows) {
        final_shader->setUniform("use_shadows", 1);
        checkError();
        setShadowCubeMapsForFinalShader(); //texture units 4 and above
    } else {
        final_shader->setUniform("use_shadows", 0);
    }
    if (settings.use_ambient_occlusion) {
        final_shader->setUniform("use_ssao", 1);
    } else {
        final_shader->setUniform("use_ssao", 0);
    }
    checkError();
    if (settings.display_ssao_map) {
        final_shader->setUniform("show_ssao_map", 1);
    } else {
        final_shader->setUniform("show_ssao_map", 0);
    }
    checkError();
    final_shader->setUniform("inverse_view", glm::inverse(view));

    glViewport(0,0,SCREENWIDTH, SCREENHEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_quad->draw();
    final_shader->unbind();
}

void Renderer::drawMesh(std::string meshfile) {
    mesh_map[meshfile]->draw();
}

void Renderer::drawShape(PrimitiveType p) {
    switch(p) {
        case PrimitiveType::PRIMITIVE_CONE: {
            shape_map[PrimitiveType::PRIMITIVE_CONE]->draw();
            break;
        }
        case PrimitiveType::PRIMITIVE_CUBE: {
            shape_map[PrimitiveType::PRIMITIVE_CUBE]->draw();
            break;
        }
        case PrimitiveType::PRIMITIVE_TORUS: {
            shape_map[PrimitiveType::PRIMITIVE_TORUS]->draw();
            break;
        }
        case PrimitiveType::PRIMITIVE_SPHERE: {
            shape_map[PrimitiveType::PRIMITIVE_SPHERE]->draw();
            break;
        }
        case PrimitiveType::PRIMITIVE_CYLINDER: {
            shape_map[PrimitiveType::PRIMITIVE_CYLINDER]->draw();
            break;
        }
    }
}

void Renderer::setShadowCubeMaps() {
    for (int i = 0; i < shadow_fbos.size(); i++) {
        //m_shader->setUniformArrayByIndex("depthMaps", i, i);
        //glActiveTexture(GL_TEXTURE0 + i);
        //shadow_fbos[i]->bindCubeMap();
        char buffer[50];
        sprintf(buffer, "depthCubes[%d]", i);
        GLint location = glGetUniformLocation(m_shader->getID(), buffer);
        checkError();
        glUniform1i(location, i+2); //bind the appropriate uniform to the texture unit
        glActiveTexture(GL_TEXTURE2 + i); //set the active texture unit
        shadow_fbos[i]->bindCubeMap(); //bind the cubemap to the active texture unit
        checkError();
    }
}

void Renderer::setShadowCubeMapsForFinalShader() {
    for (int i = 0; i < shadow_fbos.size(); i++) {
        char buffer[50];
        sprintf(buffer, "depthCubes[%d]", i);
        GLint location = glGetUniformLocation(final_shader->getID(), buffer);
        checkError();
        glUniform1i(location, i+4); //bind the appropriate uniform to the texture unit
        glActiveTexture(GL_TEXTURE4 + i); //set the active texture unit
        shadow_fbos[i]->bindCubeMap(); //bind the cubemap to the active texture unit
        checkError();
    }
}

void Renderer::setGBufferAndSSAOMapsForFinalShader() {
    GLint location = glGetUniformLocation(final_shader->getID(), "viewpos_tex");
    glUniform1i(location, 0);
    glActiveTexture(GL_TEXTURE0);
    gbuffer->bindPositionTexture();
    checkError();

    location = glGetUniformLocation(final_shader->getID(), "viewnormal_tex");
    glUniform1i(location, 1);
    glActiveTexture(GL_TEXTURE1);
    gbuffer->bindNormalTexture();
    checkError();

    location = glGetUniformLocation(final_shader->getID(), "diffuse_tex");
    glUniform1i(location, 2);
    glActiveTexture(GL_TEXTURE2);
    gbuffer->bindDiffuseTexture();
    checkError();

    location = glGetUniformLocation(final_shader->getID(), "ssao_tex");
    glUniform1i(location, 3);
    glActiveTexture(GL_TEXTURE3);
    blur_fbo->bindBlurredSSAOTexture();
    checkError();
}

void Renderer::setDepthMaps() {
    for (int i = 0; i < moving_lights.size(); i++) {
        char buffer[50];
        sprintf(buffer, "depthMaps[%d]", 2+i);
        GLint location = glGetUniformLocation(m_shader->getID(), buffer);
        checkError();
        glUniform1i(location, i+5); //bind the appropriate uniform to the texture unit
        glActiveTexture(GL_TEXTURE5 + i); //set the active texture unit
        moving_lights[i]->bindDepthMap(); //bind the cubemap to the active texture unit
        checkError();
    }
}

void Renderer::setStaticLights()
{
    std::vector<CS123SceneLightData> static_lights = m_scene->getLights();
    for (int i = 0; i < static_lights.size(); i++) {
        m_shader->setLight(static_lights[i]);
    }
}

void Renderer::setStaticLightsForFinalShader()
{
    std::vector<CS123SceneLightData> static_lights = m_scene->getLights();
    for (int i = 0; i < static_lights.size(); i++) {
        final_shader->setLight(static_lights[i]);
    }
}

void Renderer::setMovingLights()
{
    for (int i = 0; i < moving_lights.size(); i++) {
        CS123SceneLightData light;
        light.id = i + 2; //FIX!!!!
        light.dir = glm::vec4(moving_lights[i]->getDir(), 0.0);
        light.pos = glm::vec4(moving_lights[i]->getPos(), 1.0);
        light.color = glm::vec4(moving_lights[i]->getColor(), 1.0);
        light.type = LightType::LIGHT_DIRECTIONAL;
        m_shader->setLight(light);
    }
}

void Renderer::applyTexture(CS123SceneMaterial m) {
    //g_buffer_shader->setUniform("ambient_color", glm::vec3(m.cAmbient.xyz()));
    g_buffer_shader->setUniform("diffuse_color", glm::vec3(m.cDiffuse.xyz()));
    //g_buffer_shader->setUniform("specular_color", glm::vec3(m.s.xyz()));
    //g_buffer_shader->setUniform("shininess", material.shininess);
    std::string name = m.textureMap.filename;
    if (filename2texture_map.count(name) == 0) {
        QImage image(name.c_str());
        filename2texture_map[name] = std::make_unique<Texture2D>(image.bits(), image.width(), image.height(), GL_UNSIGNED_BYTE);
        checkError();
    }
    GLint location = glGetUniformLocation(g_buffer_shader->getID(), "tex");
    checkError();
    glUniform1i(location, 0);
    glActiveTexture(GL_TEXTURE0);
    filename2texture_map[name]->bind();
    GLint usetexture_location = glGetUniformLocation(g_buffer_shader->getID(), "use_texture");
    checkError();
    glUniform1i(usetexture_location, 1);
    checkError();
}

