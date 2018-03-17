struct VertexShaderInput
{
    float4 pos : SV_POSITION;

    #ifdef USE_TEXTURE
    float2 tex : TEXCOORD0;
    #endif //USE_TEXTURE

    #ifdef USE_COLOR
    float4 col : COLOR;
    #endif //USE_COLOR
};

struct VertexShaderOuput
{
    float4 pos : SV_POSITION;

    #ifdef USE_TEXTURE
    float2 tex : TEXCOORD0;
    #endif //USE_TEXTURE

    #ifdef USE_COLOR
    float4 col : COLOR;
    #endif //USE_COLOR
};

VertexShaderOuput VShader(VertexShaderInput input)
{
    VertexShaderOuput output;
    output.pos = input.pos;

    #ifdef USE_TEXTURE
    output.tex = input.tex;
    #endif //USE_TEXTURE

    #ifdef USE_COLOR
    output.col = input.col;
    #endif //USE_COLOR

    return output;
}