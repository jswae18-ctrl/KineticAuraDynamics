#include "AuraErrors.h"
#include <stdexcept>


AuraError::AuraError(AuraErrorCode errorCode)
    : errorCode(errorCode)
{
    if (errorCode == AuraErrorCode::None)
    {
        throw std::invalid_argument("AuraError cannot be constructed with AuraErrorCode::None");
    }
}
