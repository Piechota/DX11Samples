cbuffer ConstBuffer : register(b0)
{
	float4x4 wvpMatrix;
}

struct VSI
{
	float3 position : POSITION;
	float4 color : COLOR;
};
struct VSO
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};
VSO VS( VSI input )
{
	VSO output;
	output.position = mul(float4(input.position, 1.f), wvpMatrix);
	output.color = input.color;

	return output;
}

float4 PS(VSO input) : SV_Target
{
	return input.color;	
}