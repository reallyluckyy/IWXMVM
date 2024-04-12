struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT ret;
    
    ret.pos = float4(input.pos, 1.0f);
    ret.uv = float2(0.5f, -0.5f) * input.pos.xy + 0.5f;

    ret.pos.xy += float2(-1 / 2560.0f, 1 / 1088.0f) * ret.pos.w;
    return ret;
}