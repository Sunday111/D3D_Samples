Texture2D shaderTexture;
SamplerState SampleType;

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texCoord : TEXCOORD0) : SV_TARGET
{
    float4 textureColor = shaderTexture.Sample(SampleType, texCoord);
    return (textureColor + color) * 0.5f;
}