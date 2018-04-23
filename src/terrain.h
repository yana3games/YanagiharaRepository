#pragma once
//=============================================================================
// File : terrain.h
// Date : 2017/12/10(日)
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


// 地形クラス
class Terrain : public GameObject
{
 public:
  Terrain();
  Terrain(const Terrain&);
  ~Terrain();

  HRESULT Init(void);
  void Update(void);
  void Render(CCamera*);
  static Terrain* Create(float fWidth, float fHeight, int nBlockX, int nBlockY);

 private:
  HRESULT CreateVertexBuffer(void);
  HRESULT CreateIndexBuffer(void);

  void SetNormalAll( Vector3* normal);

  struct Vertex
  {
    Vector3 pos;
    Vector3 nrm;
    Vector2 tex;
  };
  ID3D11Buffer *m_vertexBuffer, *m_indexBuffer; // バッファー
  int m_vertexCnt, m_indexCnt; // バッファーの要素数
  Vector2 m_fieldSize;         // 全体の広さ
  Vector2 m_blockSize;         // 1ブロックの広さ
  Vector2 m_numBlock;          // ブロック数
  Vector3 m_LTPos;             // 最初の頂点の位置(左上
  Texture* m_texture;          // テクスチャ

};
