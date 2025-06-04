#include <android/androidresourceloader.h>

AndroidResourceLoader::AndroidResourceLoader(AAssetManager* manager)
    : assetManager_(manager) {}
std::vector<char> AndroidResourceLoader::readFile(std::string filename) {
    AAssetDir* assetDir =
        AAssetManager_openDir(assetManager_, resPath_.c_str());
    if (!assetDir) {
        return {};
    }

    AAsset* asset =
        AAssetManager_open(assetManager_, filename.c_str(), AASSET_MODE_BUFFER);
    if (asset == nullptr) {
        return {};
    }
    long length = AAsset_getLength(asset);

    std::vector<char> buffer(length);
    long offset = 0;
    long readed = 0;
    while ((readed = AAsset_read(asset, buffer.data() + offset, length - offset)) >
           0) {
        offset += readed;
    }

    AAsset_close(asset);
    AAssetDir_close(assetDir);

    return buffer;
}
void AndroidResourceLoader::setResourcesPath(std::string path) { resPath_ = path; }
