//=============================================================================
// File : Texture.cpp
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================
#include "Texture.h"
#include "appManager.h"
#include "renderer.h"
#include <Windows.h>
#include <wincodec.h>


///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
imageutil::Factory::Factory(): m_factory(nullptr){
  HRESULT hr = CoCreateInstance(
    CLSID_WICImagingFactory,
    nullptr,
    CLSCTX_INPROC_SERVER,
    IID_PPV_ARGS(&m_factory)
  );
}


///////////////////////////////////////////////////////////////////////////////
// デストラクタ
imageutil::Factory::~Factory() {
  m_factory->Release();
  m_factory = nullptr;
}


///////////////////////////////////////////////////////////////////////////////
// TGAテクスチャのロード(WIC非対応の為自作
std::shared_ptr<imageutil::Image> imageutil::TgaFactory::Load(const std::wstring &path)
{
  // 2バイトcharから1バイトcharに変換
  char filename[256] = { '\0' };
  wcstombs(filename, path.c_str(), path.size());

  // ファイルの読み込み
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    MessageBox(NULL, "tgaファイルの読み込みに失敗しました！ｗ", "エラー！", MB_OK);
    return nullptr;
  }

  // ヘッダ情報の読み込み
  unsigned char header[18];
  fread(header, 1, 18, fp);

  // 画像の幅高さの取得
  unsigned int width, height;
  width = *((WORD*)&header[12]);
  height = *((WORD*)&header[14]);

  // Imageデータの作成
  auto image = std::make_shared<Image>(width, height, 4);

  // ピクセルデータの格納
  unsigned char* data = image->Pointer();
  // ビット深度毎の取得
  if (header[16] == 24) {
    for (unsigned short h = 0; h < height; h++) {
      for (unsigned short w = 0; w < width; w++) {
        int offset = ((((height - 1) * width) - (h * width)) + w) * 4;
        data[ offset + 2] = fgetc(fp);
        data[ offset + 1] = fgetc(fp);
        data[ offset + 0] = fgetc(fp);
        data[ offset + 3] = 255;
      }
    }
  }
  else if (header[16] == 32) {
    for (unsigned short h = 0; h < height; h++) {
      for (unsigned short w = 0; w < width; w++) {
        int offset = ((((height-1) * width) - (h * width)) + w) * 4;
        data[ offset + 2] = fgetc(fp);
        data[ offset + 1] = fgetc(fp);
        data[ offset + 0] = fgetc(fp);
        data[ offset + 3] = fgetc(fp);
      }
    }
  }
  else{
    MessageBox(NULL, "Bit深度32/24じゃないんだけど！ｗ", "TGAフォーマットエラー！", MB_OK);
    return nullptr;
  }

  // ファイルクローズ
  fclose(fp);

  return image;
}


///////////////////////////////////////////////////////////////////////////////
// テクスチャのロード
std::shared_ptr<imageutil::Image> imageutil::Factory::Load(const std::wstring &path) 
{
  // decoder作ってファイルを渡す
  Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
  HRESULT hr = m_factory->CreateDecoderFromFilename(path.c_str(), 0
    , GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
  if (FAILED(hr)) {
    return nullptr;
  }

  // decoderからframeを取得
  Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
  hr = decoder->GetFrame(0, &frame);
  if (FAILED(hr)) {
    return nullptr;
  }

  // フレームからサイズとピクセルフォーマットとデータを得る
  UINT width, height;
  hr = frame->GetSize(&width, &height);
  if (FAILED(hr)) {
    return nullptr;
  }

  //assert(width > 0 && height > 0);

  // Determine format
  WICPixelFormatGUID pixelFormat;
  hr = frame->GetPixelFormat(&pixelFormat);
  if (FAILED(hr)) {
    return nullptr;
  }

  if (pixelFormat != GUID_WICPixelFormat32bppRGBA) {
    // 変換する
    Microsoft::WRL::ComPtr<IWICFormatConverter> FC;
    hr = m_factory->CreateFormatConverter(&FC);
    if (FAILED(hr)) {
      return nullptr;
    }

    hr = FC->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA
      , WICBitmapDitherTypeErrorDiffusion
      , 0, 0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) {
      return nullptr;
    }

    // copy
    auto image = std::make_shared<Image>(width, height, 4);
    FC->CopyPixels(0, image->Stride(), image->Size(), image->Pointer());
    return image;
  }
  else {
    // copy
    auto image = std::make_shared<Image>(width, height, 4);
    frame->CopyPixels(0, image->Stride(), image->Size(), image->Pointer());
    return image;
  }
}


///////////////////////////////////////////////////////////////////////////////
// テクスチャの生成
bool Texture::Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &device
  , const std::shared_ptr<imageutil::Image> &image)
{
  // テクスチャオブジェクトの設定
  D3D11_TEXTURE2D_DESC desc;
  desc.Width = image->Width();
  desc.Height = image->Height();
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA initData;
  initData.pSysMem = image->Pointer();
  initData.SysMemPitch = image->Stride();
  initData.SysMemSlicePitch = image->Size();

  // テクスチャオブジェクトの作成
  auto hr = device->CreateTexture2D(&desc, &initData, &m_texture);
  if (FAILED(hr)) {
    return false;
  }

  // テクスチャ用シェーダーリソースビュー設定
  D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
  SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  SRVDesc.Texture2D.MipLevels = 1;

  // テクスチャのシェーダーリソースビュー作成
  hr = device->CreateShaderResourceView(m_texture.Get(), &SRVDesc, &m_srv);
  if (FAILED(hr))
  {
    return false;
  }

  // テクスチャーのサンプラーステート設定
  D3D11_SAMPLER_DESC samplerDesc;
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.BorderColor[0] = 0;
  samplerDesc.BorderColor[1] = 0;
  samplerDesc.BorderColor[2] = 0;
  samplerDesc.BorderColor[3] = 0;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  // テクスチャーのサンプラーステート作成
  hr = device->CreateSamplerState(&samplerDesc, &m_sampler);
  if (FAILED(hr))
  {
    return false;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// テクスチャのセット
void Texture::Set(int registerIndex)
{
  AppManager::GetRenderer()->GetDeviceContext()->PSSetShaderResources(registerIndex, 1, m_srv.GetAddressOf());
  AppManager::GetRenderer()->GetDeviceContext()->PSSetSamplers(registerIndex, 1, m_sampler.GetAddressOf());
}


///////////////////////////////////////////////////////////////////////////////
// シェーダーリソースビューのゲッター
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Texture::GetShaderResouceView(void)
{
  return m_srv;
}