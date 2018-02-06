struct VS_OUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUT VShader(float2 pos : POSITION, float2 uv : TEXCOORD0)
{
    VS_OUT output;
    output.pos = float4(pos, 0, 1);
    output.uv = uv;
    return output;
}

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D rt_1;
Texture2D rt_2;
Texture2D rt_1_ds;
Texture2D rt_2_ds;

float4 PShader(VS_OUT input) : SV_TARGET {
    float2 uv = input.uv;

    float rt_1_depth = dot(
        rt_1_ds.Sample(Sampler, uv),
        rt_1_ds.Sample(Sampler, uv));

    float rt_2_depth = dot(
        rt_2_ds.Sample(Sampler, uv),
        rt_2_ds.Sample(Sampler, uv));

    float4 result;

    if (rt_1_depth < rt_2_depth) {
        result = rt_1.Sample(Sampler, uv);
    } else {
        result = rt_2.Sample(Sampler, uv);
    }

    return result;
}