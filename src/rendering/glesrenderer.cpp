#include <rendering/glesrenderer.h>

#define USE_DEPTH_BUFFER 0

namespace {
std::uint32_t loadShader(GLenum type, const char* source) {
    auto shader = glCreateShader(type);
    if (shader == 0) {
        throw std::runtime_error("cannot create shader");
    }

    glShaderSource(shader, 1, (const GLchar**)&source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        char errorMsg[2048];
        glGetShaderInfoLog(shader, sizeof(errorMsg), nullptr, errorMsg);
        glDeleteShader(shader);
        throw std::runtime_error(
            std::string("cannot create shader, error message is ") + errorMsg);
    }

    return shader;
}

std::uint32_t compileProgram(ResourceLoader* loader,
                             const std::string& vertex_shader_name,
                             const std::string& fragment_shader_name) {
    auto vertex_shader_txt = loader->readFile(vertex_shader_name);
    if (vertex_shader_txt.empty()) {
        throw std::runtime_error("cannot read vertex shader");
    }
    vertex_shader_txt.resize(vertex_shader_txt.size() + 1);
    vertex_shader_txt[vertex_shader_txt.size() - 1] = '\0';
    const int vertexShader =
        loadShader(GL_VERTEX_SHADER, vertex_shader_txt.data());
    if (vertexShader == 0) {
        throw std::runtime_error("cannot load vertex shader");
    }

    auto fragment_shader_txt = loader->readFile(fragment_shader_name);
    if (fragment_shader_txt.empty()) {
        throw std::runtime_error("cannot read fragment shader");
    }
    fragment_shader_txt.resize(fragment_shader_txt.size() + 1);
    fragment_shader_txt[fragment_shader_txt.size() - 1] = '\0';
    const int fragmentShader =
        loadShader(GL_FRAGMENT_SHADER, fragment_shader_txt.data());
    if (fragmentShader == 0) {
        throw std::runtime_error("cannot load fragment shader");
    }

    auto program_id = glCreateProgram();
    if (program_id == 0) {
        throw std::runtime_error("cannot create program");
    }
    glAttachShader(program_id, vertexShader);
    glAttachShader(program_id, fragmentShader);
    glLinkProgram(program_id);
    int linked;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
    if (linked == 0) {
        glDeleteProgram(program_id);
        throw std::runtime_error("cannot link program");
    }

    return program_id;
}
}  // namespace

GLESRenderer::GLESRenderer() {}

bool GLESRenderer::initRenderer(ResourceLoader* loader) {
    program_overlay_id_ =
        compileProgram(loader, "sprite_vs.txt", "sprite_fs.txt");
    glUseProgram(program_overlay_id_);
    overlay_vert_loc_ = glGetAttribLocation(program_overlay_id_, "a_Position");
    overlay_tex_loc_ =
        glGetAttribLocation(program_overlay_id_, "a_Texture");  // texture
    overlay_mat_loc_ =
        glGetUniformLocation(program_overlay_id_, "u_mvpMatrix");  // texture

    return true;
}

void GLESRenderer::createFramebuffer() {
    glGenFramebuffers(1, &viewFramebuffer_);
    glGenRenderbuffers(1, &viewRenderbuffer_);

    glBindFramebuffer(GL_FRAMEBUFFER_OES, viewFramebuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER_OES, viewRenderbuffer_);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
                              GL_RENDERBUFFER_OES, viewRenderbuffer_);
}
bool GLESRenderer::updateInfoAboutWindow() {
    int w, h;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES,
                                 &w);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER_OES,
                                 GL_RENDERBUFFER_HEIGHT_OES, &h);
    camera_->setViewport(w, h);

    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffers(1, &depthRenderbuffer_);
        glBindRenderbuffer(GL_RENDERBUFFER_OES, depthRenderbuffer_);
        glRenderbufferStorage(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES,
                              camera_->width(), camera_->height());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES,
                                  GL_RENDERBUFFER_OES, depthRenderbuffer_);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER_OES) !=
        GL_FRAMEBUFFER_COMPLETE_OES) {
        return false;
    }

    return true;
}

void GLESRenderer::prepareFrame() {
    float aspect = camera_->aspect();

    float ws = camera_->internalWidth() / 2;
    float hs = camera_->internalHeight() / 2;

    SetOrtho(proj_, -ws, ws, -hs, hs, -1, 1);

#ifdef __APPLE__
    glBindFramebuffer(GL_FRAMEBUFFER_OES, viewFramebuffer_);
#endif
    glViewport(0, 0, camera_->width(), camera_->height());

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program_overlay_id_);
}

void GLESRenderer::showFrame() {
#ifdef __APPLE__
    glBindRenderbuffer(GL_RENDERBUFFER_OES, viewRenderbuffer_);
#endif
}

void GLESRenderer::destroyFramebuffer() {
    glDeleteFramebuffers(1, &viewFramebuffer_);
    viewFramebuffer_ = 0;
    glDeleteRenderbuffers(1, &viewRenderbuffer_);
    viewRenderbuffer_ = 0;

    if (USE_DEPTH_BUFFER) {
        glDeleteRenderbuffers(1, &depthRenderbuffer_);
        depthRenderbuffer_ = 0;
    }
}

void GLESRenderer::drawOverlay(const void* data,
                                        int data_width,
                                        int data_height) {
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data_width, data_height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);

    float vertices[] = {
        camera_->xLeft(),  camera_->yTop(),    0.f,
        camera_->xLeft(),  camera_->yBottom(), 0.f,
        camera_->xRight(), camera_->yTop(),    0.f,
        camera_->xRight(), camera_->yBottom(), 0.f,
    };

    float texture[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

    glUseProgram(program_overlay_id_);
    glVertexAttribPointer(overlay_vert_loc_, 3, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(overlay_vert_loc_);

    glVertexAttribPointer(overlay_tex_loc_, 2, GL_FLOAT, false, 0, texture);
    glEnableVertexAttribArray(overlay_tex_loc_);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
    glUniformMatrix4fv(overlay_mat_loc_, 1, GL_FALSE, (GLfloat*)&proj_.m_[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture_id);
}

void GLESRenderer::drawOverlay(const Sprite& sprite) {
    drawOverlay(sprite.data(), sprite.glWidth(), sprite.glHeight());
}
