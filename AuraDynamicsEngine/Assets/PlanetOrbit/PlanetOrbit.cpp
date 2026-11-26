#include "PlanetOrbit.h"
#include <cmath>

PlanetOrbit::PlanetOrbit(float orbitRadius, float orbitSpeed)
    : orbitRadius(orbitRadius), orbitSpeed(orbitSpeed)
{}

void PlanetOrbit::Update(float deltaTime)
{
    if (!std::isfinite(deltaTime))
    {
        return;
    }
    orbitAngle += orbitSpeed * deltaTime;
    
    if (orbitAngle >= DirectX::XM_2PI)
    {
        orbitAngle -= DirectX::XM_2PI;
    }
}

DirectX::XMFLOAT3 PlanetOrbit::GetPosition() const noexcept
{
    return
    {
        std::cos(orbitAngle) * orbitRadius, 0.0f, std::sin(orbitAngle) * orbitRadius
    };
}
