#pragma once

#include <resourceloader.h>

class FileResourceLoader : public ResourceLoader {
  public:
    virtual std::vector<char> readFile(std::string filename);
    virtual void setResourcesPath(std::string path);

  private:
    std::string resourcesPath_;
};
