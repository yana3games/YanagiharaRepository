//=============================================================================
// File : skydome.cpp
// Date : 2017/12/12(火)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "appManager.h"
#include "skydome.h"
#include "renderer.h"
#include "textureManager.h"
#include "Texture.h"
#include "appUtility.h"
#include "shaderManager.h"
#include "shader.h"
#include "camera.h"
#include "cameraMan.h"


///////////////////////////////////////////////////////////////////////////////
// 地形作成
Skydome* Skydome::Create(float fRadius, float width, float height, const Vector3& pos)
{
  // 地形新規作成
  Skydome *pRet = new Skydome;
  if (pRet == nullptr) return nullptr;

  // 1ブロックの大きさを出す
  pRet->m_radius = fRadius;
  pRet->m_numDivision = Vector2(width, height);
  pRet->m_vertexCnt = ((width + 1) * (height - 1) + 2);
  pRet->m_indexCnt = ((2 + 2 * width)*(height - 2) + (height - 3) * 2);
  pRet->GetTransform()->SetPosition( pos);
  pRet->Init();

  return pRet;
}

///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
Skydome::Skydome()
{
  m_vertexBuffer = nullptr;
  m_indexBuffer = nullptr;
  m_layer = D3D11Renderer::eLayerSky;
  m_shaderType = ShaderManager::eShaderTypeSky;
  m_texture = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// コピーコンストラクタ
Skydome::Skydome(const Skydome&)
{
}


///////////////////////////////////////////////////////////////////////////////
// デストラクタ
Skydome::~Skydome()
{
  std::wstring filename = L"./data/TEXTURE/skydome/skydome.jpg";
  AppManager::GetTextureManager()->Release(filename);

  SAFE_RELEASE(m_indexBuffer);
  SAFE_RELEASE(m_vertexBuffer);
}


///////////////////////////////////////////////////////////////////////////////
// 初期化処理
HRESULT Skydome::Init()
{
  bool result;
  std::wstring filename = L"./data/TEXTURE/skydome/skydome.jpg";
  result = AppManager::GetTextureManager()->Load(filename);
  if (!result) { return E_FAIL; }
  m_texture = AppManager::GetTextureManager()->GetResource(filename);

  HRESULT hr;
  hr = CreateVertexBuffer();
  if (FAILED(hr)) return E_FAIL;
  hr = CreateIndexBuffer();
  if (FAILED(hr)) return E_FAIL;
  
    //ID3D11RasterizerState* hpRasterizerState = NULL;
    //D3D11_RASTERIZER_DESC hRasterizerDesc = {
    //  D3D11_FILL_WIREFRAME,
    //  D3D11_CULL_NONE,	//ポリゴンの裏表を無くす
    //  FALSE,
    //  0,
    //  0.0f,
    //  FALSE,
    //  FALSE,
    //  FALSE,
    //  FALSE,
    //  FALSE
    //};
    //if (FAILED(D3D11Renderer::GetInstance()->GetDevice()->CreateRasterizerState(&hRasterizerDesc, &hpRasterizerState))) {
    //  MessageBox(NULL, "RastrerizerCreate", "error!", MB_OK);
    //}
    //
    ////ラスタライザーをコンテキストに設定
    //D3D11Renderer::GetInstance()->GetDeviceContext()->RSSetState(hpRasterizerState);

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// 更新処理
void Skydome::Update()
{
  GetTransform()->Rotate(Vector3(0.0f, 0.0001f, 0.0f));
  GetTransform()->SetPosition(Vec3(CCameraMan::GetInstance()->GetCamera()->GetPos()));
}


///////////////////////////////////////////////////////////////////////////////
// 描画処理
void Skydome::Render(CCamera* pCamera)
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
HRESULT Skydome::CreateVertexBuffer()
{
  HRESULT result;
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;
  // デバイスの取得
  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();

  // 頂点の一時バッファ確保
  Vertex* vertices = new Vertex[m_vertexCnt];

  float fTheta = 0.0f;
  float fPhi = 0.0f;
  vertices[0].pos = Vector3(0.0f, m_radius, 0.0f);
  vertices[0].tex = Vector2(0.5f, 0.0f);
  for (int height = 1; height < m_numDivision.y; height++)
  {
    fTheta = (DirectX::XM_PI / m_numDivision.y) * height + (DirectX::XM_PI*0.5f);
    for (int width = 0; width <= m_numDivision.x; width++)
    {
      fPhi = (2 * DirectX::XM_PI / m_numDivision.x) * width;
      vertices[((height - 1)*((int)m_numDivision.x + 1)) + width + 1].pos = Vector3(m_radius*cos(fTheta)*sin(fPhi), m_radius*sin(fTheta), m_radius*cos(fTheta)*cos(fPhi));
      vertices[((height - 1)*((int)m_numDivision.x + 1)) + width + 1].tex = Vector2(1.0f / m_numDivision.x*width, 1.0f / m_numDivision.y*height);
    }
  }
  vertices[m_vertexCnt - 1].pos = Vector3(0.0f, -m_radius, 0.0f);
  vertices[m_vertexCnt - 1].tex = Vector2(0.5f, 1.0f);

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

  // 頂点バッファ作成
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
HRESULT Skydome::CreateIndexBuffer(void)
{
  HRESULT result;
  D3D11_BUFFER_DESC indexBufferDesc;
  D3D11_SUBRESOURCE_DATA indexData;

  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();

  unsigned long* index = new unsigned long[m_indexCnt];
 
  // 頂点座標の設定
  for (int height = 0; height < m_numDivision.y - 2; height++)
  {
    if (height != 0)
    {
      index[(2 + 2 + 2 * (int)m_numDivision.x)*height - 2] = ((height - 1)*(m_numDivision.x + 1)) + m_numDivision.x + 1;
      index[(2 + 2 + 2 * (int)m_numDivision.x)*height - 1] = ((height + 1)*(m_numDivision.x + 1)) + 1;
    }
    index[(2 + 2 + 2 * (int)m_numDivision.x)*height + 0] = (height + 1) * (m_numDivision.x + 1) + 1;
    index[(2 + 2 + 2 * (int)m_numDivision.x)*height + 1] = height * (m_numDivision.x + 1) + 1;

    for (int width = 1; width <= m_numDivision.x; width++)
    {
      index[(2 + 2 + 2 * (int)m_numDivision.x)*height + width * 2 + 0] = ((height + 1) * (m_numDivision.x + 1)) + width + 1;
      index[(2 + 2 + 2 * (int)m_numDivision.x)*height + width * 2 + 1] = (height * (m_numDivision.x + 1)) + width + 1;
    }
  }
  // 静的インデックスバッファの説明を設定します。 
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCnt;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;
  indexBufferDesc.StructureByteStride = 0;

  // サブリソース構造体にインデックスデータへのポインタを渡します。 
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

