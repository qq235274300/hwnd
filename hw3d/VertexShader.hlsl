struct VSOut
{
	float4 pos : SV_Position;	
};

cbuffer CBuf
{
	 matrix transform1;
}

VSOut main(float3 pos : Position)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), transform1);
	return vso;
}