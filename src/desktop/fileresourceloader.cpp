#include "fileresourceloader.h"

#include <cstring>

template<typename T>
std::vector<T> FileResourceLoader::readf(string filename) {
    string filePath = resourcesPath_ + filename;
    FILE* fin = fopen(filePath.c_str(), "ra");
    if (fin == nullptr) {
        return {};
    }

    fseek(fin, 0L, SEEK_END);
    long sz = ftell(fin);
    fseek(fin, 0L, SEEK_SET);

    std::vector<T> buffer(sz);

    fread(buffer.data(), 1, sz, fin);

    fclose(fin);
    return buffer;
}

std::vector<char> FileResourceLoader::readFile(string filename) {
    return readf<char>(filename);
}

std::vector<unsigned char> FileResourceLoader::readFileU(string filename) {
    return readf<unsigned char>(filename);
}

void FileResourceLoader::setResourcesPath(string path) {
    resourcesPath_ = path;
}
