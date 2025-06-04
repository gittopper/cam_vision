#include "fileresourceloader.h"

#include <cstring>

std::vector<char> FileResourceLoader::readFile(string filename) {
    string filePath = resourcesPath_ + filename;
    FILE* fin = fopen(filePath.c_str(), "ra");
    if (fin == nullptr) {
        return {};
    }

    fseek(fin, 0L, SEEK_END);
    long sz = ftell(fin);
    fseek(fin, 0L, SEEK_SET);

    std::vector<char> buffer(sz);

    fread(buffer.data(), 1, sz, fin);

    fclose(fin);
    return buffer;
}

void FileResourceLoader::setResourcesPath(string path) {
    resourcesPath_ = path;
}
