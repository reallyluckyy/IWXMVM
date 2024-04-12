struct PS_INPUT
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

sampler2D depthTex : register(s0);

float GetLinearizedDepth(float2 texcoord)
{
    float depth = tex2Dlod(depthTex, float4(texcoord, 0, 0)).x;
    const float N = 1.0f;
    const float farPlane = 1000.0f;
    depth /= farPlane - depth * (farPlane - N);
    return depth;
}

float3 GetScreenSpaceNormal(float2 texcoord)
{
    float3 offset = float3(1.0f / 2560.0f, 1.0f / 1088.0f, 0.0f);
    float2 posCenter = texcoord.xy;
    float2 posNorth  = posCenter - offset.zy;
    float2 posEast   = posCenter + offset.xz;

    float3 vertCenter = float3(posCenter - 0.5f, 1.0f) * GetLinearizedDepth(posCenter);
    float3 vertNorth  = float3(posNorth - 0.5f,  1.0f) * GetLinearizedDepth(posNorth);
    float3 vertEast   = float3(posEast - 0.5f,   1.0f) * GetLinearizedDepth(posEast);

    return normalize(cross(vertCenter - vertNorth, vertCenter - vertEast)) * 0.5f + 0.5f;
}

float4 main(PS_INPUT input) : COLOR
{
    // float3 normal = GetScreenSpaceNormal(input.uv);
/*
    if (depth < 0.001f)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
*/
    float depth = GetLinearizedDepth(input.uv);
    return float4(depth, depth, depth, 1.0f);
}