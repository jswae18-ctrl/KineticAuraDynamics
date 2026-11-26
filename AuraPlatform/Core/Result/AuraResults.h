#pragma once
#include <iostream>
#include <variant>
#include <utility>
#include <type_traits>
#include "../Error/AuraErrors.h"

/* USE: AuraResult<void> :WHEN SUCCESS RETURNS NO VALUE. 
 * SUCCESS = std::monostate, FAILURE = AuraError.
 */
template<typename Success>
class AuraResult final
{
public:
    
    static_assert(!std::is_same_v<Success, AuraError>,
        "AuraResult: Success type cannot be an AuraError type.");

    explicit AuraResult(Success value)
        : result(std::move(value))
    {}

    explicit AuraResult(AuraError error)
        : result(std::move(error))
    {}
    
    [[nodiscard]] bool Succeeded() const noexcept
    {
        // holds_alternative returns true or false.
        return std::holds_alternative<Success>(result);
    }

    [[nodiscard]] const Success& GetValue() const
    {
        return std::get<Success>(result);
    }
    
    [[nodiscard]] const AuraError& GetError() const
    {
        return std::get<AuraError>(result);
    }

private:
    
    // std::variant = This or That
    std::variant<Success, AuraError> result;
    
};


template<>
class AuraResult<void> final
{
public:
    
    AuraResult()
        : result(std::monostate{})
    {}

    explicit AuraResult(AuraError error)
        : result(std::move(error))
    {}
    
    [[nodiscard]] bool Succeeded() const noexcept
    {
        return std::holds_alternative<std::monostate>(result);
    }
    
    [[nodiscard]] const AuraError& GetError() const
    {
        return std::get<AuraError>(result);
    }

private:
    std::variant<std::monostate, AuraError> result;
};

