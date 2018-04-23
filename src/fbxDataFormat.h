#pragma once
//=============================================================================
// File : fbxDataFormat.h
// Date : 2018/02/20(火)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "Vector.h"
#include <vector>


namespace FBXStruct
{
  struct Vertex
  {
    Vector3 pos;
    Vector3 nrm;
    Vector2 tex;
    float weight[4];
    unsigned int boneIndex[4];
  };

  struct Matrix
  {
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
  };
}

class BornRefarence
{
 public:
  BornRefarence(unsigned char index, float weight) : index(index), weight(weight) {}
  unsigned char index;
  float weight;
};

class Point
{
 public:
  Point(const Vector3& positions) : positions(positions) {}
  Vector3 positions;
  std::vector<BornRefarence> bornRefarences;
};