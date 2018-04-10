#ifdef USE_TEXTURE
    Texture2D shaderTexture;
    SamplerState SampleType;
#endif /*USE_TEXTURE*/

struct FragmentShaderInput
{
    float4 position : SV_POSITION;

    #ifdef USE_TEXTURE
    float2 tex : TEXCOORD0;
    #endif //USE_TEXTURE

    #ifdef USE_COLOR
    float4 color : COLOR;
    #endif //USE_COLOR
};


float4 PShader(FragmentShaderInput input) : SV_TARGET
{
    #ifdef USE_TEXTURE
        float4 col = shaderTexture.Sample(SampleType, input.tex);
        #ifdef GRAYSACLE_TEXTURE
            return float4(col.r, col.r, col.r, 1.f);
        #else
            return shaderTexture.Sample(SampleType, input.tex);
        #endif /* GRAYSACLE_TEXTURE */
    #endif /*USE_TEXTURE*/

    #ifdef USE_COLOR
    return input.color;
    #endif /*USE_COLOR*/

    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
