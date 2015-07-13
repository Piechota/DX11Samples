cbuffer ConstBuffer0 : register(b0)
{
	matrix wvpMatrix;
	float deltaTime;
}

struct GSO
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct Particle
{
	float3 velocity;
	float3 accelerate;
};

static const GSO GSOffsets[] =
{
	{ float4(-0.02f, -0.02f, 0.00f, 1.0f), float4(1.f, 0.f, 0.f, 1.f) },
	{ float4(-0.02f, +0.02f, 0.00f, 1.0f), float4(1.f, 0.f, 0.f, 1.f) },
	{ float4(+0.02f, -0.02f, 0.00f, 1.0f), float4(1.f, 0.f, 0.f, 1.f) },
	{ float4(+0.02f, +0.02f, 0.00f, 1.0f), float4(1.f, 0.f, 0.f, 1.f) }
};

RWStructuredBuffer<Particle> particleData : register(u0);
RWBuffer<float> particlePosition : register(u1);
[numthreads(16 * 16, 1, 1)]
void CS(int3 dispatchThreadID : SV_DISPATCHTHREADID)
{
	int positionOffset = dispatchThreadID.x * 3;
	particleData[dispatchThreadID.x].velocity += particleData[dispatchThreadID.x].accelerate * deltaTime;
	float3 velocity = particleData[dispatchThreadID.x].velocity * deltaTime;
	particlePosition[positionOffset + 0] += velocity.x;
	particlePosition[positionOffset + 1] += velocity.y;
	particlePosition[positionOffset + 2] += velocity.z;
}

[maxvertexcount(4)]
void GS(point float3 p[1] : POSITION, inout TriangleStream<GSO> triStream)
{
	GSO OUT;

	float4 color = length(p[0]) / 3.f;
	color.ba = float2(0.f, 1.f);
	color.r = 1.f - color.r;

	OUT = GSOffsets[0];
	OUT.position += float4(p[0], 0.f);
	OUT.position = mul(wvpMatrix, OUT.position);
	OUT.color = color;
	triStream.Append(OUT);
	OUT = GSOffsets[1];
	OUT.position += float4(p[0], 0.f);
	OUT.position = mul(wvpMatrix, OUT.position);
	OUT.color = color;
	triStream.Append(OUT);
	OUT = GSOffsets[2];
	OUT.position += float4(p[0], 0.f);
	OUT.position = mul(wvpMatrix, OUT.position);
	OUT.color = color;
	triStream.Append(OUT);
	OUT = GSOffsets[3];
	OUT.position += float4(p[0], 0.f);
	OUT.position = mul(wvpMatrix, OUT.position);
	OUT.color = color;
	triStream.Append(OUT);
}

float3 VS(float3 position : POSITION) : POSITION
{
	return position;
}

float4 PS(GSO IN) : SV_TARGET
{
	return IN.color;
}