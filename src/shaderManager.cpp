//=============================================================================
// File : shaderManager.cpp
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <Windows.h>
#include "appManager.h"
#include "shaderManager.h"
#include "shader.h"
#include "renderer.h"
#include <algorithm>


// default
#include "skydomeDefaultShader.h"
#include "terrainDefaultShader.h"
#include "expSpriteDefaultShader.h"

// depth shadow
#include "skinShadowShader.h"

// cascade shadow
#include "cascadeDepthShader.h"
#include "skinCascadeDepthShader.h"
#include "terrainCascadeDepthShader.h"
#include "regidCascadeShadowShader.h"
#include "skinCascadeShadowShader.h"
#include "terrainCascadeShadowShader.h"


///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
ShaderManager::ShaderManager()
{
  // 対象の種類分だけ配列確保
  m_useShader.resize(eShaderTypeMax);
}


///////////////////////////////////////////////////////////////////////////////
// デストラクタ
ShaderManager::~ShaderManager()
{
  // 全シェーダーデータ削除
  AllRelease();
}


///////////////////////////////////////////////////////////////////////////////
// 使用するシェーダクラスの作成
void ShaderManager::Initialize()
{
  // ゲームに使用するシェーダーの読み込み
  m_resShader["skinShadow"]     = new SkinShadowShader;
  m_resShader["regidDefault"]   = new ExpSpriteDefaultShader;
  m_resShader["terrainDefault"] = new TerrainDefaultShader;
  m_resShader["skyDefault"]     = new SkydomeDefaultShader;
  //m_resShader["skinDepth"]      = new SkinDepthShader;
  //m_resShader["skinDepth"]      = new SkinDepthShader;


  m_resShader["cascadeDepth"]       = new CascadeDepthShader;
  m_resShader["skinCascadeDepth"]   = new SkinCascadeDepthShader;
  m_resShader["terrainCascadeDepth"]= new TerrainCascadeDepthShader;
  m_resShader["regidCascadeShadow"] = new RegidCascadeShadowShader;
  m_resShader["skinCascadeShadow"]  = new SkinCascadeShadowShader;
  m_resShader["terrainCascadeShadow"] = new TerrainCascadeShadowShader;
}


///////////////////////////////////////////////////////////////////////////////
// 現在セット中のタイプ毎のシェーダーを取得
Shader*  ShaderManager::GetShader(eShaderType type)
{
  return m_useShader[type];
}


///////////////////////////////////////////////////////////////////////////////
// 普通のシェーダーのセット
void ShaderManager::SetDefaultShader(void)
{
  m_useShader[eShaderTypeSky]  = m_resShader["skyDefault"];
  m_useShader[eShaderTypeTerrain] = m_resShader["terrainDefault"];
  m_useShader[eShaderTypeRegid]  = m_resShader["regidDefault"];
//  m_useShader[eShaderTypeSkin]   = m_resShader["skinDefault"];
//  m_useShader[eShaderTypeSprite] = m_resShader["spriteDefault"];
}


///////////////////////////////////////////////////////////////////////////////
// 深度情報書き込み用シェーダーのセット
void ShaderManager::SetDepthShader(void)
{
  m_useShader[eShaderTypeRegid]  = m_resShader["regidDepth"];
  m_useShader[eShaderTypeSkin]   = m_resShader["skinDepth"];
}


///////////////////////////////////////////////////////////////////////////////
// シャドウマップを利用したシェーダーのセット
void ShaderManager::SetShadowShader(void)
{
  m_useShader[eShaderTypeRegid]  = m_resShader["regidShadow"];
//  m_useShader[eShaderTypeSkin]   = m_resShader["skinShadow"];
}


///////////////////////////////////////////////////////////////////////////////
// カスケード対応の深度情報書き込み用シェーダーのセット
void ShaderManager::SetCascadeDepthShader(const DirectX::XMMATRIX& proj, const DirectX::XMMATRIX& view)
{
  // シェーダのセット
  m_useShader[eShaderTypeRegid] = m_resShader["cascadeDepth"];
  m_useShader[eShaderTypeSkin] = m_resShader["skinCascadeDepth"];
  m_useShader[eShaderTypeTerrain] = m_resShader["terrainCascadeDepth"];
}


///////////////////////////////////////////////////////////////////////////////
// カスケードシャドウマップを利用したオブジェクト描画シェーダーのセット
void ShaderManager::SetCascadeShader(DirectX::XMMATRIX* proj, DirectX::XMMATRIX* view, RenderTexture** zbuffer)
{
  // シェーダーのセット
  SetDefaultShader(); // シャドウオブジェクト対象外は通常シェーダ描画する
  m_useShader[eShaderTypeRegid] = m_resShader["regidCascadeShadow"];
  m_useShader[eShaderTypeSkin] = m_resShader["skinCascadeShadow"];
  m_useShader[eShaderTypeTerrain] = m_resShader["terrainCascadeShadow"];

  // 固定情報のセット
  ((RegidCascadeShadowShader*)m_useShader[eShaderTypeRegid])->Set(proj, view, zbuffer);
  ((SkinCascadeShadowShader*)m_useShader[eShaderTypeSkin])->Set(proj, view, zbuffer);
  ((TerrainCascadeShadowShader*)m_useShader[eShaderTypeTerrain])->Set(proj, view, zbuffer);
}


///////////////////////////////////////////////////////////////////////////////
// 遅延レンダリングのマルチサンプリング描画シェーダーのセット
void ShaderManager::SetDeferredShader(void)
{
  m_useShader[eShaderTypeRegid]  = m_resShader["regidDeferred"];
  m_useShader[eShaderTypeSkin]   = m_resShader["skinDeferred"];
}


///////////////////////////////////////////////////////////////////////////////
// 遅延レンダリングの最終描画用シェーダーセット
void ShaderManager::SetDeferredBlendShader(void)
{
  m_useShader[eShaderTypeSprite] = m_resShader["spriteDefaultEnd"];
}


///////////////////////////////////////////////////////////////////////////////
// 頂点シェーダロード
bool ShaderManager::LoadVS(const std::string& key, D3D11_INPUT_ELEMENT_DESC* layout, int numElements)
{
  std::unordered_map<std::string, VertexShader>::const_iterator itr = m_resVS.find(key);
  if (itr != m_resVS.end()) return true; // 既にロードされている。

  VertexShader vertexShader;
  if (!CreateVertexShader(key, &vertexShader.pVS, &vertexShader.pIL, layout, numElements)) {
    MessageBox(NULL, key.c_str(), "MakeVertexShaderError!", MB_OK);
    return false; // 作成失敗
  }

  // ロード成功
  m_resVS[key] = vertexShader;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// 指定の頂点シェーダリソースのゲット
ShaderManager::VertexShader ShaderManager::GetVS(const std::string& key)
{
  std::unordered_map<std::string, VertexShader>::const_iterator itr = m_resVS.find(key);
  if (itr == m_resVS.end()) return VertexShader(); // その名前のリソースが存在しない

  // 取得成功
  return itr->second;
}


///////////////////////////////////////////////////////////////////////////////
// ピクセルシェーダロード
bool ShaderManager::LoadPS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11PixelShader*>::const_iterator itr = m_resPS.find(key);
  if( itr != m_resPS.end()) return true; // 既にロードされている。

  ID3D11PixelShader* pixelShader;
  if( !CreatePixelShader( key, &pixelShader)) {
    MessageBox(NULL, key.c_str(), "MakePicelShaderError!", MB_OK);
    return false; // 作成失敗
  }

  // ロード成功
  m_resPS[ key] = pixelShader;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// 指定のピクセルシェーダリソースのゲット
ID3D11PixelShader* ShaderManager::GetPS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11PixelShader*>::const_iterator itr = m_resPS.find(key);
  if (itr == m_resPS.end()) return nullptr; // その名前のリソースが存在しない

  // 取得成功
  return itr->second;
}


///////////////////////////////////////////////////////////////////////////////
// ジオメトリシェーダロード
bool ShaderManager::LoadGS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11GeometryShader*>::const_iterator itr = m_resGS.find(key);
  if (itr != m_resGS.end()) return true; // 既にロードされている。

  ID3D11GeometryShader* geometryShader;
  if (!CreateGeometryShader(key, &geometryShader)) {
    MessageBox(NULL, key.c_str(), "MakeGeometryShaderError!", MB_OK);
    return false; // 作成失敗
  }

  // ロード成功
  m_resGS[key] = geometryShader;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// 指定のジオメトリシェーダリソースのゲット
ID3D11GeometryShader* ShaderManager::GetGS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11GeometryShader*>::const_iterator itr = m_resGS.find(key);
  if (itr == m_resGS.end()) return nullptr; // その名前のリソースが存在しない

  // 取得成功
  return itr->second;
}


///////////////////////////////////////////////////////////////////////////////
// ハルシェーダロード
bool ShaderManager::LoadHS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11HullShader*>::const_iterator itr = m_resHS.find(key);
  if (itr != m_resHS.end()) return true; // 既にロードされている。

  ID3D11HullShader* hullShader;
  if (!CreateHullShader(key, &hullShader)) {
    MessageBox(NULL, key.c_str(), "MakeHullShaderError!", MB_OK);
    return false; // 作成失敗
  }

  // ロード成功
  m_resHS[key] = hullShader;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// 指定のハルシェーダリソースのゲット
ID3D11HullShader* ShaderManager::GetHS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11HullShader*>::const_iterator itr = m_resHS.find(key);
  if (itr == m_resHS.end()) return nullptr; // その名前のリソースが存在しない

  // 取得成功
  return itr->second;
}

///////////////////////////////////////////////////////////////////////////////
// ドメインシェーダロード
bool ShaderManager::LoadDS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11DomainShader*>::const_iterator itr = m_resDS.find(key);
  if (itr != m_resDS.end()) return true; // 既にロードされている。

  ID3D11DomainShader* domainShader;
  if (!CreateDomainShader(key, &domainShader)) {
    MessageBox(NULL, key.c_str(), "MakeDomainShaderError!", MB_OK);
    return false; // 作成失敗
  }

  // ロード成功
  m_resDS[key] = domainShader;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// 指定のドメインシェーダリソースのゲット
ID3D11DomainShader* ShaderManager::GetDS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11DomainShader*>::const_iterator itr = m_resDS.find(key);
  if (itr == m_resDS.end()) return nullptr; // その名前のリソースが存在しない

  // 取得成功
  return itr->second;
}

///////////////////////////////////////////////////////////////////////////////
// 全てのシェーダーリソースの削除 エラーどうやって出せばいいんだ？ 後日談
bool ShaderManager::AllRelease(void)
{
  // シェーダクラス解放
  for (auto&& shader : m_resShader) {
    delete shader.second;
  }
  m_resShader.clear();
  m_useShader.clear();

  // 頂点シェーダ・入力レイアウト解放処理
  for (auto&& vs : m_resVS) {
    vs.second.pVS->Release();
    vs.second.pIL->Release();
  }
  m_resVS.clear();

  // ピクセルシェーダ解放処理
  for (auto&& ps : m_resPS) {
    ps.second->Release();
  }
  m_resPS.clear();

  // ジオメトリシェーダ解放処理
  for (auto&& gs : m_resGS) {
    gs.second->Release();
  }
  m_resGS.clear();

  // ハルシェーダ解放処理
  for (auto&& hs : m_resHS) {
    hs.second->Release();
  }
  m_resHS.clear();

  // ドメインシェーダ解放処理
  for (auto&& ds : m_resDS) {
    ds.second->Release();
  }
  m_resDS.clear();
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// 頂点シェーダー作成
bool ShaderManager::CreateVertexShader(const std::string& csoName, ID3D11VertexShader** resVS, ID3D11InputLayout** resIL, D3D11_INPUT_ELEMENT_DESC* layout, int numElements)
{
  FILE* fp = fopen(csoName.c_str(), "rb");
  if (fp == NULL) return false;

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char* cso_data = new unsigned char[cso_sz];
  fread(cso_data, cso_sz, 1, fp);
  fclose(fp);

  ID3D11VertexShader* pVertexShader;
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateVertexShader(cso_data, cso_sz, NULL, &pVertexShader);
  if (FAILED(hr)) return false;
  *resVS = pVertexShader;

  ID3D11InputLayout* pInputLayout;
  hr = AppManager::GetRenderer()->GetDevice()->CreateInputLayout(layout, numElements, cso_data, cso_sz, &pInputLayout);
  if (FAILED(hr)) return false;
  *resIL = pInputLayout;

  delete[] cso_data;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// ピクセルシェーダー作成
bool ShaderManager::CreatePixelShader(const std::string& csoName, ID3D11PixelShader** resPS)
{
  FILE* fp = fopen(csoName.c_str(), "rb");
  if (fp == NULL) return false;

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char* cso_data = new unsigned char[cso_sz];
  fread(cso_data, cso_sz, 1, fp);
  fclose(fp);

  ID3D11PixelShader* pPixelShader;
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreatePixelShader(cso_data, cso_sz, NULL, &pPixelShader);
  if (FAILED(hr)) return false;
  *resPS = pPixelShader;

  delete[] cso_data;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// ジオメトリシェーダ作成
bool ShaderManager::CreateGeometryShader( const std::string& csoName, ID3D11GeometryShader** resGS)
{
  FILE* fp = fopen(csoName.c_str(), "rb");
  if (fp == NULL) return false;

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char* cso_data = new unsigned char[cso_sz];
  fread(cso_data, cso_sz, 1, fp);
  fclose(fp);

  ID3D11GeometryShader* pGeometryShader;
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateGeometryShader(cso_data, cso_sz, NULL, &pGeometryShader);
  if (FAILED(hr)) return false;
  *resGS = pGeometryShader;

  delete[] cso_data;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// ハルシェーダー作成
bool ShaderManager::CreateHullShader(const std::string& csoName, ID3D11HullShader** resHS)
{
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// ドメインシェーダー作成
bool ShaderManager::CreateDomainShader(const std::string& csoName, ID3D11DomainShader** resDS)
{
  return true;
}
