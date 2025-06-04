#pragma once

#include <resourceloader.h>
#include <rendering/camera.h>
#include <sprite.h>

#include <string>

class Renderer {
  public:
    virtual bool initRenderer(ResourceLoader* loader) = 0;

    virtual void createFramebuffer() = 0;
    virtual void destroyFramebuffer() = 0;
    virtual bool updateInfoAboutWindow() = 0;

    virtual void prepareFrame() = 0;
    virtual void drawOverlay(const Sprite& sprite) = 0;

    virtual void showFrame() = 0;

    virtual void setCamera(Camera*) = 0;

    virtual ~Renderer() {}
};
