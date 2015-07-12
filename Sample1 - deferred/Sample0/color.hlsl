Texture2D diffuseTex : register(t0);
cbuffer Matrix : register(b0)
{
	matrix wvpMatrix;
	matrix wvMatrix;
}
SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
};

struct PointLight
{
	float3 position;
	float3 color;
	float linearAtt;
	float test;
};

cbuffer Lights : register(b1)
{
	PointLight p0;
}

struct VSI
{
	float3 position : POSITION;
	float3 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};
struct VSO
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

VSO VS(VSI input)
{
	VSO OUT;

	OUT.position = mul(wvpMatrix, float4(input.position, 1.f));
	OUT.color = float4(input.color, 1.f);
	OUT.normal = mul(wvMatrix, float4(input.normal, 0.f));
	OUT.uv = input.uv;

	return OUT;
}

struct PSO
{
	float4 albedo : SV_TARGET0;
	float4 normal : SV_TARGET1;
};

PSO PS(VSO IN)
{
	PSO OUT;

	OUT.albedo = diffuseTex.Sample(samLinear, IN.uv);
	OUT.normal = float4(mad(IN.normal, .5f, .5f), 1.f);

	return OUT;
}