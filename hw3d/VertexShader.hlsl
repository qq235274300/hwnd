struct VSOut
{
	float4 color : Color;
	float4 pos : SV_Position;	
};

cbuffer CBuf
{
	 matrix transform1;
}

VSOut main(float2 pos : Position,float4 color : Color)
{
	VSOut vso;
	vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform1);
	vso.color = color;
	return vso;
}