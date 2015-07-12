Texture2D diffuseTex : register(t0);
cbuffer Matrix : register(b0)
{
	matrix wvpMatrix;
}
SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
};
struct GSO
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

const static float3 boxOffsets[] =
{
	float3(-1.f, +1.f, -1.f), float3(+1.f, +1.f, -1.f),
	float3(-1.f, -1.f, -1.f), float3(+1.f, -1.f, -1.f),

	float3(-1.f, -1.f, +1.f), float3(+1.f, -1.f, +1.f),
	float3(-1.f, +1.f, +1.f), float3(+1.f, +1.f, +1.f)
};

float3 VS(float3 position : POSITION) : POSITION
{
	return position;
}

[maxvertexcount(16)]
void GS(point float3 p[1] : POSITION, inout TriangleStream<GSO> triStream)
{
	GSO OUT;

	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[0], 1.f));
	OUT.uv = float2(0.f, 0.f);
	triStream.Append(OUT);
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[1], 1.f));
	OUT.uv = float2(1.f, 0.f);
	triStream.Append(OUT);
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[2], 1.f));
	OUT.uv = float2(0.f, 1.f);				  
	triStream.Append(OUT);					  
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[3], 1.f));
	OUT.uv = float2(1.f, 1.f);				  
	triStream.Append(OUT);					  
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[4], 1.f));
	OUT.uv = float2(0.f, 0.f);				  
	triStream.Append(OUT);					  
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[5], 1.f));
	OUT.uv = float2(1.f, 0.f);				  
	triStream.Append(OUT);					  
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[6], 1.f));
	OUT.uv = float2(0.f, 1.f);				  
	triStream.Append(OUT);					  
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[7], 1.f));
	OUT.uv = float2(1.f, 1.f);
	triStream.Append(OUT);

	triStream.RestartStrip();

	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[2], 1.f));
	OUT.uv = float2(0.f, 0.f);
	triStream.Append(OUT);
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[4], 1.f));
	OUT.uv = float2(1.f, 0.f);
	triStream.Append(OUT);
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[0], 1.f));
	OUT.uv = float2(0.f, 1.f);
	triStream.Append(OUT);
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[6], 1.f));
	OUT.uv = float2(1.f, 1.f);
	triStream.Append(OUT);
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[1], 1.f));
	OUT.uv = float2(0.f, 0.f);
	triStream.Append(OUT);
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[7], 1.f));
	OUT.uv = float2(1.f, 0.f);
	triStream.Append(OUT);
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[3], 1.f));
	OUT.uv = float2(0.f, 1.f);
	triStream.Append(OUT);
	OUT.position = mul(wvpMatrix, float4(p[0] + boxOffsets[5], 1.f));
	OUT.uv = float2(1.f, 1.f);
	triStream.Append(OUT);
}

float4 PS(GSO IN) : SV_TARGET
{
	return diffuseTex.Sample(samLinear, IN.uv);
}