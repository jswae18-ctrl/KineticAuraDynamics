#pragma once
#include "../Core/AuraDiagnosticEvent.h"
class IAuraDiagnosticSink
{
public:
    
    virtual ~IAuraDiagnosticSink() = default;
    
    virtual void Write(const AuraDiagnosticEvent& event) = 0;
    
};
