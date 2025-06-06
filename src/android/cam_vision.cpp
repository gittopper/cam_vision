#include <android/androidresourceloader.h>
#include <engine.h>
#include <android/native_camera.h>

std::shared_ptr<Engine> engine;
std::mutex m;

extern "C" {

JNIEXPORT void JNICALL
    Java_com_github_camvision_CamVision_init(JNIEnv* env,
                                                jobject obj,
                                                jint width,
                                                jint height,
                                                jobject javaAssetManager) {
    std::lock_guard<std::mutex> lock(m);
    std::shared_ptr<ResourceLoader> res_loader(new AndroidResourceLoader(
        AAssetManager_fromJava(env, javaAssetManager)));
    if (nullptr == engine) {
        engine = std::make_shared<Engine>();
        engine->setCamBuilder(std::make_shared<CameraBuilder>());
        engine->setup(width, height, res_loader.get());
    }
    engine->renderer().initRenderer(res_loader.get());
    engine->resize(width, height);
}

JNIEXPORT void JNICALL
    Java_com_github_camvision_CamVision_step(JNIEnv* env, jobject obj) {
    std::lock_guard<std::mutex> lock(m);
    engine->step();
}

JNIEXPORT void JNICALL
Java_com_github_camvision_CamVision_tap(JNIEnv* env, jobject obj, jint x, jint y) {
    std::lock_guard<std::mutex> lock(m);
    if(engine) {
        engine->tap(x, y);
    }
}

JNIEXPORT void JNICALL
    Java_com_github_camvision_CamVision_onPause(JNIEnv* env, jobject obj) {
    std::lock_guard<std::mutex> lock(m);
    if(engine) {
        engine->pause();
    }
}

JNIEXPORT void JNICALL
    Java_com_github_camvision_CamVision_onResume(JNIEnv* env, jobject obj,
                                                 jobject javaAssetManager) {
    std::lock_guard<std::mutex> lock(m);
    if (engine) {
        engine->resume();
    }
}

}
