#pragma once

#include <stdexcept>
#include "../../../AuraPlatform/Core/Error/AuraErrors.h"


enum class UIErrors
{
    None,
    InvalidID,
    InvalidDefinition,
    InvalidProperty,
    DuplicateID,
    NotFound,
    UnsupportedOperation,
    ValidationFailed
};

class UIError final
{
public:

    explicit UIError(UIErrors error)
        : auraError(AuraErrorCode::DomainError),
          error(error)
    {
        if (error == UIErrors::None)
        {
            throw std::invalid_argument("UIError cannot be constructed with UIErrors::None");
        }
    }

    [[nodiscard]] UIErrors GetUIError() const noexcept
    {
        return error;
    }

    [[nodiscard]] const AuraError& GetAuraError() const noexcept
    {
        return auraError;
    }

private:
    AuraError auraError;
    UIErrors error;
};