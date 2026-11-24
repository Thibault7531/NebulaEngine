#pragma once

#include <string>

namespace Nebula::Graphics
{
    class Surface
    {
    public:
        virtual ~Surface() = default;

        virtual std::string GetAPIVersion() const = 0;
        virtual std::string GetAPIVendor() const = 0;
        virtual std::string GetAPIRenderer() const = 0;

        virtual void SetInstance(void* instance) = 0;
        virtual void SetDevice(void* physicalDevice, void* logicalDevice, void* commandQueue) = 0;
        virtual void* GetSurface() const = 0;
    };

}
