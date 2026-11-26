#pragma once
#include <DirectXMath.h>

class PlanetOrbit
{
public:
    PlanetOrbit(float orbitRadius, float orbitSpeed);
    
    void Update(float deltaTime);
    
    DirectX::XMFLOAT3 GetPosition() const noexcept;
   
private:
    float orbitRadius;
    float orbitSpeed;
    float orbitAngle = 0.0f;
    
};
