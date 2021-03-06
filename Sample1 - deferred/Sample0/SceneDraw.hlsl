Texture2D diffuseTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D depthTex : register(t3);
SamplerState samLinear : register(s0);

struct VSI
{
	float2 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VSO
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VSO VS(VSI IN)
{
	VSO OUT;
	OUT.position = float4(IN.position, 0.f, 1.f);
	OUT.uv = IN.uv;

	return OUT;
}
float4 PS(VSO IN) : SV_TARGET
{
	return diffuseTex.Sample(samLinear, IN.uv);
}