#pragma once
//=============================================================================
// File  : shaderManager.h
// Date  : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <d3d11.h>
#include <unordered_map>
#include <DirectXMath.h>


///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class Shader;
class RenderTexture;


///////////////////////////////////////////////////////////////////////////////
// シェーダー管理クラス
class ShaderManager
{
 public:
  // 頂点シェーダと入力レイアウトのラップ
  struct VertexShader {
    ID3D11VertexShader* pVS;
    ID3D11InputLayout* pIL;
  };

  // シェーダーを適応するオブジェクトの種類
  typedef enum {
    eShaderTypeSky = 0,
    eShaderTypeRegid,
    eShaderTypeSkin,
    eShaderTypeTerrain,
    eShaderTypeSprite,
    eShaderTypeMax
  }eShaderType;
  ShaderManager();
  ~ShaderManager();

  // 使用するシェーダクラス作成
  void Initialize(void);

  // セットされてるシェーダの取得
  Shader* GetShader( eShaderType type);

  // 普通のシェーダーのセット
  void SetDefaultShader(void);

  // 深度情報格納用シェーダーのセット
  void SetDepthShader(void);

  // シャドウマップを利用するシェーダーのセット
  void SetShadowShader(void);

  // カスケード対応の深度情報格納用シェーダーのセット
  void SetCascadeDepthShader(const DirectX::XMMATRIX& proj, const DirectX::XMMATRIX& view);

  // カスケードシャドウマップを利用するシェーダーのセット
  void SetCascadeShader(DirectX::XMMATRIX* proj, DirectX::XMMATRIX* view, RenderTexture** zbuffer);
  
  // 遅延レンダリングのマルチレンダー描画シェーダーのセット
  void SetDeferredShader(void);

  // 遅延レンダリングの最終描画用シェーダーセット
  void SetDeferredBlendShader(void);

  // VertexShader
  bool LoadVS(const std::string& key, D3D11_INPUT_ELEMENT_DESC* layout, int numElements);
  VertexShader GetVS(const std::string& key);
  
  // PixelShader
  bool LoadPS(const std::string& key);
  ID3D11PixelShader* GetPS(const std::string& key);

  // GeometryShader
  bool LoadGS(const std::string& key);
  ID3D11GeometryShader* GetGS(const std::string& key);

  // HullShader
  bool LoadHS(const std::string& key);
  ID3D11HullShader* GetHS(const std::string& key);

  // DomainShader
  bool LoadDS(const std::string& key);
  ID3D11DomainShader* GetDS(const std::string& key);

  // AllShader
  bool AllRelease(void);

 private:
  bool CreateVertexShader(  const std::string& csoName, ID3D11VertexShader**   resVS, ID3D11InputLayout** resIL, D3D11_INPUT_ELEMENT_DESC* layout, int numElements); // 頂点シェーダ作成
  bool CreatePixelShader(   const std::string& csoName, ID3D11PixelShader**    resPS); // ピクセルシェーダ作成
  bool CreateGeometryShader(const std::string& csoName, ID3D11GeometryShader** resGS); // ジオメトリシェーダ作成
  bool CreateHullShader(    const std::string& csoName, ID3D11HullShader**     resHS); // ハルシェーダ作成
  bool CreateDomainShader(  const std::string& csoName, ID3D11DomainShader**   resDS); // ドメインシェーダ作成

  std::unordered_map< std::string, VertexShader>          m_resVS;  // 頂点シェーダバッファマップ
  std::unordered_map< std::string, ID3D11PixelShader*>    m_resPS;  // ピクセルシェーダマップ
  std::unordered_map< std::string, ID3D11GeometryShader*> m_resGS;  // ジオメトリシェーダバッファマップ
  std::unordered_map< std::string, ID3D11HullShader*>     m_resHS;  // ハルシェーダバッファマップ
  std::unordered_map< std::string, ID3D11DomainShader*>   m_resDS;  // ドメインシェーダバッファマップ

  std::unordered_map< std::string, Shader*>               m_resShader; // シェーダークラスバッファマップ
  std::vector< Shader*>                                   m_useShader; // 使用するシェーダーのバッファ
};