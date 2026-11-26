#pragma once

enum class AuraErrorCode
{
    None,
    InvalidError,
    DomainError,
    InvalidIdentity,
    DuplicateRegistration,
    NotFound,
    Unauthorized
};
class AuraError final
{
public:
    explicit AuraError(AuraErrorCode errorCode);
    
    
    [[nodiscard]] AuraErrorCode GetErrorCode() const noexcept
    {
        return errorCode;
    }

private:
    AuraErrorCode errorCode;
};
