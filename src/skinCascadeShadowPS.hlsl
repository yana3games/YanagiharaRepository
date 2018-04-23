Texture2D diffuse: register(t0);
SamplerState SamplerType: register(s0);

Texture2D shadowMap1: register(t1);
Texture2D shadowMap2: register(t2);
Texture2D shadowMap3: register(t3);
Texture2D shadowMap4: register(t4);

cbuffer SunMatrix : register(b0)
{
  float4x4 sunView;
  float4x4 sunProj[4];
  float4 divisionFar;
};

struct PS_INPUT
{
  float4 posH : SV_POSITION;
  float3 posW : TEXCOORD0;
  float3 nrm : NORMAL;
  float2 tex : TEXCOORD1;
  float depth : TEXCOORD2;
};


float4 main(PS_INPUT input) :SV_TARGET
{
  float shadow = 0.5f;
  float4 bias;		//バイアス値（調整が必要）
  bias.x = 0.002f;
  bias.y = 0.0015f;
  bias.z = 0.00015f;
  bias.w = 0.00005f;
  float4 color;
  color = diffuse.Sample(SamplerType, input.tex);
  float bright = 1.0f;
  if (input.depth < divisionFar.x)
  {
    //float thisDepth = mul(sunView, float4(input.posW, 1.0f)).z;
    float4x4 sunVP = mul(sunProj[0], sunView);
    float4 posSun = mul(sunVP, float4(input.posW, 1.0f));
    float thisDepth = posSun.z / posSun.w;
    float2 shadowTex;
    shadowTex.x = (1.0f + posSun.x / posSun.w) * 0.5f;
    shadowTex.y = (1.0f - posSun.y / posSun.w) * 0.5f;
    float smDepth = shadowMap1.Sample(SamplerType, shadowTex).r;

    if (thisDepth - smDepth > bias.x)
    {
      bright = shadow;
    }
  }
  else if (input.depth < divisionFar.y)
  {
    //float thisDepth = mul(sunView, float4(input.posW, 1.0f)).z;
    float4x4 sunVP = mul(sunProj[1], sunView);
    float4 posSun = mul(sunVP, float4(input.posW, 1.0f));
    float thisDepth = posSun.z / posSun.w;
    float2 shadowTex;
    shadowTex.x = (1.0f + posSun.x / posSun.w) * 0.5f;
    shadowTex.y = (1.0f - posSun.y / posSun.w) * 0.5f;
    float smDepth = shadowMap2.Sample(SamplerType, shadowTex).r;

    if (thisDepth - smDepth > bias.y)
    {
      bright = shadow;
    }
  }
  else if (input.depth < divisionFar.z)
  {
    //float thisDepth = mul(sunView, float4(input.posW, 1.0f)).z;
    float4x4 sunVP = mul(sunProj[2], sunView);
    float4 posSun = mul(sunVP, float4(input.posW, 1.0f));
    float thisDepth = posSun.z / posSun.w;
    float2 shadowTex;
    shadowTex.x = (1.0f + posSun.x / posSun.w) * 0.5f;
    shadowTex.y = (1.0f - posSun.y / posSun.w) * 0.5f;
    float smDepth = shadowMap3.Sample(SamplerType, shadowTex).r;

    if (thisDepth - smDepth > bias.z)
    {
      bright = shadow;
    }
  }
  else if (input.depth < divisionFar.w)
  {
    //float thisDepth = mul(sunView, float4(input.posW, 1.0f)).z;
    float4x4 sunVP = mul(sunProj[3], sunView);
    float4 posSun = mul(sunVP, float4(input.posW, 1.0f));
    float thisDepth = posSun.z / posSun.w;
    float2 shadowTex;
    shadowTex.x = (1.0f + posSun.x / posSun.w) * 0.5f;
    shadowTex.y = (1.0f - posSun.y / posSun.w) * 0.5f;
    float smDepth = shadowMap4.Sample(SamplerType, shadowTex).r;

    if (thisDepth - smDepth > bias.w)
    {
      bright = shadow;
    }
  }
  color.xyz *= bright;
  return color;
}