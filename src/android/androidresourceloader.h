#pragma once

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <jni.h>

#include <resourceloader.h>

class AndroidResourceLoader : public ResourceLoader {
  public:
    AndroidResourceLoader(AAssetManager* manager);
    std::vector<char> readFile(string filename) override;
    std::vector<unsigned char> readFileU(string filename) override;
    virtual void setResourcesPath(string path);

    template<typename T>
    std::vector<T> readf(string filename);
  private:
    std::string resPath_;
    AAssetManager* assetManager_;
};

