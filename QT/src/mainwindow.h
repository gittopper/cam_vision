#pragma once

#include <QOpenGLWidget>
#include <QTimer>
#include <QtOpenGL>

class MainWindow : public QOpenGLWidget {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = 0);

  protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;


  protected slots:
    void render();  // Определяем координаты объектов
};
