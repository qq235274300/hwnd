cbuffer CBuf2
{
	float4 face_colors1[8] ;
}

float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
	return face_colors1[(tid / 2) % 8];
}