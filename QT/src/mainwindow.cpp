#include <QApplication>
#include <QtOpenGL>
#include <engine.h>
#include <mainwindow.h>

#include <desktop/fileresourceloader.h>
#include <cv_camera.h>

std::shared_ptr<Engine> engine;

MainWindow::MainWindow(QWidget* parent) : QOpenGLWidget(parent) {
    QSurfaceFormat format;
    format.setGreenBufferSize(8);
    format.setRedBufferSize(8);
    format.setBlueBufferSize(8);
    format.setAlphaBufferSize(8);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setFormat(format);
    glDepthFunc(GL_LEQUAL);
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(render()));
    timer->start(50);
}

void MainWindow::initializeGL() {}

void MainWindow::resizeGL(int w, int h) {
    if (!engine) {
        engine = std::make_shared<Engine>();
        auto rloader = std::make_shared<FileResourceLoader>();
        rloader->setResourcesPath(
            QApplication::applicationDirPath().toStdString() + "/assets/");
        engine->renderer().initRenderer(rloader.get());
        engine->webCam() = std::make_shared<CVCamera>();
        engine->setup(w, h, rloader.get());
    }
    engine->resize(w, h);
    render();
}

void MainWindow::paintGL() {
    engine->step();
}

void MainWindow::render() {
    update();
}
