#pragma once

#include "Nebula/Renderer/Renderer.h"

namespace Nebula
{
    /**
     * Class defining textures
     */
    class Texture
    {
    public:
        virtual ~Texture() = default;

        /**
         * Gets the width of the texture.
         * @return Width of the texture
         */
        virtual uint32_t GetWidth() const = 0;
        /**
         * Gets the height of the texture.
         * @return Height of the texture
         */
        virtual uint32_t GetHeight() const = 0;

        /**
         * Binds the texture to a specified slot.
         * @param slot Slot to bind the texture
         */
        virtual void Bind(uint32_t slot = 0) const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D>Create(const std::string& path);
    };

}
