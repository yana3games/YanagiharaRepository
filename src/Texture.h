#pragma once
//=============================================================================
// File : Texture.h
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "renderer.h"
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

struct IWICImagingFactory;

namespace imageutil {
  class Image
  {
   private:
    std::vector<unsigned char> m_buffer;
    int m_width;
    int m_height;
    int m_pixelBytes;

   public:
    Image(int w, int h, int pixelBytes): m_width(w), m_height(h), m_pixelBytes(pixelBytes), m_buffer(w*h*pixelBytes) {};
    int Width()const { return m_width; }
    int Stride()const { return m_width*m_pixelBytes; }
    int Height()const { return m_height; }
    unsigned char* Pointer() { return m_buffer.empty() ? nullptr : &m_buffer[0]; }
    size_t Size()const { return m_buffer.size(); }
  };

  class Factory
  {
    IWICImagingFactory *m_factory;

   public:
    Factory();
    ~Factory();
    std::shared_ptr<Image> Load(const std::wstring &path);
  };

  class TgaFactory
  {
  public:
    TgaFactory() {};
    ~TgaFactory() {};
    std::shared_ptr<Image> Load(const std::wstring &path);
  };
}

class Texture
{
 protected:
  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;        // テクスチャオブジェクト
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;   // テクスチャのシェーダーリソースビュー
  Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;     // テクスチャのサンプラーステート

 public:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResouceView(void);
  bool Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &device
    , const std::shared_ptr<imageutil::Image> &image);

  void Set(int registerIndex);
  // ID3D11ShaderResourceView** GetSRV() { return &m_srv; }// これやるとデータ消えるんで（戒め）
};
