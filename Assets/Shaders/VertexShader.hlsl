struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR, float2 texCoord : TEXCOORD0)
{
    VOut output;

    output.position = position;
    output.color = color;
    output.texCoord = texCoord;

    return output;
}