cbuffer CBuf
{
    matrix model;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    //SV_Position 是系统语义（强制要求）
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal)
{
    VSOut vso;
    // 顶点在世界空间中的坐标
    vso.worldPos = (float3) mul(float4(pos, 1.0f) ,model);
     // 顶点法线在世界空间中的方向  法线不能用 4×4 matrix（带位移），否则法线会被平移
    vso.normal = mul(n ,(float3x3)model);
    //modelViewProj = model * view * projection  转换到屏幕空间（viewport） mvp矩阵
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    
    return vso;
}