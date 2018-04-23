#pragma once
//=============================================================================
// File : regidDefaultShader.h
// Date : 2018/01/26(金)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "shader.h"

class RegidDefaultShader : public Shader
{
 public:
  RegidDefaultShader();
  ~RegidDefaultShader();
//  void SetParameters(ID3D11Buffer* boneCB, ID3D11Buffer* vertexBuffer, DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, Texture* texture);
//
//  struct Vertex
//  {
//    Vector3 pos;
//    Vector3 nrm;
//    Vector2 tex;
//    float weight[4];
//    unsigned int boneIndex[4];
//  };
//
//  struct Matrix
//  {
//    DirectX::XMMATRIX world;
//    DirectX::XMMATRIX view;
//    DirectX::XMMATRIX proj;
//    DirectX::XMMATRIX wit;
//  };
//
// private:
//  ID3D11Buffer*          m_matCB;    // コンスタントバッファ
};