#pragma once
#include <cstddef>
#include "AuraID.h"
#include <functional>

// RETURNS THE HASHED VALUE OF A KEY USING AURA HASHER
struct AuraHasher final
{
    std::size_t operator()(const AuraID& id) const noexcept
    {
        return std::hash<std::string_view>{}(id.GetValue());
    }
};