cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
};

struct VI
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
};

struct VO
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

VO VS(VI input)
{
	VO output;
	output.PosH = mul(float4(input.Pos, 1.f), gWorldViewProj);
	output.Color = input.Color;

	return output;
}

float4 PS(VO input) : SV_Target
{
	return input.Color;
}

technique11 Default
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
};