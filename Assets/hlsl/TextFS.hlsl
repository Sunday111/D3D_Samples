Texture2D shaderTexture;
SamplerState SampleType;

struct FragmentShaderInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 PShader(FragmentShaderInput input) : SV_TARGET
{
    float col = shaderTexture.Sample(SampleType, input.tex).r;
    return float4(col.r, col.r, col.r, col.r);
}
