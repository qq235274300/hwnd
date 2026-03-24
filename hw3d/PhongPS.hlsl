cbuffer LightCbuf
{
    //点光源位置 目的是告诉每个像素光源的位置
    float3 lightPos;
    float3 ambient; //环境光
    float3 diffuseColor; //慢反射光颜色
    float diffuseIntensity;
    //点光源衰减参数
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCbuf
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};

Texture2D tex;
SamplerState splr;

float4 main(float3 worldPos : Position, float3 n : Normal,float2 tc : Texcoord) : SV_TARGET
{
    // fragment to light vector data
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    // attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    // diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    // reflected light vector
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    // calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
    // final color
   // return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}