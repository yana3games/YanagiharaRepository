cbuffer Matrix : register(b0)
{
  float4x4 mtxWorld;
  float4x4 mtxView;
  float4x4 mtxProj;
  float4x4 mtxWIT;
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
  float3 posW : TEXCOORD0;
  float3 nrm : NORMAL;
  float2 tex : TEXCOORD1;
  float depth : TEXCOORD2;
};


VS_OUTPUT main(VS_INPUT input)
{
  VS_OUTPUT output;
  output.posW = mul(mtxWorld, float4(input.pos, 1.0f)).xyz;
  output.posH = mul(mtxView, float4(output.posW, 1.0f));

  output.depth = output.posH.z;

  output.posH = mul(mtxProj, output.posH);

  output.nrm = mul(mtxWIT, float4(input.nrm, 0.0f)).xyz;

  output.tex = input.tex;

  return output;
}