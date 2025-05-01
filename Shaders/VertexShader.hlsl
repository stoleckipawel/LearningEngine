struct VSInput
{
    float3 pos : POSITION;
};

float4 main(in VSInput Input) : SV_Position
{
    return float4(Input.pos, 1.0);
}