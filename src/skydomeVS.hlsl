cbuffer Matrix : register(b0)
{
  float4x4 mtxWorld;
  float4x4 mtxView;
  float4x4 mtxProj;
};

struct VS_INPUT
{
  float3 pos : POSITION;
  float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
  float4 posH : SV_POSITION;
  float2 tex : TEXCOORD0;
};


VS_OUTPUT main(VS_INPUT input)
{
  VS_OUTPUT output;

  float4x4 mtxWVP = mul(mul(mtxProj, mtxView), mtxWorld);
  output.posH = mul(mtxWVP, float4(input.pos, 1.0f));

  output.tex = input.tex;

  return output;
}
