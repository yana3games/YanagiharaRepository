struct PS_INPUT
{
  float4 posH : SV_POSITION;
  float depth : TEXCOORD0;
};


float4 main(PS_INPUT input) :SV_TARGET
{
  return input.posH.z / input.posH.w;
}
