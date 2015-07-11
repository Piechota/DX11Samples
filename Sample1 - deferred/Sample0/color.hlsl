Texture2D diffuseTex : register(t0);
cbuffer Matrix : register(b0)
{
	matrix wvpMatrix;
	matrix wMatrix;
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
	float3 w_position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

VSO VS(VSI input)
{
	VSO OUT;

	OUT.position = mul(wvpMatrix, float4(input.position, 1.f));
	OUT.color = float4(input.color, 1.f);
	OUT.w_position = mul(wMatrix, float4(input.position, 1.f));
	OUT.normal = mul(wMatrix, float4(input.normal, 0.f));
	OUT.uv = input.uv;

	return OUT;
}

float4 PS(VSO IN) : SV_TARGET
{
	float3 view = normalize(p0.position - IN.w_position);
	float att = 1.f / (length(p0.position - IN.w_position) * p0.linearAtt);

	return diffuseTex.Sample(samLinear, IN.uv) * float4(p0.color, 1.f) * dot(view, IN.normal) * att;
}