cbuffer LightCbuf
{
    //点光源位置 目的是告诉每个像素光源的位置
    float3 lightPos;
};

static const float3 materialColor = { 0.7f, 0.7f, 0.9f }; //物体自身颜色
static const float3 ambient = { 0.15f, 0.15f, 0.15f }; //环境光
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f }; //慢反射光颜色
static const float diffuseIntensity = 1.0f;
//点光源衰减参数
static const float attConst = 1.0f;
static const float attLin = 1.0f;
static const float attQuad = 1.0f;

float4 main(float3 worldPos : Position, float3 n : Normal) : SV_TARGET
{
	// fragment to light vector data
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// diffuse attenuation
    const float att = attConst + attLin * distToL + attQuad * (distToL * distToL);
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	// final color
    return float4(saturate(diffuse + ambient), 1.0f);
}