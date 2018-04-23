#pragma once
//=============================================================================
// File : skydome.h
// Date : 2017/12/12(火)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Vector.h"
#include "gameObject.h"


///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class Texture;
class CCamera;


// スカイドームクラス
class Skydome : public GameObject
{
public:
  Skydome();
  Skydome(const Skydome&);
  ~Skydome();

  HRESULT Init(void);
  void Update(void);
  void Render(CCamera*);
  static Skydome* Create(float fRadius, float width, float height, const Vector3& pos);

private:
  HRESULT CreateVertexBuffer(void);
  HRESULT CreateIndexBuffer(void);
  struct Vertex
  {
    Vector3 pos;
    Vector2 tex;
  };

  ID3D11Buffer *m_vertexBuffer, *m_indexBuffer; // バッファー
  int m_vertexCnt, m_indexCnt; // バッファーの要素数

  float m_radius;              // 半径
  Vector2 m_numDivision;       // 分割数

  Texture* m_texture;          // テクスチャ
};