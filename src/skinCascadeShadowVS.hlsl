cbuffer Matrix : register(b0)
{
  float4x4 mtxWorld;
  float4x4 mtxView;
  float4x4 mtxProj;
  float4x4 mtxWIT;
};

cbuffer Bone : register(b1)
{
  float4x4 mtxBone[512];
};

struct VS_INPUT
{
  float3 pos : POSITION;
  float3 nrm : NORMAL;
  float2 tex : TEXCOORD0;

  float4 weight : WEIGHT;
  uint4 boneIndex : INDEX;
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
  // 各ボーンの合成
  float4x4 bone = (matrix)0;
  float w[4] = (float[4])input.weight;
  uint index[4] = (uint[4])input.boneIndex;
  for (int i = 0; i < 3; i++) {
    bone += mtxBone[index[i]] * w[i];
  }
  bone += mtxBone[index[3]] * (1.0f - w[0] - w[1] - w[2]);

  output.posW = mul(bone, float4(input.pos, 1.0f)).xyz;
  output.posW = mul(mtxWorld, float4(output.posW, 1.0f)).xyz;
  output.posH = mul(mtxView, float4(output.posW, 1.0f));

  output.depth = output.posH.z;

  output.posH = mul(mtxProj, output.posH);

  output.nrm = mul(mtxWIT, float4(input.nrm, 0.0f)).xyz;
  output.nrm = normalize(output.nrm);

  output.tex = input.tex;

  return output;
}