cbuffer Matrix : register(b0)
{
  float4x4 mtxWorld;
  float4x4 mtxView;
  float4x4 mtxProj;
};

struct VS_INPUT
{
  float3 pos : POSITION;
  float3 nrm : NORMAL;
  float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
  float4 posH : SV_POSITION;
};


VS_OUTPUT main(VS_INPUT input)
{
  VS_OUTPUT output;
  output.posH = mul(mtxWorld, float4(input.pos, 1.0f));
  output.posH = mul(mtxView, output.posH);
  output.posH = mul(mtxProj, output.posH);

  return output;
}