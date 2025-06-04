#pragma once

#ifndef __APPLE__
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#endif

#include <math/mat44.h>
#include <rendering/renderer.h>

#include <memory>

class GLESRenderer : virtual public Renderer {
  public:
    GLESRenderer();
    void createFramebuffer() override;
    void destroyFramebuffer() override;
    bool updateInfoAboutWindow() override;

    void prepareFrame() override;
    void showFrame() override;

    void setCamera(Camera* camera) override {
        camera_ = camera;
    }

    bool initRenderer(ResourceLoader* loader) override;
    void drawOverlay(const void* data, int data_width, int data_height);
    void drawOverlay(const Sprite& sprite) override;

  protected:
    Camera* camera_;

    std::uint32_t program_overlay_id_;
    std::uint32_t overlay_vert_loc_;
    std::uint32_t overlay_tex_loc_;
    std::uint32_t overlay_mat_loc_;


    Math::Mat44 proj_;

    GLuint viewRenderbuffer_, viewFramebuffer_;
    GLuint depthRenderbuffer_;
};
