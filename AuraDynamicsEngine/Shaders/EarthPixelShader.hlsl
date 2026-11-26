cbuffer TransformationBuffer : register(b0)
{
    matrix wvpMatrix;
    matrix worldMatrix;
    matrix normalMatrix;

    float3 cameraPosition;
    float padding;
};

cbuffer EarthLightingBuffer : register(b1)
{
    float3 sunPosition;
    float cloudOffset;
};

// =========================================================================================
// PIXEL SHADER INPUT
// =========================================================================================
struct VS_OUTPUT
{
    float4 pos      : SV_POSITION;
    float3 worldPos : POSITION;
    float3 nrm      : NORMAL;
    float2 uv       : TEXCOORD;
    float3 tangent  : TANGENT;
};

// =========================================================================================
// EARTH SURFACE TEXTURES
// =========================================================================================
Texture2D EarthDayTexture      : register(t0);
Texture2D EarthSpecularTexture : register(t1);
Texture2D EarthNightTexture    : register(t2);
Texture2D EarthNormalTexture   : register(t3);
Texture2D EarthCloudTexture    : register(t4);

SamplerState EarthSampler : register(s0);

// =========================================================================================
// EARTH PIXEL SHADER
// =========================================================================================
float4 main(VS_OUTPUT input) : SV_TARGET
{
    // =====================================================================================
    // WORLD-SPACE SURFACE BASIS
    // =====================================================================================
    const float3 geometryNormal =
        normalize(
            input.nrm
        );

    const float3 tangent =
        normalize(
            input.tangent -
            geometryNormal *
            dot(
                input.tangent,
                geometryNormal
            )
        );

    const float3 bitangent =
        normalize(
            cross(
                geometryNormal,
                tangent
            )
        );

    float3 tangentNormal =
        EarthNormalTexture.Sample(
            EarthSampler,
            input.uv
        ).xyz;

    tangentNormal =
        normalize(
            tangentNormal *
            2.0f -
            1.0f
        );

    const float3x3 tangentToWorld =
        float3x3(
            tangent,
            bitangent,
            geometryNormal
        );

    const float3 surfaceNormal =
        normalize(
            mul(
                tangentNormal,
                tangentToWorld
            )
        );

    // =====================================================================================
    // CAMERA AND SUN DIRECTIONS
    // =====================================================================================
    const float3 directionToCamera =
        cameraPosition -
        input.worldPos;

    const float cameraDistanceSquared =
        dot(
            directionToCamera,
            directionToCamera
        );

    const float3 viewDirection =
        cameraDistanceSquared > 0.000001f
        ? directionToCamera *
            rsqrt(cameraDistanceSquared)
        : geometryNormal;

    const float3 directionToSun =
        sunPosition -
        input.worldPos;

    const float sunDistanceSquared =
        dot(
            directionToSun,
            directionToSun
        );

    const float3 lightDirection =
        sunDistanceSquared > 0.000001f
        ? directionToSun *
            rsqrt(sunDistanceSquared)
        : geometryNormal;

    // =====================================================================================
    // PLANET-SCALE DAY AND NIGHT VISIBILITY
    // =====================================================================================
    // The geometric sphere normal decides which hemisphere physically faces the Sun.
    // Normal-map detail cannot move night lighting onto the daylight hemisphere.
    const float geometricLight =
        dot(
            geometryNormal,
            lightDirection
        );

    const float sphereNdotL =
        saturate(
            geometricLight
        );

    const float dayVisibility =
        smoothstep(
            -0.02f,
            0.04f,
            geometricLight
        );

    const float nightVisibility =
        1.0f -
        smoothstep(
            -0.08f,
            -0.01f,
            geometricLight
        );

    // =====================================================================================
    // EARTH TEXTURE SAMPLES
    // =====================================================================================
    const float3 dayColor =
        EarthDayTexture.Sample(
            EarthSampler,
            input.uv
        ).rgb;

    const float3 nightColor =
        EarthNightTexture.Sample(
            EarthSampler,
            input.uv
        ).rgb;

    const float oceanMask =
        saturate(
            EarthSpecularTexture.Sample(
                EarthSampler,
                input.uv
            ).r
        );

    // =====================================================================================
    // DAYLIGHT SURFACE
    // =====================================================================================
    const float detailNdotL =
        saturate(
            dot(
                surfaceNormal,
                lightDirection
            )
        );

    const float ambientStrength =
        0.015f;

    const float3 ambientColor =
        dayColor *
        ambientStrength *
        dayVisibility;

    const float3 diffuseColor =
        dayColor *
        detailNdotL *
        dayVisibility;

    // =====================================================================================
    // NIGHT-SIDE EMISSION
    // =====================================================================================
    const float nightLightStrength =
        1.0f;

    const float3 nightEmission =
        nightColor *
        nightVisibility *
        nightLightStrength;

    // =====================================================================================
    // OCEAN SPECULAR
    // =====================================================================================
    const float3 combinedDirection =
        lightDirection +
        viewDirection;

    const float combinedLengthSquared =
        dot(
            combinedDirection,
            combinedDirection
        );

    const float3 halfVector =
        combinedLengthSquared > 0.000001f
        ? combinedDirection *
            rsqrt(combinedLengthSquared)
        : surfaceNormal;

    const float normalDotHalf =
        saturate(
            dot(
                surfaceNormal,
                halfVector
            )
        );

    const float specularExponent =
        128.0f;

    const float specularStrength =
        0.15f;

    const float specular =
        pow(
            normalDotHalf,
            specularExponent
        ) *
        sphereNdotL *
        dayVisibility *
        oceanMask;

    const float3 specularColor =
        specular.xxx *
        specularStrength;

    // =====================================================================================
    // CLOUD LAYER
    // =====================================================================================
    float2 cloudUV =
        input.uv;

    cloudUV.x =
        frac(
            cloudUV.x +
            cloudOffset
        );

    const float3 cloudSample =
        EarthCloudTexture.Sample(
            EarthSampler,
            cloudUV
        ).rgb;

    const float cloudBrightness =
        dot(
            cloudSample,
            float3(
                0.299f,
                0.587f,
                0.114f
            )
        );

    const float cloudMask =
        smoothstep(
            0.20f,
            0.75f,
            cloudBrightness
        );

    const float cloudAmbient =
        0.02f;

    const float cloudLighting =
        cloudAmbient +
        sphereNdotL *
        dayVisibility;

    const float3 cloudColor =
        cloudSample *
        cloudLighting;

    const float cloudOpacity =
        saturate(
            cloudMask
        );

    // =====================================================================================
    // FINAL EARTH COLOR
    // =====================================================================================
    const float3 surfaceColor =
        ambientColor +
        diffuseColor +
        specularColor +
        nightEmission;

    const float3 finalColor =
        saturate(
            lerp(
                surfaceColor,
                cloudColor,
                cloudOpacity
            )
        );

    return float4(
        finalColor,
        1.0f
    );
}