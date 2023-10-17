#pragma once

namespace IWXMVM::Types
{
    struct Keyframe
    {
        std::uint32_t* tick;
        float* value; // TODO: we probably also want vec3 keyframes eventually?

        Keyframe(std::uint32_t* tick, float* value) : tick(tick), value(value)
        {
        }
    };
}