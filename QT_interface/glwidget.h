#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    int width;
    int height;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void configure();
    void draw_axes( void );
    void processHits (GLint hits, GLuint buffer[]);
    void SelectObjectAtXY(int x, int y);
    void MouseMoveToRotate(int diffX, int diffY);
    void MouseMoveToSeedpointMove(float diffX, float diffY);
    void MouseMoveToZoom(int diffX, int diffY);
    void mouseMoveEvent(QMouseEvent *event);
    void Magnify();
    void mouseMotion(int x, int y);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


};

#endif // GLWIDGET_H
