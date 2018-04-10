struct VertexShaderInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

struct VertexShaderOuput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

cbuffer Constants : register(b0) {
    float4x4 WorldMatrix;
}; 

VertexShaderOuput VShader(VertexShaderInput input)
{
    VertexShaderOuput output;
    output.pos = mul(WorldMatrix, input.pos);
    output.tex = input.tex;

    return output;
}