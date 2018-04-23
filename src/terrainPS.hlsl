Texture2D diffuse: register(t0);

SamplerState SamplerType: register(s0);

struct PS_INPUT
{
  float4 posH : SV_POSITION;
  float3 nrm : NORMAL;
  float2 tex : TEXCOORD0;
};

float4 main( PS_INPUT input) : SV_TARGET
{
  return diffuse.Sample(SamplerType, input.tex);
}