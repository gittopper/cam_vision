#pragma once

#include <resourceloader.h>

class FileResourceLoader : public ResourceLoader {
  public:
    template<typename T> std::vector<T> readf(std::string filename);

    std::vector<char> readFile(std::string filename) override;
    std::vector<unsigned char> readFileU(std::string filename) override;
    void setResourcesPath(std::string path) override;

  private:
    std::string resourcesPath_;
};
