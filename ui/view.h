#ifndef VIEW_H
#define VIEW_H

#include "GL/glew.h"
#include <qgl.h>
#include <QTime>
#include <QTimer>
#include <QSettings>
#include "shapes/OpenGLShape.h"
#include "shapes/Mesh.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "scenegraph/Scene.h"
#include "Renderer.h"

class View : public QGLWidget {
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    QTime m_time;
    QTimer m_timer;
    float time_elapsed;
    bool m_captureMouse;

    std::unique_ptr<OpenGLShape> m_shape;
    std::unique_ptr<Mesh> m_mesh;
    std::shared_ptr<Scene> m_scene;
    std::unique_ptr<Renderer> m_renderer;


    glm::vec3 position;
    glm::vec3 look;
    glm::vec3 up;
    glm::vec3 right;
    GLuint m_phongProgram;
    glm::mat4x4 m_projection;
    glm::mat4x4 m_view;

    int m_screenwidth;
    int m_screenheight;

    bool W_pressed;
    bool A_pressed;
    bool S_pressed;
    bool D_pressed;
    bool I_pressed;
    bool J_pressed;
    bool K_pressed;
    bool L_pressed;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void tick();
};

#endif // VIEW_H
