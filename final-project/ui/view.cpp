#include "view.h"
#include "cs123_lib/resourceloader.h"
#include "cs123_lib/errorchecker.h"
#include "shapes/Cylinder.h"
#include "shapes/SpecialShape1.h"
#include "gl/GLDebug.h"
#include "glm.hpp"
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr
#include "lib/CS123XmlSceneParser.h"

#include "viewformat.h"
#include <QApplication>
#include <QKeyEvent>
#include <iostream>
#include <Settings.h>

View::View(QWidget *parent) : QGLWidget(ViewFormat(), parent),
    m_time(), m_timer(), time_elapsed(0), m_captureMouse(false),
    m_shape(nullptr), m_phongProgram(0),
    W_pressed(false), A_pressed(false), S_pressed(false), D_pressed(false),
    J_pressed(false), K_pressed(false), I_pressed(false), L_pressed(false),
    m_renderer(nullptr), m_scene(nullptr)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor
    if (m_captureMouse) {
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The update loop is implemented using a timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    //set up camera parameters
    position = glm::vec3(2,.8,1.0);
    look = glm::vec3(-1,0,0);
    up = glm::vec3(0,1,0);
    right = glm::normalize(glm::cross(up, look));
    settings.power = 1;
    settings.radius = .5;
}

View::~View()
{
}

void View::initializeGL() {
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.
    //initialize glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit(); //Causes invalid enum error
    checkError();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Something is very wrong, glew initialization failed." << std::endl;
    }
    std::cout << "Using GLEW " <<  glewGetString( GLEW_VERSION ) << std::endl;
    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    m_time.start();
    m_timer.start(1000 / 60);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    CS123::GL::checkError();
    CS123::GL::checkError();
    m_shape = std::make_unique<SpecialShape1>(100,100,10, true);
    CS123::GL::checkError();
    CS123XmlSceneParser parser(":/museum.xml");
    CS123::GL::checkError();
    parser.parse();
    CS123::GL::checkError();
    m_scene = std::make_shared<Scene>();
    CS123::GL::checkError();
    m_scene->parse(m_scene.get(), &parser);
    CS123::GL::checkError();
    m_renderer = std::make_unique<Renderer>(m_scene, m_screenwidth, m_screenheight);
    CS123::GL::checkError();
}

void View::paintGL() {

    // TODO: Implement the demo rendering here
    /*
    glUseProgram(m_phongProgram);
    glUniformMatrix4fv(glGetUniformLocation(m_phongProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4x4()));
    glUniformMatrix4fv(glGetUniformLocation(m_phongProgram, "view"), 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(glGetUniformLocation(m_phongProgram, "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
    m_mesh->draw();
    glUseProgram(0);
    */
    m_view = glm::lookAt(position, position + look, up);
    m_renderer->renderScene(m_projection, m_view, m_screenwidth, m_screenheight, time_elapsed);
}

void View::resizeGL(int w, int h) {
    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    w = static_cast<int>(w / ratio);
    h = static_cast<int>(h / ratio);
    m_screenwidth = w;
    m_screenheight = h;
    m_view = glm::lookAt(position, look, up);
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event) {

}

void View::mouseMoveEvent(QMouseEvent *event) {
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    if(m_captureMouse) {
        int deltaX = event->x() - width() / 2;
        int deltaY = event->y() - height() / 2;
        if (!deltaX && !deltaY) return;
        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

        // TODO: Handle mouse movements here
        look = glm::normalize((glm::rotate(-float(.0025*deltaX), up)*glm::vec4(look,0)).xyz());
        right = glm::normalize(glm::cross(up, look));
        look = glm::normalize((glm::rotate(float(.0025*deltaY), right)*glm::vec4(look,0)).xyz());
        std::cout << deltaX << std::endl;
    }
}

void View::mouseReleaseEvent(QMouseEvent *event) {

}

void View::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here
    if (event->key() == Qt::Key_W) {
        W_pressed = true;
        S_pressed = false;
    }
    if (event->key() == Qt::Key_S) {
        S_pressed = true;
        W_pressed = false;
    }
    if (event->key() == Qt::Key_A) {
        A_pressed = true;
        D_pressed = false;
    }
    if (event->key() == Qt::Key_D) {
        D_pressed = true;
        A_pressed = false;
    }
    if (event->key() == Qt::Key_I) {
        I_pressed = true;
        K_pressed = false;
    }
    if (event->key() == Qt::Key_K) {
        K_pressed = true;
        I_pressed = false;
    }
    if (event->key() == Qt::Key_J) {
        J_pressed = true;
        L_pressed = false;
    }
    if (event->key() == Qt::Key_L) {
        L_pressed = true;
        J_pressed = false;
    }
}

void View::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W)
        W_pressed = false;
    if (event->key() == Qt::Key_S)
        S_pressed = false;
    if (event->key() == Qt::Key_A)
        A_pressed = false;
    if (event->key() == Qt::Key_D)
        D_pressed = false;
    if (event->key() == Qt::Key_I)
        I_pressed = false;
    if (event->key() == Qt::Key_J)
        J_pressed = false;
    if (event->key() == Qt::Key_K)
        K_pressed = false;
    if (event->key() == Qt::Key_L)
        L_pressed = false;
}

void View::tick() {
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = m_time.restart() * 0.001f;
    time_elapsed = seconds;

    if (J_pressed) {
        look = glm::normalize((glm::rotate(float(.025), up)*glm::vec4(look,0)).xyz());
        right = glm::normalize(glm::cross(up, look));
    } else if (L_pressed) {
        look = glm::normalize((glm::rotate(-float(.025), up)*glm::vec4(look,0)).xyz());
        right = glm::normalize(glm::cross(up, look));
    }
    if (I_pressed) {
        look = glm::normalize((glm::rotate(-float(.025), right)*glm::vec4(look,0)).xyz());
    } else if (K_pressed) {
        look = glm::normalize((glm::rotate(float(.025), right)*glm::vec4(look,0)).xyz());
    }
    // TODO: Implement the demo update here
    if (W_pressed) {
         //position = position + float(.1)*glm::normalize(glm::vec3(look.xy(),0));

             position = position + float(.03)*look;
             //position.y = 0.1f;

         //printf("%f, %f, %f\n", position.x, position.y, position.z);
     } else if (S_pressed) {
         //position = position + float(.1)*glm::normalize(glm::vec3(look.xy(),0));

             position = position - float(.03)*look;

             //position.y = 0.1f;

     }
     if (A_pressed) {
         //position = position + float(.1)*glm::normalize(glm::vec3(right.xy(),0));
         position = position + float(.03)*right;
         //position.y = 0.1f;

     } else if (D_pressed) {
         //position = position + float(.1)*glm::normalize(glm::vec3(right.xy(),0));
         position = position - float(.03)*right;
         //position.y = 0.1f;

     }

     position.y = 0.6;
    // Flag this view for repainting (Qt will call paintGL() soon after)
//    char buffer [50];
//    sprintf(buffer, "pos: (%f, %f, %f)",position.x, position.y, position.z);
//    std::cout << buffer << std::endl;
//    sprintf(buffer, "look: (%f, %f, %f)",look.x, look.y, look.z);
//    std::cout << buffer << std::endl;

    update();
}
