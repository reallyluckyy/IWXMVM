struct PS_INPUT
{
    float4 position : POSITION1;
    float3 normalWorld : NORMAL0;
    float3 positionWorld : NORMAL1;
    float3 color : COLOR;
};

cbuffer LightInfo : register(c12)
{
    float4 lightColor : register(c12);
    float4 lightDirection : register(c13);
    float4 camPosition : register(c14);
    float4 filmtweaksParams : register(c15);
};

float4 main(PS_INPUT input) : COLOR
{
    float4 ambient = float4(0.45, 0.45, 0.45, 1.0);
    float4 diffuse = max(dot(lightDirection.xyz, input.normalWorld), 0) * lightColor;
    float specularLight = 1;
    float3 viewDirection = normalize(camPosition.xyz - input.positionWorld);
    float3 reflectionDirection = reflect(-lightDirection.xyz, input.normalWorld);
    float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0), 16);
    float specular = specularAmount * specularLight;

    float4 outputColor = float4(input.color, 0) * (diffuse + ambient + specular);

    float3 coeff = float3(0.2125, 0.7154, 0.0721);
    outputColor.rgb = (outputColor.rgb - 0.5) * filmtweaksParams[1] + 0.5;
    outputColor.rgb = outputColor.rgb * filmtweaksParams[3];
    outputColor.rgb = outputColor.rgb + filmtweaksParams[0];
    float intensity = dot(outputColor.rgb, coeff);
    outputColor.rgb = lerp(intensity, outputColor.rgb, 1 - filmtweaksParams[2]);

    return outputColor;
}