#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

class ResourceLoader {
  public:
    virtual std::vector<char> readFile(string filename) = 0;
    virtual void setResourcesPath(string path) = 0;

    virtual ~ResourceLoader() {}
};
using ResourceLoaderPtr = std::shared_ptr<ResourceLoader>;
