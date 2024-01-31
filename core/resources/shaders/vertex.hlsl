struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal: NORMAL;
    float3 color: COLOR;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float3 normalWorld : NORMAL0;
    float3 positionWorld: NORMAL1;
    float3 color : COLOR;
};

cbuffer Matrices : register(c0)
{
    matrix viewProjectionMatrix : register(c0);
    matrix modelMatrix : register(c4);
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.position = mul(modelMatrix, float4(input.position, 1));
    output.position = mul(viewProjectionMatrix, output.position);
    output.normalWorld = normalize(mul(modelMatrix, float4(input.normal, 0))).xyz;
    output.positionWorld = mul(modelMatrix, float4(input.position, 1)).xyz;
    output.color = input.color;

    return output;
}