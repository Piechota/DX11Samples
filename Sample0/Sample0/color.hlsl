Texture2D diffuseTex : register(t0);
cbuffer Matrix : register(b0)
{
	matrix wvpMatrix;
}
SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
};

struct VSI
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};
struct VSO
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VSO VS(VSI input)
{
	VSO OUT;

	OUT.position = mul(wvpMatrix, float4(input.position, 1.f));
	OUT.uv = input.uv;

	return OUT;
}

float4 PS(VSO IN) : SV_TARGET
{
	return diffuseTex.Sample(samLinear, IN.uv);
}