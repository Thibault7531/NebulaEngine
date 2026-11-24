#include "Metal3Surface.h"

#include <QuartzCore/QuartzCore.hpp>
#include <Metal/Metal.hpp>

namespace Nebula::Graphics
{

    Metal3Surface::Metal3Surface(void* window)
        : m_NativeMetal3Surface{SwiftMetal3Surface::CreateMetal3Surface(window)}, m_MetalLayer{m_NativeMetal3Surface.GetMetalLayer()}
    {
    }

    Metal3Surface::~Metal3Surface()
    {
    }

    void Metal3Surface::SetDevice(void* physicalDevice, void* logicalDevice, void* commandQueue)
    {
        m_NativeMetal3Surface.SetDevice(logicalDevice);
        m_MetalDevice = logicalDevice;
        m_MetalCommandQueue = static_cast<MTL::CommandQueue*>(commandQueue);
    }

    void* Metal3Surface::GetSurface() const
    {
        return m_MetalLayer;
    }

    std::string Metal3Surface::GetAPIVersion() const
    {
        return "Metal 3";
    }

    std::string Metal3Surface::GetAPIVendor() const
    {
        MTL::GPUFamily families[] = {
            MTL::GPUFamilyApple10,
            MTL::GPUFamilyApple9,
            MTL::GPUFamilyApple8,
            MTL::GPUFamilyApple7,
            MTL::GPUFamilyApple6,
            MTL::GPUFamilyApple5,
            MTL::GPUFamilyApple4,
            MTL::GPUFamilyApple3,
            MTL::GPUFamilyApple2,
            MTL::GPUFamilyApple1,
            MTL::GPUFamilyMetal3,
            MTL::GPUFamilyMac2,
            MTL::GPUFamilyMac1,
            MTL::GPUFamilyCommon3,
            MTL::GPUFamilyCommon2,
            MTL::GPUFamilyCommon1,
            MTL::GPUFamilyMacCatalyst1,
            MTL::GPUFamilyMacCatalyst2,
            MTL::GPUFamilyMetal3,
            MTL::GPUFamilyMetal4
        };

        std::string gpuFamily = "Unknown";

        for (auto family : families)
        {
            if (static_cast<MTL::Device*>(m_MetalDevice)->supportsFamily(family))
            {
                gpuFamily = GPUFamilyToString(reinterpret_cast<void*>(family));
            }
        }
        return gpuFamily;
    }

    std::string Metal3Surface::GetAPIRenderer() const
    {
        return static_cast<MTL::Device*>(m_MetalDevice)->name()->utf8String();
    }

    std::string Metal3Surface::GPUFamilyToString(void* gpuFamily)
    {
        switch (static_cast<MTL::GPUFamily>(reinterpret_cast<int64_t>(gpuFamily)))
        {
        case MTL::GPUFamilyApple1: return "Apple GPU Family 1 (Apple A7)";
        case MTL::GPUFamilyApple2: return "Apple GPU Family 2 (Apple A8/A8X)";
        case MTL::GPUFamilyApple3: return "Apple GPU Family 3 (Apple A9/A9X/A10/A10X)";
        case MTL::GPUFamilyApple4: return "Apple GPU Family 4 (Apple A11)";
        case MTL::GPUFamilyApple5: return "Apple GPU Family 5 (Apple A12/A12X/A12Z)";
        case MTL::GPUFamilyApple6: return "Apple GPU Family 6 (Apple A13)";
        case MTL::GPUFamilyApple7: return "Apple GPU Family 7 (Apple A14/M1)";
        case MTL::GPUFamilyApple8: return "Apple GPU Family 8 (Apple A15/A16/M2)";
        case MTL::GPUFamilyApple9: return "Apple GPU Family 9 (Apple A17Pro/M3/M4)";
        case MTL::GPUFamilyApple10: return "Apple GPU Family 10";
        case MTL::GPUFamilyMac1: return "Mac GPU Family 1";
        case MTL::GPUFamilyMac2: return "Mac GPU Family 2";
        case MTL::GPUFamilyCommon1: return "Common GPU Family 1";
        case MTL::GPUFamilyCommon2: return "Common GPU Family 2";
        case MTL::GPUFamilyCommon3: return "Common GPU Family 3";
        case MTL::GPUFamilyMacCatalyst1: return "Mac Catalyst GPU Family 1";
        case MTL::GPUFamilyMacCatalyst2: return "Mac Catalyst GPU Family 2";
        case MTL::GPUFamilyMetal3: return "Metal 3 GPU Family";
        case MTL::GPUFamilyMetal4: return "Metal 4 GPU Family";
        }
    }
}
