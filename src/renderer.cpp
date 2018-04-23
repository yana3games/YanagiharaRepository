//=============================================================================
// File : renderer.cpp
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
#include "renderer.h"
#include "gameObject.h"
#include "appManager.h"
#include "appConfig.h"
#include "appUtility.h"
#include "sun.h"
#include "cascadeShadowManager.h"
#include "cameraMan.h"
#include "camera.h"


///////////////////////////////////////////////////////////////////////////////
// �|���S���̏���������
HRESULT D3D11Renderer::Init(HWND hWnd)
{
  // �`�惊�X�g�����C���[���m��
  m_drawObjectLayerList.resize(eLayerMax);

  // �f�o�C�X�̏�����
  m_pDevice               = nullptr;                       // DirectX11�̃f�o�C�X
  m_pDeviceContext        = nullptr;                       // DirectX11�̃f�o�C�X�R���e�L�X�g
  m_pSwapChain            = nullptr;                       // �X���b�v�`�F�C��

  m_DriverType            = D3D_DRIVER_TYPE_NULL;          // �h���C�o�[�^�C�v
  m_FeatureLevel          = D3D_FEATURE_LEVEL_11_0;        // �@�\���x���̈�ԍ����̊i�[ti
  m_MultiSampleCount      = 4;                             // �}���`�T���v���̃J�E���g
  m_MultiSampleQuality    = 0;                             // �}���`�T���v���̕i��
  m_MultiSampleMaxQuality = 0;                             // �}���`�T���v���̍ő�i��
  m_SwapChainCount        = 2;                             // �X���b�v�`�F�C���̃J�E���g
  m_SwapChainFormat       = DXGI_FORMAT_R8G8B8A8_UNORM;    // �X���b�v�`�F�C���̃t�H�[�}�b�g
  m_DepthStencilFormat    = DXGI_FORMAT_D24_UNORM_S8_UINT; // �[�x�X�e���V���̃t�H�[�}�b�g

  m_pRTT                  = nullptr;                       // �����_�����O�^�[�Q�b�g�̃e�N�X�`��
  m_pRTV                  = nullptr;                       // �����_�[�^�[�Q�b�g�r���[
  m_pRTSRV                = nullptr;                       // ���Ƃ��Ďg������Ƃ��@�����_�[�^�[�Q�b�g�̃V�F�[�_�[���\�[�X�r���[
  m_pDST                  = nullptr;                       // �[�x�X�e���V���e�N�X�`��
  m_pDSV                  = nullptr;                       // �[�x�X�e���V���r���[
  m_pDSSRV                = nullptr;                       // �[�x�X�e���V���̃V�F�[�_���\�[�X�b�r���[
  m_ClearColor[0]         = 0.5f;                          // �N���A�J���[
  m_ClearColor[1]         = 0.8f;
  m_ClearColor[2]         = 0.2f;
  m_ClearColor[3]         = 1.0f;

  // �f�o�C�X�����t���O.
  UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif//defined(DEBUG) || deifned(_DEBUG)

  // �h���C�o�[�^�C�v.
  D3D_DRIVER_TYPE driverTypes[] = {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE,
  };
  UINT numDriverTytpes = sizeof(driverTypes) / sizeof(driverTypes[0]);

  // �@�\���x��.
  D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };
  UINT numFeatureLevels = sizeof(featureLevels) / sizeof(featureLevels[0]);

  // �X���b�v�`�F�C���̍\���ݒ�.
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
  sd.BufferCount                        = m_SwapChainCount;     //�o�b�t�@���i�_�u���o�b�t�@
  sd.BufferDesc.Width                   = AppConfig::GetInstance()->GetScreenWidth();         //�f�B�X�v���C�o�b�t�@�c������
  sd.BufferDesc.Height                  = AppConfig::GetInstance()->GetScreenHeight();
  sd.BufferDesc.Format                  = m_SwapChainFormat;    //DXGI_FORMAT_R8G8B8A8_UNORM; // ���\�[�X�f�[�^�t�H�[�}�b�g
  sd.BufferDesc.RefreshRate.Numerator   = 60;                   //���q 60fps�Ń��t���b�V��
  sd.BufferDesc.RefreshRate.Denominator = 1;                    //����
  sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;//�f�B�X�v���C�o�b�t�@�̎g�����B�����_�[�^�[�Q�b�g�Ƃ��Ďg�p�E�����_�����O�e�N�X�`��
  sd.OutputWindow                       = hWnd;                 //�E�B���h�E�n���h��
  //�A���`�G�C���A�V���O�g���ꍇ�́��̐���������Ə���ɂ���Ă������ۂ��H
  sd.SampleDesc.Count                   = m_MultiSampleCount;   //�}���`�T���v���̃T���v�����O�J�E���g��
  sd.SampleDesc.Quality                 = m_MultiSampleQuality; //�l�������قǕi��up
  sd.Windowed                           = TRUE;                 //TRUE(�E�B���h�E���[�h) FALSE(�t���X�N���[�����[�h)

  for (UINT idx = 0; idx < numDriverTytpes; ++idx)
  {
    // �h���C�o�[�^�C�v�ݒ�.
    m_DriverType = driverTypes[idx];

    // �f�o�C�X�ƃX���b�v�`�F�C���̐���.
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
      NULL,               //�r�f�I�A�_�v�^�[�ւ̃|�C���^
      m_DriverType,       //�쐬����f�o�C�X�̎��
      NULL,               //�\�t�g�E�F�A���X�^���C�U�[����������DLL�̃n���h��
      createDeviceFlags,  //�L���ɂ��郉���^�C�����C���[
      featureLevels,      //�쐬�����݂�@�\���x���̏���
      numFeatureLevels,   //�t���[�`���[���x���̗v�f���H
      D3D11_SDK_VERSION,  //SDK�̃o�[�W����
      &sd,                //�X���b�v�`�F�[���̏������p�����[�^
      &m_pSwapChain,      //�����_�����O�Ɏg�p����X���b�v�`�F�[��
      &m_pDevice,         //�쐬���ꂽ�f�o�C�X	ID3D11Device
      &m_FeatureLevel,    //���̃f�o�C�X�ŃT�|�[�g����Ă�@�\���x���̈�ԍ�����H
      &m_pDeviceContext   //�f�o�C�X�R���e�L�X�g ID3D11DeviceContext
      );
    // ���������烋�[�v��E�o.
    if (SUCCEEDED(hr)){
      break;
    }

    // ���s���Ă��Ȃ����`�F�b�N.
    if (FAILED(hr)){
      return false;
    }
  }

  HRESULT hr = S_OK;

  // �}���`�T���v���̍ő�i���l���擾.
  hr = m_pDevice->CheckMultisampleQualityLevels(m_SwapChainFormat, m_MultiSampleCount, &m_MultiSampleMaxQuality);
  if (FAILED(hr)){
    MessageBox(hWnd, "�}���`�T���v���̍ő�i���l�̎擾�Ɏ��s���܂����I", "�x���I", MB_ICONWARNING);
    return false;
  }

  // �o�b�N�o�b�t�@���擾
  hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pRTT);
  if (FAILED(hr)){
    MessageBox(hWnd, "�o�b�N�o�b�t�@�̎擾�Ɏ��s���܂����I", "�x���I", MB_ICONWARNING);
    return false;
  }

  // �����_�[�^�[�Q�b�g�𐶐�.
  hr = m_pDevice->CreateRenderTargetView(m_pRTT, NULL, &m_pRTV);
  if (FAILED(hr)){
    MessageBox(hWnd, "�����_�[�^�[�Q�b�g�̐����Ɏ��s���܂����I", "�x���I", MB_ICONWARNING);
    return false;
  }

  // �����_�[�^�[�Q�b�g�̃V�F�[�_���\�[�X�r���[�𐶐�.
  hr = m_pDevice->CreateShaderResourceView(m_pRTT, NULL, &m_pRTSRV);
  if (FAILED(hr)){
    MessageBox(hWnd, "�����_�[�^�[�Q�b�g�̃V�F�[�_���\�[�X�r���[�̐����Ɏ��s���܂����I", "�x���I", MB_ICONWARNING);
    return false;
  }

  DXGI_FORMAT textureFormat = m_DepthStencilFormat;
  DXGI_FORMAT resourceFormat = m_DepthStencilFormat;

  // �e�N�X�`���ƃV�F�[�_���\�[�X�r���[�̃t�H�[�}�b�g��K�؂Ȃ��̂ɕύX.
  switch (m_DepthStencilFormat)
  {
  case DXGI_FORMAT_D16_UNORM:
  {
    textureFormat = DXGI_FORMAT_R16_TYPELESS;
    resourceFormat = DXGI_FORMAT_R16_UNORM;
  }
  break;

  case DXGI_FORMAT_D24_UNORM_S8_UINT:
  {
    textureFormat = DXGI_FORMAT_R24G8_TYPELESS;
    resourceFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
  }
  break;

  case DXGI_FORMAT_D32_FLOAT:
  {
    textureFormat = DXGI_FORMAT_R32_TYPELESS;
    resourceFormat = DXGI_FORMAT_R32_FLOAT;
  }
  break;

  case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
  {
    textureFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
    resourceFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
  }
  break;
  }
  D3D11_TEXTURE2D_DESC td;

  // �[�x�X�e���V���e�N�X�`���̐���.
  ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
  td.Width					= AppConfig::GetInstance()->GetScreenWidth();
  td.Height					= AppConfig::GetInstance()->GetScreenHeight();
  td.MipLevels				= 1;
  td.ArraySize				= 1;
  td.Format					= textureFormat;
  td.SampleDesc.Count			= m_MultiSampleCount;
  td.SampleDesc.Quality		= m_MultiSampleQuality;
  td.Usage					= D3D11_USAGE_DEFAULT;
  td.BindFlags				= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
  td.CPUAccessFlags			= 0;
  td.MiscFlags				= 0;

  // �[�x�X�e���V���e�N�X�`���̐���.
  hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pDST);
  if (FAILED(hr)){
  	MessageBox(hWnd, "�[�x�X�e���V���e�N�X�`���̐����Ɏ��s���܂����I", "�x���I", MB_ICONWARNING);
  	return false;
  }

  // �[�x�X�e���V���r���[�̐ݒ�.
  //�}���`�T���v���̐����[���łȂ��Ƃ���
  //D3D11_DSV_DIMENSION_TEXTURE2DMS��
  //D3D11_SRV_DIMENSION_TEXTURE2DMS��ݒ肵�Ȃ��Ƃ����Ȃ�
  D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
  ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
  dsvd.Format = m_DepthStencilFormat;
  if (m_MultiSampleCount == 0){
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvd.Texture2D.MipSlice = 0;
  }
  else{
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
  }


  // �[�x�X�e���V���r���[�̐���.
  hr = m_pDevice->CreateDepthStencilView(m_pDST, &dsvd, &m_pDSV);
  if (FAILED(hr)) {
    MessageBox(hWnd, "�[�x�X�e���V���r���[�̐����Ɏ��s���܂����I", "�x���I", MB_ICONWARNING);
    return false;
  }

  // �V�F�[�_���\�[�X�r���[�̐ݒ�.
  D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
  ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
  srvd.Format = resourceFormat;

  if (m_MultiSampleCount == 0){
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MostDetailedMip = 0;
    srvd.Texture2D.MipLevels = 1;
  }
  else{
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
  }

  // �V�F�[�_���\�[�X�r���[�𐶐�.
  hr = m_pDevice->CreateShaderResourceView(m_pDST, &srvd, &m_pDSSRV);
  if (FAILED(hr)){
    MessageBox(hWnd, "�V�F�[�_�[���\�[�X�r���[�̐����Ɏ��s���܂����I", "�x���I", MB_ICONWARNING);
    return false;
  }

  // �f�o�C�X�R���e�L�X�g�Ƀ����_�[�^�[�Q�b�g��ݒ�.
  m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);

  // �r���[�|�[�g�̐ݒ�.
  D3D11_VIEWPORT vp;
  vp.Width = (float)AppConfig::GetInstance()->GetScreenWidth();
  vp.Height = (float)AppConfig::GetInstance()->GetScreenHeight();
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;

  // �f�o�C�X�R���e�L�X�g�Ƀr���[�|�[�g��ݒ�.
  m_pDeviceContext->RSSetViewports(1, &vp);
  
  // �u�����h�X�e�[�g�쐬
  //ID3D11BlendState* hpBlendState = nullptr;
  //D3D11_BLEND_DESC BlendStateDesc;
  //BlendStateDesc.AlphaToCoverageEnable = false;
  //BlendStateDesc.IndependentBlendEnable = false;
  //for (int i = 0; i < 8; i++) {
  //  BlendStateDesc.RenderTarget[i].BlendEnable = true;
  //  BlendStateDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  //  BlendStateDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  //  BlendStateDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
  //  BlendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
  //  BlendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
  //  BlendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  //  BlendStateDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  //}
  //m_pDevice->CreateBlendState(&BlendStateDesc, &hpBlendState);
  //
  //// �u�����h�X�e�[�g���R���e�L�X�g�ɐݒ�
  //float blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
  //m_pDeviceContext->OMSetBlendState(hpBlendState, blendFactor, 0xffffffff);


  // �e�p���z�f�[�^����
  m_sun = new Sun(Vector3(1000.0f, 1000.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f));

  // �J�X�P�[�h�V���h�E�p�}�l�[�W���̐���
  m_pCascadeShadowManager = new CascadeShadowManager;
  m_pCascadeShadowManager->SunSet(m_sun);

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// �����_���[�̏I������
D3D11Renderer::~D3D11Renderer(void)
{
  if (m_pDeviceContext){
    m_pDeviceContext->ClearState();
    m_pDeviceContext->Flush();
  }

  SAFE_RELEASE(m_pRTSRV)
  SAFE_RELEASE(m_pRTV)
  SAFE_RELEASE(m_pRTT)

  SAFE_RELEASE(m_pDSSRV)
  SAFE_RELEASE(m_pDSV)
  SAFE_RELEASE(m_pDST)

  SAFE_RELEASE(m_pSwapChain)
  SAFE_RELEASE(m_pDeviceContext)
  SAFE_RELEASE(m_pDevice)

  if (m_sun) delete m_sun;
  if (m_pCascadeShadowManager) delete m_pCascadeShadowManager;
}


///////////////////////////////////////////////////////////////////////////////
// �����_���[�̕`�揈��
void D3D11Renderer::DrawBegin(void)
{
  // �r���[���N���A����
  m_pDeviceContext->ClearRenderTargetView(m_pRTV, m_ClearColor);
  m_pDeviceContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


///////////////////////////////////////////////////////////////////////////////
// �����_���[�̕`�揈��
void D3D11Renderer::Draw(void)
{
  CCameraMan::GetInstance()->Set();
  //DrawAll(CCameraMan::GetInstance()->GetCamera());
  m_pCascadeShadowManager->Draw();
}


///////////////////////////////////////////////////////////////////////////////
// �����_���[�̕`�揈��
void D3D11Renderer::DrawEnd(void)
{
  // �`�挋�ʂ�\��
  m_pSwapChain->Present(0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// �`�惊�X�g�փI�u�W�F�N�g��ǉ�
void D3D11Renderer::AddDrawLayerList(eLayer layer, GameObject* object)
{
  m_drawObjectLayerList[layer].push_back(object);
}


///////////////////////////////////////////////////////////////////////////////
// �I�u�W�F�N�g�P�ʂ̕`��
void D3D11Renderer::DrawObject(GameObject* object, CCamera* pCamera)
{
  // �g�p����
  if (!object->IsVisible()) return;
  // ��������ɋ��邩
  if (!object->ViewingFrustumCulling(&m_viewingFrustum)) return;
  // �`��
  object->Render(pCamera);
}


///////////////////////////////////////////////////////////////////////////////
// �`�惊�X�g���̃I�u�W�F�N�g�S�Ă�`��
void D3D11Renderer::DrawAll(CCamera* pCamera)
{
  // ������J�����O�p�N���X�Ɏ���������Z�b�g
  m_viewingFrustum.SetupViewingFrustum(pCamera->GetFar(), *pCamera->GetProjMtx(), *pCamera->GetViewMtx());

  // �`��
  for (auto&& layer : m_drawObjectLayerList) {
    for (auto&& object : layer) {
      DrawObject(object, pCamera);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// ����̃��C���[���烌�C���[�܂ł̕`��
void D3D11Renderer::DrawLayer( CCamera* pCamera, eLayer start, eLayer end)
{
  // ������J�����O�p�N���X�Ɏ���������Z�b�g
  m_viewingFrustum.SetupViewingFrustum(pCamera->GetFar(), *pCamera->GetProjMtx(), *pCamera->GetViewMtx());

  // �`��
  for (auto layer = m_drawObjectLayerList.begin() + (int)start; layer != m_drawObjectLayerList.begin() + (int)end + 1; ++layer) {
    for (auto&& object : (*layer)) {
      DrawObject(object, pCamera);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// �S�`�惊�X�g�폜
void D3D11Renderer::ReleaseAll(void)
{
  for (auto layer = m_drawObjectLayerList.begin(); layer != m_drawObjectLayerList.begin(); ++layer) {
    layer->clear();
  }
  m_drawObjectLayerList.clear();
}


///////////////////////////////////////////////////////////////////////////////
// ����̃I�u�W�F�N�g��`�惌�C���[����폜�\��
void D3D11Renderer::Release(GameObject* object, eLayer layer)
{
  m_drawObjectLayerList[layer].erase( std::find(m_drawObjectLayerList[layer].begin(), m_drawObjectLayerList[layer].end(), object));
}


///////////////////////////////////////////////////////////////////////////////
// �f�o�C�X�̃Q�b�^�[
ID3D11Device* D3D11Renderer::GetDevice(void)
{
  return m_pDevice;
}


///////////////////////////////////////////////////////////////////////////////
// �f�o�C�X�̃Q�b�^�[
ID3D11DeviceContext* D3D11Renderer::GetDeviceContext(void)
{
  return m_pDeviceContext;
}

