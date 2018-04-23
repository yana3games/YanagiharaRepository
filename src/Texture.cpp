//=============================================================================
// File : Texture.cpp
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================
#include "Texture.h"
#include "appManager.h"
#include "renderer.h"
#include <Windows.h>
#include <wincodec.h>


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
imageutil::Factory::Factory(): m_factory(nullptr){
  HRESULT hr = CoCreateInstance(
    CLSID_WICImagingFactory,
    nullptr,
    CLSCTX_INPROC_SERVER,
    IID_PPV_ARGS(&m_factory)
  );
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
imageutil::Factory::~Factory() {
  m_factory->Release();
  m_factory = nullptr;
}


///////////////////////////////////////////////////////////////////////////////
// TGA�e�N�X�`���̃��[�h(WIC��Ή��̈׎���
std::shared_ptr<imageutil::Image> imageutil::TgaFactory::Load(const std::wstring &path)
{
  // 2�o�C�gchar����1�o�C�gchar�ɕϊ�
  char filename[256] = { '\0' };
  wcstombs(filename, path.c_str(), path.size());

  // �t�@�C���̓ǂݍ���
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    MessageBox(NULL, "tga�t�@�C���̓ǂݍ��݂Ɏ��s���܂����I��", "�G���[�I", MB_OK);
    return nullptr;
  }

  // �w�b�_���̓ǂݍ���
  unsigned char header[18];
  fread(header, 1, 18, fp);

  // �摜�̕������̎擾
  unsigned int width, height;
  width = *((WORD*)&header[12]);
  height = *((WORD*)&header[14]);

  // Image�f�[�^�̍쐬
  auto image = std::make_shared<Image>(width, height, 4);

  // �s�N�Z���f�[�^�̊i�[
  unsigned char* data = image->Pointer();
  // �r�b�g�[�x���̎擾
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
    MessageBox(NULL, "Bit�[�x32/24����Ȃ��񂾂��ǁI��", "TGA�t�H�[�}�b�g�G���[�I", MB_OK);
    return nullptr;
  }

  // �t�@�C���N���[�Y
  fclose(fp);

  return image;
}


///////////////////////////////////////////////////////////////////////////////
// �e�N�X�`���̃��[�h
std::shared_ptr<imageutil::Image> imageutil::Factory::Load(const std::wstring &path) 
{
  // decoder����ăt�@�C����n��
  Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
  HRESULT hr = m_factory->CreateDecoderFromFilename(path.c_str(), 0
    , GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
  if (FAILED(hr)) {
    return nullptr;
  }

  // decoder����frame���擾
  Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
  hr = decoder->GetFrame(0, &frame);
  if (FAILED(hr)) {
    return nullptr;
  }

  // �t���[������T�C�Y�ƃs�N�Z���t�H�[�}�b�g�ƃf�[�^�𓾂�
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
    // �ϊ�����
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
// �e�N�X�`���̐���
bool Texture::Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &device
  , const std::shared_ptr<imageutil::Image> &image)
{
  // �e�N�X�`���I�u�W�F�N�g�̐ݒ�
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

  // �e�N�X�`���I�u�W�F�N�g�̍쐬
  auto hr = device->CreateTexture2D(&desc, &initData, &m_texture);
  if (FAILED(hr)) {
    return false;
  }

  // �e�N�X�`���p�V�F�[�_�[���\�[�X�r���[�ݒ�
  D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
  SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  SRVDesc.Texture2D.MipLevels = 1;

  // �e�N�X�`���̃V�F�[�_�[���\�[�X�r���[�쐬
  hr = device->CreateShaderResourceView(m_texture.Get(), &SRVDesc, &m_srv);
  if (FAILED(hr))
  {
    return false;
  }

  // �e�N�X�`���[�̃T���v���[�X�e�[�g�ݒ�
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

  // �e�N�X�`���[�̃T���v���[�X�e�[�g�쐬
  hr = device->CreateSamplerState(&samplerDesc, &m_sampler);
  if (FAILED(hr))
  {
    return false;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �e�N�X�`���̃Z�b�g
void Texture::Set(int registerIndex)
{
  AppManager::GetRenderer()->GetDeviceContext()->PSSetShaderResources(registerIndex, 1, m_srv.GetAddressOf());
  AppManager::GetRenderer()->GetDeviceContext()->PSSetSamplers(registerIndex, 1, m_sampler.GetAddressOf());
}


///////////////////////////////////////////////////////////////////////////////
// �V�F�[�_�[���\�[�X�r���[�̃Q�b�^�[
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Texture::GetShaderResouceView(void)
{
  return m_srv;
}