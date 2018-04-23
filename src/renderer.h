#pragma once
//=============================================================================
// File : renderer.h
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "viewingFrustum.h"
#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include <list>

///////////////////////////////////////////////////////////////////////////////
// �O���錾
class GameObject;
class Sun;
class CascadeShadowManager;
class CCamera;


///////////////////////////////////////////////////////////////////////////////
// �N���X�̒�`

// �����_���[�N���X
class D3D11Renderer
{
 public:
  // �`�揇���C���[
  typedef enum
  {
    eLayerSky = 0,           // �w�i
    eLayerNormalObject,      // �I�u�W�F�N�g
    eLayerTranslucentObject, // �������I�u�W�F�N�g
    eLayerEffect,            // �G�t�F�N�g��
    eLayerBillboard,         // �r���{�[�h
    eLayerUI,                // UI
    eLayerFade,              // �t�F�[�h�C��
    eLayerMax                // ���C���[��
  }eLayer;

  ~D3D11Renderer();
  HRESULT Init(HWND hWnd);
  void DrawBegin();
  void Draw();
  void DrawEnd();

  // �`��֘A
  void AddDrawLayerList(eLayer layer, GameObject* object);
  void DrawAll(CCamera*);
  void DrawLayer(CCamera*, eLayer start, eLayer end);
  void ReleaseAll(void);
  void Release(GameObject* object, eLayer layer);

  // �e��Q�b�^�[
  ID3D11Device* GetDevice(void);
  ID3D11DeviceContext* GetDeviceContext(void);
  ID3D11RenderTargetView** GetRenderTargetView(void) { return &m_pRTV; }
  ID3D11DepthStencilView** GetDepthStencilView(void) { return &m_pDSV; }
  Sun* GetSun(void) { return m_sun; }

 private:
  // �I�u�W�F�N�g�P�ʂ̕`��
  void DrawObject(GameObject* object, CCamera* pCamera);

  // �`�惊�X�g
  std::vector<std::list<GameObject*>> m_drawObjectLayerList;

  // ������J�����O�}�l�[�W��
  ViewingFrustum m_viewingFrustum;

  // �e�p�̑��z�f�[�^
  Sun* m_sun;

  // �J�X�P�[�h�V���h�E�`��p�}�l�[�W��
  CascadeShadowManager* m_pCascadeShadowManager;


  ID3D11Device*              m_pDevice;               // DirectX11�̃f�o�C�X
  ID3D11DeviceContext*       m_pDeviceContext;        // DirectX11�̃f�o�C�X�R���e�L�X�g
  IDXGISwapChain*            m_pSwapChain;            // �X���b�v�`�F�C��

  D3D_DRIVER_TYPE            m_DriverType;            // �h���C�o�[�^�C�v
  D3D_FEATURE_LEVEL          m_FeatureLevel;          // �@�\���x���̈�ԍ����̊i�[
  UINT                       m_MultiSampleCount;      // �}���`�T���v���̃J�E���g
  UINT                       m_MultiSampleQuality;    // �}���`�T���v���̕i��
  UINT                       m_MultiSampleMaxQuality; // �}���`�T���v���̍ő�i��
  UINT                       m_SwapChainCount;        // �X���b�v�`�F�C���̃J�E���g
  DXGI_FORMAT                m_SwapChainFormat;       // �X���b�v�`�F�C���̃t�H�[�}�b�g
  DXGI_FORMAT                m_DepthStencilFormat;    // �[�x�X�e���V���̃t�H�[�}�b�g

  ID3D11Texture2D*           m_pRTT;                  // �����_�����O�^�[�Q�b�g�̃e�N�X�`��
  ID3D11RenderTargetView*    m_pRTV;                  // �����_�[�^�[�Q�b�g�r���[�H
  ID3D11ShaderResourceView*  m_pRTSRV;                // ���Ƃ��Ďg������Ƃ��@�����_�[�^�[�Q�b�g�̃V�F�[�_�[���\�[�X�r���[
  ID3D11Texture2D*           m_pDST;                  // �[�x�X�e���V���e�N�X�`��
  ID3D11DepthStencilView*    m_pDSV;                  // �[�x�X�e���V���r���[
  ID3D11ShaderResourceView*  m_pDSSRV;                // �[�x�X�e���V���̃V�F�[�_���\�[�X�b�r���[
  FLOAT                      m_ClearColor[4];         // �N���A�J���[

  bool                       m_createFlag;            // �����t���O
};