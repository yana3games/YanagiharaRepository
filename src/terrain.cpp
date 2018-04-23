//=============================================================================
// File : terrain.h
// Date : 2017/12/10(日)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "appManager.h"
#include "terrain.h"
#include "renderer.h"
#include "textureManager.h"
#include "Texture.h"
#include "appUtility.h"
#include "shaderManager.h"
#include "shader.h"

#include "camera.h"


///////////////////////////////////////////////////////////////////////////////
// 地形作成
Terrain* Terrain::Create(float fWidth, float fHeight, int nBlockX, int nBlockY)
{
  // 地形新規作成
  Terrain *pRet = new Terrain;
  if (pRet == nullptr) return nullptr;
  
  // 1ブロックの大きさを出す
  pRet->m_fieldSize = Vector2(fWidth, fHeight);
  pRet->m_numBlock = Vector2((float)nBlockX, (float)nBlockY);
  pRet->m_LTPos = Vector3(-fWidth  * 0.5f, 0.0f, fHeight * 0.5f); // 初期の頂点位置
  pRet->m_blockSize = Vector2(fWidth / nBlockX, fHeight / nBlockY);

  pRet->Init();

  return pRet;
}


///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
Terrain::Terrain()
{
  m_vertexBuffer = nullptr;
  m_indexBuffer = nullptr;
  m_layer = D3D11Renderer::eLayerNormalObject;
  m_shaderType = ShaderManager::eShaderTypeTerrain;
  m_texture = nullptr;
}


///////////////////////////////////////////////////////////////////////////////
// コピーコンストラクタ
Terrain::Terrain(const Terrain&)
{
}


///////////////////////////////////////////////////////////////////////////////
// デストラクタ
Terrain::~Terrain()
{
  std::wstring filename = L"Big_pebbles_pxr128.tif";
  AppManager::GetTextureManager()->Release(filename);

  SAFE_RELEASE(m_indexBuffer);
  SAFE_RELEASE(m_vertexBuffer);
}


///////////////////////////////////////////////////////////////////////////////
// 初期化処理
HRESULT Terrain::Init()
{
  bool result;
  std::wstring filename = L"./data/TEXTURE/terrain/Big_pebbles_pxr128.tif";
  result = AppManager::GetTextureManager()->Load(filename);
  if (!result) { return E_FAIL; }
  m_texture = AppManager::GetTextureManager()->GetResource(filename);

  HRESULT hr;
  hr = CreateVertexBuffer();
  if (FAILED(hr)) return E_FAIL;
  hr = CreateIndexBuffer();
  if (FAILED(hr)) return E_FAIL;
//  ID3D11RasterizerState* hpRasterizerState = NULL;
//  D3D11_RASTERIZER_DESC hRasterizerDesc = {
//    D3D11_FILL_WIREFRAME,
//    D3D11_CULL_NONE,	//ポリゴンの裏表を無くす
//    FALSE,
//    0,
//    0.0f,
//    FALSE,
//    FALSE,
//    FALSE,
//    FALSE,
//    FALSE
//  };
//  if (FAILED(AppManager::GetRenderer()->GetDevice()->CreateRasterizerState(&hRasterizerDesc, &hpRasterizerState))) {
//    MessageBox(NULL, "RastrerizerCreate", "error!", MB_OK);
//  }
//  
//  //ラスタライザーをコンテキストに設定
//  AppManager::GetRenderer()->GetDeviceContext()->RSSetState(hpRasterizerState);

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// 更新処理
void Terrain::Update()
{
}


///////////////////////////////////////////////////////////////////////////////
// 描画処理
void Terrain::Render(CCamera* pCamera)
{
  // シェーダを設定して描画
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();

  // プリミティブの種類を設定
  pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // 入力アセンブラに頂点バッファを設定
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  pDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

  // 入力アセンブラにインデックスバッファを設定
  pDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

  // シェーダ情報のセット
  Shader* shader = AppManager::GetShaderManager()->GetShader(m_shaderType);
  shader->SetParameters(
    GetTransform()->GetWorldMatrix(),
    *pCamera->GetViewMtx(),
    *pCamera->GetProjMtx(),
    m_texture
  );

  // 描画
  pDeviceContext->DrawIndexed(m_indexCnt, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// 頂点バッファ作成
HRESULT Terrain::CreateVertexBuffer()
{
  HRESULT result;
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;
  // デバイスの取得
  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();

  // 頂点数の計算
  m_vertexCnt = ((int)m_numBlock.x + 1) * ((int)m_numBlock.y + 1);

  // 頂点の一時バッファ確保
  Vertex* vertices = new Vertex[ m_vertexCnt];

//  float *pHeightMap = (float *)&m_aHeightMap[0];
  Vector3* normal = new Vector3[ m_vertexCnt];
  SetNormalAll(normal);
  int i = 0;
  for (int nY = 0; nY < m_numBlock.y + 1; nY++)
  {
    for (int nX = 0; nX < m_numBlock.x + 1; nX++)
    {
      // 位置
      vertices[i].pos.x = m_LTPos.x + m_blockSize.x * nX;
      vertices[i].pos.y = 0;
      vertices[i].pos.z = m_LTPos.z - (m_blockSize.y * nY);

      // 法線
      vertices[i].nrm = normal[i];

      // テクスチャ座標
      vertices[i].tex = Vector2(1.0f * nX, 1.0f * nY);

      ++i;
    }
  }
  delete[] normal;

  // 頂点バッファの設定
  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = sizeof(Vertex)* m_vertexCnt;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  // サブリソース構造体に頂点データへのポインタを格納
  vertexData.pSysMem = vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  //頂点バッファ作成
  result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
  delete[] vertices;
  // 頂点確保を失敗
  if (FAILED(result)) {
    return E_FAIL;
  }
  return S_OK;
}


/////////////////////////////////////////////////////////////////////////////////
// インデックスバッファ作成
HRESULT Terrain::CreateIndexBuffer(void)
{
  HRESULT result;
  D3D11_BUFFER_DESC indexBufferDesc;
  D3D11_SUBRESOURCE_DATA indexData;

  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();

  // インデックス数を計算
  m_indexCnt = (((int)m_numBlock.x + 1) * 2 + 2) * ((int)m_numBlock.y)-2;

  int nNumUpper = 0;
  int nNumLower = (int)m_numBlock.x + 1;

  int i = 0;
  unsigned long* index = new unsigned long[m_indexCnt];
  for (int nY = 0; nY < m_numBlock.y; nY++)
  {
    for (int nX = 0; nX < m_numBlock.x + 1; nX++)
    {
      // 下点
      index[i] = nNumLower;
      ++i;
      nNumLower++;

      // 上点
      index[i] = nNumUpper;
      ++i;

      nNumUpper++;
    }

    if (nY < m_numBlock.y - 1)
    {
      nNumUpper--;    // ダミー点を打つための調整

      // ダミー点１
      index[i] = nNumUpper;
      ++i;
      nNumUpper++;

      // ダミー点２
      index[i] = nNumLower;
      ++i;
    }
  }
  //静的インデックスバッファの説明を設定します。 
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCnt;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;
  indexBufferDesc.StructureByteStride = 0;

  //サブリソース構造体にインデックスデータへのポインタを渡します。 
  indexData.pSysMem = index;
  indexData.SysMemPitch = 0;
  indexData.SysMemSlicePitch = 0;

  //インデックスバッファを作成します。
  result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
  if (FAILED(result)) {
    return E_FAIL;
  }
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//	法線セッツァー
void Terrain::SetNormalAll( Vector3* normal)
{
  // 初期位置を計算する
  Vector3 Pos[4] =
  {
    Vector3( -m_blockSize.x * m_numBlock.x * 0.5f,                 0.0f, -m_blockSize.y * m_numBlock.y * 0.5f ),
    Vector3( -m_blockSize.x * m_numBlock.x * 0.5f,                 0.0f, -m_blockSize.y * m_numBlock.y * 0.5f + m_blockSize.y ),
    Vector3( -m_blockSize.x * m_numBlock.x * 0.5f + m_blockSize.x, 0.0f, -m_blockSize.y * m_numBlock.y * 0.5f ),
    Vector3(  m_blockSize.x * m_numBlock.x * 0.5f + m_blockSize.x, 0.0f,  m_blockSize.y * m_numBlock.y * 0.5f + m_blockSize.y )
  };

  // 頂点に入れる法線を初期化
  {
    int i = 0;

    while( i < m_vertexCnt )
    {
      normal[i++] = Vector3( 0.0f, 0.0f, 0.0f );
    }
  }

  // 面法線の計算と頂点に入れる法線を計算する
  // ただし、単位化は後で行う
  {
    int i = 0;  // 面

    for (int y = 0; y < m_numBlock.y; ++y)
    {
      for (int x = 0; x < m_numBlock.x; ++x)
      {
        // ポリゴンその一
        {
          Vector3 nor;

          // 左下の頂点からのベクトル1,2
          Vector3 v3Vec1 = Pos[1] - Pos[0];
          Vector3 v3Vec2 = Pos[2] - Pos[1];

          // 外積から法線を求める
          Vector3::Cross(&nor, &v3Vec1, &v3Vec2);
          Vector3::Normalize(&nor, &nor);

          // 対象となる頂点に入れる
          normal[x + 0 + ((int)m_numBlock.x + 1)*(y + 1)] += nor;
          normal[x + 0 + ((int)m_numBlock.x + 1)*(y + 0)] += nor;
          normal[x + 1 + ((int)m_numBlock.x + 1)*(y + 1)] += nor;

          // 次の法線へ
          ++i;
        }

        // ポリゴンその２
        {
          Vector3 nor;

          // 左上の頂点からのベクトル1,2
          Vector3 v3Vec1 = Pos[3] - Pos[1];
          Vector3 v3Vec2 = Pos[2] - Pos[3];

          // 外積から法線を求める
          Vector3::Cross(&nor, &v3Vec1, &v3Vec2);
          Vector3::Normalize(&nor, &nor);

          // 対象となる頂点に入れる
          normal[x + 0 + ((int)m_numBlock.x + 1)*(y + 0)] += nor;
          normal[x + 1 + ((int)m_numBlock.x + 1)*(y + 0)] += nor;
          normal[x + 1 + ((int)m_numBlock.x + 1)*(y + 1)] += nor;
          // 次の法線へ
          ++i;
        }

        // 次のブロックへの初期化
        Pos[0].y = 0;//m_heightMap[(y + 1)*(m_numBlock.x + 1) + x];
        Pos[1].y = 0;//m_heightMap[y*(m_numBlock.x + 1) + x];
        Pos[2].y = 0;//m_heightMap[(y + 1)*(m_numBlock.x + 1) + x + 1];
        Pos[3].y = 0;//m_heightMap[y*(m_numBlock.x + 1) + x + 1];
      }
    }
  }

  for (int i = 0; i < m_vertexCnt; ++i) {
    Vector3::Normalize(normal, normal);
  }
}

/////////////////////////////////////////////////////////////////////////////////
////	インクルード
//#include "meshfield.h"
//#include <stdlib.h>
//#include <time.h>
//#include "manager.h"
//#include "renderer.h"
//#include "scene.h"
//#include "textureManager.h"
//#include "math.h"
//
/////////////////////////////////////////////////////////////////////////////////
////	定数
//const float CMeshField::HEIGHTMAP[51*51] =
//{
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  17.0f,  15.0f,   7.0f,   6.5f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  17.0f,  17.0f,  15.0f,  10.0f,   7.5f,   7.0f,   6.5f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  17.0f,  15.0f,  15.0f,  10.0f,   8.5f,   8.0f,   7.5f,   7.0f,   0.5f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  17.0f,  15.0f,   9.0f,   8.5f,   8.0f,   7.5f,   7.0f,   6.5f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  17.0f,  15.0f,   9.0f,   8.5f,   8.0f,   7.5f,   7.0f,   6.5f,   6.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  17.0f,  15.0f,   8.0f,   8.0f,   7.5f,   7.0f,   6.5f,   7.0f,   7.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  15.0f,   7.5f,   8.0f,   7.5f,   7.0f,   6.0f,   8.0f,   8.0f,   8.0f,   0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   0.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   6.0f,   7.0f,   7.5f,   7.0f,   6.5f,   5.0f,   7.0f,   9.0f,   9.0f,   0.0f,   0.0f,   0.0f,   2.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//
//  20.0f,  20.0f,   0.0f,   6.5f,   7.0f,   6.5f,   2.0f,   4.0f,   6.0f,   8.0f,  10.0f,   0.0f,   0.0f,   0.0f,   3.0f,   1.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   6.5f,   0.0f,   1.0f,   3.0f,   5.0f,   7.0f,   9.0f,   9.0f,   0.0f,   0.0f,   4.0f,   2.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   4.0f,   6.0f,   8.0f,   8.0f,   8.0f,   7.0f,   5.0f,   3.0f,   1.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   3.0f,   5.0f,   7.0f,   7.0f,   7.0f,   6.0f,   6.0f,   4.0f,   2.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   4.0f,   6.0f,   6.0f,   6.0f,   5.0f,   5.0f,   5.0f,   3.0f,   1.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   3.0f,   5.0f,   5.0f,   5.0f,   4.0f,   4.0f,   4.0f,   4.0f,   2.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   4.0f,   4.0f,   4.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,  10.0f,   8.0f,   6.0f,   5.0f,   4.0f,   3.0f,   3.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   3.0f,   3.0f,   3.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,  10.0f,   8.0f,   6.0f,   5.0f,   4.0f,   3.0f,   3.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   2.0f,   2.0f,   1.0f,   1.0f,   1.0f,   1.0f,   1.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,  10.0f,   8.0f,   6.0f,   5.0f,   4.0f,   3.0f,   3.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,  10.0f,   8.0f,   6.0f,   5.0f,   4.0f,   3.0f,   3.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   3.0f,   3.0f,   3.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   3.0f,   3.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   4.0f,   4.0f,   4.0f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   4.0f,   4.0f,   4.0f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   4.0f,   4.0f,   4.0f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   2.0f,   0.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   4.0f,   4.0f,   4.0f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   2.0f,   2.0f,   2.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.5f,   5.5f,   3.5f,   3.5f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   5.0f,   3.0f,   3.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   2.0f,   2.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   6.0f,   7.0f,   8.0f,   5.0f,   2.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   2.0f,   2.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   6.0f,   6.0f,   7.0f,   5.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   3.0f,   4.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   6.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   3.0f,   4.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//
//  20.0f,  20.0f,   0.0f,   0.0f,   5.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   3.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   1.0f,   1.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,
//};
//
//float CMeshField::m_aHeightMap[51*51] = {0.0f};
//
//
//
/////////////////////////////////////////////////////////////////////////////////
////	コンストラクタ
//CMeshField::CMeshField() : m_pVtxBuff(NULL), m_nMaxVtx(0), m_pIdxBuff(NULL), m_nMaxIdx(0), m_pTex(NULL), m_pNormalMap(NULL), m_pNormalVtxBuff(NULL)
//{
//  // マテリアル
//  m_Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//  m_Material.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.5f);
//  m_Material.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
//  m_Material.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
//  m_Material.Power = 0.0f;
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
////	メッシュフィールド作成
//CMeshField *CMeshField::Create(const std::string& pFile, float fWidth, float fHeight, int nBlockX, int nBlockY)
//{
//  for (unsigned i = 0; i < 51 * 51; ++i)
//  {
//    m_aHeightMap[i] = HEIGHTMAP[i];
//    m_aHeightMap[i] *= 5.9f;
//  }
//
//  // メッシュフィールド新規作成
//  CMeshField *pRet = new CMeshField;
//  if (pRet == NULL)
//  {
//    return NULL;
//  }
//
//  // 1ブロックの大きさを出す
//  pRet->m_nBlockX = nBlockX;
//  pRet->m_nBlockY = nBlockY;
//  pRet->m_v3Pos = DirectX::XMVectorSet(-fWidth  * 0.5f, 0.0f, fHeight * 0.5f, 0.0f); // 初期の頂点位置
//  pRet->m_v2Size = DirectX::XMVectorSet(fWidth / nBlockX, fHeight / nBlockY, 0.0f);
//
//  pRet->Init();
//
//  // テクスチャの読み込み
//  if (pFile.empty())
//  {
//    pRet->m_pTex = NULL;
//  }
//  else {
//    CManager::GetTextureManager()->Load(pFile);
//    pRet->m_pTex = CManager::GetTextureManager()->GetResource(pFile);
//  }
//  pRet->SetObjType(OBJTYPE_FIELD);
//
//  return pRet;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	初期化処理
//HRESULT CMeshField::Init(void)
//{
//	// リザルト
//	HRESULT hr;
//
//	// デバイスの取得
//	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
//	// 頂点設定
//	{
//		// 頂点数の計算
//		m_nMaxVtx = (m_nBlockX + 1) * (m_nBlockY + 1);
//
//		// バッファの生成(確保)
//		hr = pDevice->CreateVertexBuffer(
//			sizeof(VERTEX_3D) * m_nMaxVtx, // バッファサイズ
//			D3DUSAGE_WRITEONLY,                // 頂点バッファの使用方法
//			FVF_VERTEX_3D,                     // 使用する頂点フォーマット
//			D3DPOOL_MANAGED,                   // バッファを保持するメモリクエストを指定
//			&m_pVtxBuff,                   // 頂点のバッファの先頭アドレス
//			NULL);
//
//		// 頂点確保を失敗
//		if (FAILED(hr))
//		{
//			Uninit();
//			return E_FAIL;
//		}
//
//		// 頂点設定
//		
//		VERTEX_3D *pVtx;
//		float *pHeightMap = (float *)&m_aHeightMap[0];
//
//		hr = m_pVtxBuff->Lock(0, 0, (void**)&pVtx, D3DLOCK_NOSYSLOCK);
//		for (int nY = 0; nY < m_nBlockY + 1; nY++)
//		{
//			for (int nX = 0; nX < m_nBlockX + 1; nX++)
//			{
//				// 位置
//				pVtx->pos.x = m_v3Pos.x + m_v2Size.x * nX;
//				pVtx->pos.y = *pHeightMap;
//				pVtx->pos.z = m_v3Pos.z - (m_v2Size.y * nY);
//
//				// テクスチャ座標
//				pVtx->tex = DirectX::XMVectorSet(1.0f * nX, 1.0f * nY, 0.0f, 0.0f);
//
//				// 頂点カラー
//				pVtx->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//
//				// 次の頂点へ
//				++pHeightMap;
//				++pVtx;
//			}
//		}
//
//		hr = m_pVtxBuff->Unlock(); // 頂点バッファの固定を解除
//		// 法線マップの作成
//		{
//			// 領域確保
//			m_pNormalMap = new D3DXVECTOR3[m_nBlockY*m_nBlockX * 2];
//			m_pNormalVtxBuff = new D3DXVECTOR3[m_nMaxVtx];
//
//			// 計算
//			SetNormalAll();
//		}
//	}
//
//	// インデックスを指定
//	{
//		// インデックス数を計算
//		m_nMaxIdx = ((m_nBlockX + 1) * 2 + 2) * (m_nBlockY)-2;
//
//		// 生成(確保)
//		hr = pDevice->CreateIndexBuffer(
//			sizeof(WORD) * m_nMaxIdx,       // バッファサイズ
//			D3DUSAGE_WRITEONLY,             // 頂点バッファの使用方法
//			D3DFMT_INDEX16,                 // 使用する頂点フォーマット
//			D3DPOOL_MANAGED,                // バッファを保持するメモリクエストを指定
//			&m_pIdxBuff,                    // 頂点インデックスのバッファの先頭アドレス
//			NULL);
//
//		if (FAILED(hr))
//		{
//			Uninit();
//			return E_FAIL;
//		}
//
//		int nNumUpper = 0;
//		int nNumLower = m_nBlockX + 1;
//		WORD *pIndx;
//
//		hr = m_pIdxBuff->Lock(0, 0, (void **)&pIndx, D3DLOCK_NOSYSLOCK);
//		for (int nY = 0; nY < m_nBlockY; nY++)
//		{
//			for (int nX = 0; nX < m_nBlockX + 1; nX++)
//			{
//				// 下点
//				*pIndx = nNumLower;
//				++pIndx;
//
//				nNumLower++;
//
//				// 上点
//				*pIndx = nNumUpper;
//				++pIndx;
//
//				nNumUpper++;
//			}
//
//			if (nY < m_nBlockY - 1)
//			{
//				nNumUpper--;    // ダミー点を打つための調整
//
//				// ダミー点１
//				*pIndx = nNumUpper;
//				++pIndx;
//				nNumUpper++;
//
//				// ダミー点２
//				*pIndx = nNumLower;
//				++pIndx;
//			}
//		}
//		hr = m_pIdxBuff->Unlock(); // インデックスバッファの固定を解除
//	}
//	return S_OK;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	終了処理
//void CMeshField::Uninit( void )
//{
//	if( m_pVtxBuff )
//	{
//		m_pVtxBuff->Release();
//		m_pVtxBuff = NULL;
//	}
//
//	if( m_pIdxBuff )
//	{
//		m_pIdxBuff->Release();
//		m_pIdxBuff = NULL;
//	}
//
//	if( m_pNormalMap )
//	{
//		delete[] m_pNormalMap;
//		m_pNormalMap = NULL;
//	}
//
//	if( m_pNormalVtxBuff )
//	{
//		delete m_pNormalVtxBuff;
//		m_pNormalVtxBuff = NULL;
//	}
//	CScene::Uninit();
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	更新処理
//void CMeshField::Update( void )
//{
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	描画処理
//void CMeshField::Draw( void )
//{
//	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
//
//	///////////////////////////////////////////////////////////////////////////
//	//	ライトの作成	（4つまで
//	D3DLIGHT9 light;
//	//中身の初期化
//	ZeroMemory(&light, sizeof(D3DLIGHT9));
//	//ライトの種類の設定
//	light.Type = D3DLIGHT_DIRECTIONAL;
//	//拡散光の設定
//	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//	//アンビエント光の設定
//	light.Ambient = D3DXCOLOR(0.3f, 0.2f, 0.4f, 1.0f);
//	//ライトの方向
//	D3DXVECTOR3 vecDir(0.0f, -1.0f, 1.0f);
//	//単位ベクトルへ変換
//	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
//	//デバイスにライトの設定 ( ライトのスロットIndex, 格納するライト)
//	pDevice->SetLight(0, &light);
//	//ライトを有効にする	( ライトのスロットIndex, TRUE.FALSE)
//	pDevice->LightEnable(0, TRUE);
//	//統合ライトを有効に
//	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
//
//	HRESULT hr;
//	// 頂点バッファをデータストリームにバインド
//	pDevice->SetStreamSource( 0 , m_pVtxBuff, 0, sizeof( VERTEX_3D ) );
//	// 頂点フォーマットの設定
//	pDevice->SetFVF(FVF_VERTEX_3D);
//	// インデックスバッファをデータストリームにバインド
//	pDevice->SetIndices( m_pIdxBuff );
//	// テクスチャの設定
//	pDevice->SetTexture( 0, m_pTex );
//
//	// 行列のセット
//	D3DXMATRIX mtxWorld;
//	D3DXMatrixIdentity(&mtxWorld);
//	hr = pDevice->SetTransform( D3DTS_WORLD, &mtxWorld );
//
//	// マテリアルのセット
//	hr = pDevice->SetMaterial( &m_Material );
//
//	// 描画
//	hr = pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, m_nMaxVtx, 0, m_nMaxIdx - 2 );
//
//	#ifdef _DEBUG
//	#if 0
//	// ワイヤーフレーム表示
//	{
//		D3DMATERIAL9 Normal;
//		Normal.Diffuse = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
//		Normal.Ambient = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
//		Normal.Emissive = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
//		Normal.Specular = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
//		Normal.Power    = 1.0f;
//		pDevice->SetMaterial( &Normal );
//		DWORD nBeforeFillMode;
//		pDevice->GetRenderState( D3DRS_FILLMODE, &nBeforeFillMode );
//		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
//		pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, m_nMaxVtx, 0, m_nMaxIdx - 2 );
//		pDevice->SetRenderState( D3DRS_FILLMODE, nBeforeFillMode );
//	}
//	#endif
//	#endif
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	高さ取得
//float CMeshField::GetHeight( const float x, const float z ) const
//{
//	// フィールドのオフセット値
//	const D3DXVECTOR2 offset = D3DXVECTOR2( m_v2Size.x * m_nBlockX * 0.5f, m_v2Size.y * m_nBlockY * 0.5f );
//	if( x < -offset.x || offset.x < x || z < -offset.y || offset.y < z )
//	{
//		return 0.0f;
//	}
//
//	// フィールドの左端を原点とした頂点sをつくる
//	const D3DXVECTOR2 s = D3DXVECTOR2( offset.x + x, offset.y + z );
//
//	// 対象となるブロックを計算する
//	int nBlockX = (int)( s.x / m_v2Size.x );
//	int nBlockY = (int)( s.y / m_v2Size.y );
//
//	// 対象となるブロックの頂点をpとする
//	const D3DXVECTOR3 p[4] =
//	{
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y ),
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y),
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//	};
//	
//	// p0からのベクトルaを得る
//	D3DXVECTOR2 a = D3DXVECTOR2( x - p[1].x, z - p[1].z );
//	D3DXVec2Normalize( &a, &a );
//	if(a.x < 0.0f) a.x = -a.x;
//	if(a.y < 0.0f) a.y = -a.y;
//	
//	D3DXVECTOR3 nor;
//	if( a.x > a.y )
//	{
//		const D3DXVECTOR3 v[2] = { p[1] - p[2], p[3] - p[2] };
//		D3DXVec3Cross( &nor, &v[0], &v[1] );
//		D3DXVec3Normalize( &nor, &nor );
//	}
//	else
//	{
//		const D3DXVECTOR3 v[2] = { p[1] - p[0], p[2] - p[1] };
//		D3DXVec3Cross( &nor, &v[0], &v[1] );
//		D3DXVec3Normalize( &nor, &nor );
//	}
//	return p[1].y + ( nor.x * (  x - p[1].x ) + nor.z * ( z - p[1].z )) / -nor.y;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	現在位置の傾きゲッツァー
//D3DXQUATERNION CMeshField::GetQuaternion( const float x, const float z ) const
//{
//	// フィールドのオフセット値
//	const D3DXVECTOR2 offset = D3DXVECTOR2( m_v2Size.x * m_nBlockX * 0.5f, m_v2Size.y * m_nBlockY * 0.5f );
//	if( x < -offset.x || offset.x < x || z < -offset.y || offset.y < z )
//	{
//		D3DXQUATERNION quat;
//		D3DXQuaternionIdentity(&quat);
//		return quat;
//	}
//
//	// フィールドの左端を原点とした頂点sをつくる
//	const D3DXVECTOR2 s = D3DXVECTOR2( offset.x + x, offset.y + z );
//
//	// 対象となるブロックを計算する
//	int nBlockX = (int)( s.x / m_v2Size.x );
//	int nBlockY = (int)( s.y / m_v2Size.y );
//
//	// 対象となるブロックの頂点をpとする
//	const D3DXVECTOR3 p[4] =
//	{
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y ),
//
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y),
//
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//	};
//
//	// p0からのベクトルaを得る
//	D3DXVECTOR2 a = D3DXVECTOR2( x - p[1].x, z - p[1].z );
//	D3DXVec2Normalize( &a, &a );
//	if(a.x < 0.0f) a.x = -a.x;
//	if(a.y < 0.0f) a.y = -a.y;
//
//	// 法線を求める
//	D3DXVECTOR3 nor;
//
//	if( a.x > a.y )
//	{
//		const D3DXVECTOR3 v[2] = { p[1] - p[2], p[3] - p[2] };
//		D3DXVec3Cross( &nor, &v[0], &v[1] );
//		D3DXVec3Normalize( &nor, &nor );
//	}
//
//	else
//	{
//		const D3DXVECTOR3 v[2] = { p[1] - p[0], p[2] - p[1] };
//		D3DXVec3Cross( &nor, &v[0], &v[1] );
//		D3DXVec3Normalize( &nor, &nor );
//	}
//
//	// 回転軸を求める
//	D3DXVECTOR3 axis;
//	const D3DXVECTOR3 def(0.0f,1.0f,0.0f);
//	D3DXVec3Cross(&axis,&def,&nor);
//
//	// 回転角を求める
//	const float angle = acosf(D3DXVec3Dot(&def,&nor));
//
//	// クオータニオンを作成
//	D3DXQUATERNION quat;
//	D3DXQuaternionRotationAxis(&quat,&axis,angle);
//
//	return quat;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	法線セッツァー
//void CMeshField::SetNormalAll( void )
//{
//	// 初期位置を計算する
//	D3DXVECTOR3 Pos[4] =
//	{
//		D3DXVECTOR3( -m_v2Size.x * m_nBlockX * 0.5f, m_aHeightMap[m_nBlockX + 1], -m_v2Size.y * m_nBlockY * 0.5f ),
//		D3DXVECTOR3( -m_v2Size.x * m_nBlockX * 0.5f, m_aHeightMap[0],             -m_v2Size.y * m_nBlockY * 0.5f + m_v2Size.y ),
//		D3DXVECTOR3( -m_v2Size.x * m_nBlockX * 0.5f + m_v2Size.x, m_aHeightMap[m_nBlockX + 2], -m_v2Size.y * m_nBlockY * 0.5f ),
//		D3DXVECTOR3(  m_v2Size.x * m_nBlockX * 0.5f + m_v2Size.x, m_aHeightMap[1],              m_v2Size.y * m_nBlockY * 0.5f + m_v2Size.y )
//	};
//	
//	// 頂点に入れる法線を初期化
//	{
//		int i = 0;
//
//		while( i < m_nMaxVtx )
//		{
//			m_pNormalVtxBuff[i++] = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//		}
//	}
//	
//	// 面法線の計算と頂点に入れる法線を計算する
//	// ただし、単位化は後で行う
//	{
//		int i = 0;  // 面
//
//		for( int y = 0 ; y < m_nBlockY ; ++y )
//		{
//			for( int x = 0 ; x < m_nBlockX ; ++x )
//			{
//				// ポリゴンその一
//				{
//					D3DXVECTOR3 nor;
//
//					// 左下の頂点からのベクトル1,2
//					D3DXVECTOR3 v3Vec1 = Pos[1] - Pos[0];
//					D3DXVECTOR3 v3Vec2 = Pos[2] - Pos[1];
//
//					// 外積から法線を求める
//					D3DXVec3Cross( &nor, &v3Vec1, &v3Vec2 );
//					D3DXVec3Normalize( &m_pNormalMap[i], &nor );
//
//					// 対象となる頂点に入れる
//					m_pNormalVtxBuff[x+0+(m_nBlockX+1)*(y+1)] += m_pNormalMap[i];
//					m_pNormalVtxBuff[x+0+(m_nBlockX+1)*(y+0)] += m_pNormalMap[i];
//					m_pNormalVtxBuff[x+1+(m_nBlockX+1)*(y+1)] += m_pNormalMap[i];
//
//					// 次の法線へ
//					++i;
//				}
//
//				// ポリゴンその２
//				{
//					D3DXVECTOR3 nor;
//
//					// 左上の頂点からのベクトル1,2
//					D3DXVECTOR3 v3Vec1 = Pos[3] - Pos[1];
//					D3DXVECTOR3 v3Vec2 = Pos[2] - Pos[3];
//
//					// 外積から法線を求める
//					D3DXVec3Cross( &nor, &v3Vec1, &v3Vec2 );
//					D3DXVec3Normalize( &m_pNormalMap[i], &nor );
//
//					// 対象となる頂点に入れる
//					m_pNormalVtxBuff[x+0+(m_nBlockX+1)*(y+0)] += m_pNormalMap[i];
//					m_pNormalVtxBuff[x+1+(m_nBlockX+1)*(y+0)] += m_pNormalMap[i];
//					m_pNormalVtxBuff[x+1+(m_nBlockX+1)*(y+1)] += m_pNormalMap[i];
//					// 次の法線へ
//					++i;
//				}
//
//				// 次のブロックへの初期化
//				Pos[0].y = m_aHeightMap[(y+1)*(m_nBlockX+1)+x];
//				Pos[1].y = m_aHeightMap[y*(m_nBlockX+1)+x];
//				Pos[2].y = m_aHeightMap[(y+1)*(m_nBlockX+1)+x+1];
//				Pos[3].y = m_aHeightMap[y*(m_nBlockX+1)+x+1];
//			}
//		}
//	}
//
//	// 頂点に法線を入れる
//	{
//		HRESULT hr;
//		VERTEX_3D *pVtx;
//		hr = m_pVtxBuff->Lock( 0, 0, (void**)&pVtx, D3DLOCK_NOSYSLOCK );
//		for( int i = 0 ; i < m_nMaxVtx ; ++i )
//		{
//			D3DXVec3Normalize( &pVtx[i].normal, &m_pNormalVtxBuff[i] );
//		}
//		hr = m_pVtxBuff->Unlock(); // 頂点バッファの固定を解除
//	}
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
////	当たり判定
//bool CMeshField::IsHit( const D3DXVECTOR3 &start, const D3DXVECTOR3 &end ) const
//{
//	return HitField(start,end,1.0f);
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
////	現在足場との当たり判定
//bool CMeshField::HitField( const D3DXVECTOR3 &start, const D3DXVECTOR3 &end, float par ) const
//{
//	bool re;
//
//	// 大雑把にブロックを特定
//	float x = start.x + ( end.x - start.x ) * par;
//	float z = start.z + ( end.z - start.z ) * par;
//	
//	// フィールドのオフセット値
//	const D3DXVECTOR2 offset = D3DXVECTOR2( m_v2Size.x * m_nBlockX * 0.5f, m_v2Size.y * m_nBlockY * 0.5f );
//	if( x < -offset.x || offset.x < x || z < -offset.y || offset.y < z )
//	{
//		re = false;
//	}
//	
//	// フィールドの左端を原点とした頂点sをつくる
//	const D3DXVECTOR2 s = D3DXVECTOR2( offset.x + x, offset.y + z );
//	
//	// 対象となるブロックを計算する
//	int nBlockX = (int)( s.x / m_v2Size.x );
//	int nBlockY = (int)( s.y / m_v2Size.y );
//	
//	// 対象となるブロックの頂点をpとする
//	const D3DXVECTOR3 p[4] =
//	{
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y ),
//	
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//	
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y),
//	
//		D3DXVECTOR3( (nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//	};
//	
//	// p0からのベクトルaを得る
//	const D3DXVECTOR2 a = D3DXVECTOR2( x - p[1].x, z - p[1].z );
//	D3DXVECTOR2 a_nor;
//	D3DXVec2Normalize( &a_nor, &a );
//	
//	// あたり判定
//	if( a_nor.x < a_nor.y )
//	{
//		re = StepOverFrom3Vertex(start,end,&p[1],&p[3],&p[2]);
//	}
//	else
//	{
//		re = StepOverFrom3Vertex(start,end,&p[0],&p[1],&p[2]);
//	}	
//	if(re) return re;
//	
//	// 当たらなければ次へ
//	static const float fineness = 1.0f / 6.0f;
//	par -= fineness;
//	if( par < 0.0f ) return false;
//
//	return HitField(start,end,par);
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
////	色設定
//void CMeshField::SetDiffuse( const D3DXCOLOR &_set ){
//	m_Material.Diffuse = _set;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	平面を跨いで居るか？
//bool CMeshField::StepOverFrom3Vertex(const D3DXVECTOR3& start, const D3DXVECTOR3& end, const D3DXVECTOR3 *pVtx1, const D3DXVECTOR3 *pVtx2, const D3DXVECTOR3 *pVtx3) const
//{
//	D3DXVECTOR3 vec12, vec13, normal;
//	vec12 = *pVtx2 - *pVtx1;
//	vec13 = *pVtx3 - *pVtx1;
//	D3DXVec3Cross(&normal, &vec12, &vec13);
//	D3DXVec3Normalize(&normal, &normal);
//	float a = normal.x;
//	float b = normal.y;
//	float c = normal.z;
//	float d = (normal.x*(-pVtx1->x)) + (normal.y*(-pVtx1->y)) + (normal.z*(-pVtx1->z));
//
//	float startDot = a*start.x + b*start.y + c*start.z + d;
//	float endDot = a*end.x + b*end.y + c*end.z + d;
//
//	if (startDot * endDot <= 1) return true;
//	return false;
//}