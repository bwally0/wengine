#pragma once

#include <string>
#include <vector>

class ResourceRegistry;

class IModule 
{
public:
    virtual void init(ResourceRegistry& resources) = 0;
    virtual void update(double deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    virtual std::vector<std::string> getDependencies() { return {}; }
    virtual ~IModule() = default;
};
