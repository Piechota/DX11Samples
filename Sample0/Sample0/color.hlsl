cbuffer Matrix : register(b0)
{
	matrix wvpMatrix;
}

struct VSI
{
	float3 position : POSITION;
	float3 color : COLOR;
};
struct VSO
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VSO VS(VSI input)
{
	VSO OUT;

	OUT.position = mul(float4(input.position, 1.f), wvpMatrix);
	OUT.color = float4(input.color, 1.f);

	return OUT;
}

float4 PS(VSO IN) : SV_TARGET
{
	return IN.color;
}