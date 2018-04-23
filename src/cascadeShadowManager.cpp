//=============================================================================
// File : cascadeShadowManager.cpp
// Date : 2018/02/23(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <Windows.h>
#include "cascadeShadowManager.h"
#include "appManager.h"
#include "shaderManager.h"
#include "shader.h"
#include "renderer.h"
#include "renderTexture.h"
#include "camera.h"
#include "cameraMan.h"
#include "sun.h"
#include "imgui_impl_dx11.h"


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
CascadeShadowManager::CascadeShadowManager()
{
  // �[�x�l�p�e�N�X�`������
  for (int i = 0; i < NumDivision; i++) {
    m_ZBuffer[i] = new RenderTexture(ShadowMapSize, ShadowMapSize, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT);
  }
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
CascadeShadowManager::~CascadeShadowManager()
{
  for (int i = 0; i < NumDivision; i++) {
    if (m_ZBuffer[i]) delete m_ZBuffer[i];
  }
}


///////////////////////////////////////////////////////////////////////////////
// �����_�����O
void CascadeShadowManager::Draw()
{
  // �[�x�l�`��͈͎w��
  ComputeOrthoProj();

  // �[�x�l�`��
  RenderDepth();

  // �I�u�W�F�N�g�̕`��
  RenderObject();
}


///////////////////////////////////////////////////////////////////////////////
// �f�v�X�o�b�t�@�̕`��͈͎w��
void CascadeShadowManager::ComputeOrthoProj(void)
{
  CCamera c = *CCameraMan::GetInstance()->GetCamera();
  float fov = c.GetFov();
  float aspect = c.GetAspect();
  float cFar = c.GetFar();
  float cNear = c.GetNear();
  Vector3 vecView = Vec3(*c.GetCameraZ());
  DirectX::XMVECTOR Xziku = *c.GetCameraX();
  DirectX::XMVECTOR Yziku = *c.GetCameraY();
  Vector3 posView = Vec3(c.GetPos());
  DirectX::XMMATRIX view = m_sun->GetViewMatrix();
  Vector3 posSun = m_sun->GetTransform()->GetPosition();
  Vector3 vecSun = m_sun->GetLightVec();

  bool b = true;
  ImGui::Begin("View", &b);
  ImGui::Text("[ CameraPos ]  x : %f,  y : %f,  z : %f", posView.x, posView.y, posView.z);
  ImGui::Text("[ CameraVec ]  x : %f,  y : %f,  z : %f", vecView.x, vecView.y, vecView.z);
  ImGui::Text("[ SunPos ]     x : %f,  y : %f,  z : %f", posSun.x, posSun.y, posSun.z);
  ImGui::Text("[ SunVec ]     x : %f,  y : %f,  z : %f", vecSun.x, vecSun.y, vecSun.z);
  ImGui::End();

  for (int i = 0; i < NumDivision; i++)
  {
    // ������̒��_��8�_�i�[
    std::vector<Vector3> Points;
    
    // ������̎�O4�_������o��
    Vector3 vecDivisionView = vecView*(cFar*farTable[i]);                       // v1(���S)�̃x�N�g��
    DirectX::XMMATRIX workHeight = DirectX::XMMatrixRotationAxis(Xziku, fov*0.5f);
    Vector3 topVec = Vec3(XMVector3TransformCoord(XM(vecDivisionView), workHeight));
    topVec /= cos(fov*0.5f);
    DirectX::XMMATRIX workLeft = DirectX::XMMatrixRotationAxis(Yziku, fov*0.5f*aspect);
    Vector3 leftVec = Vec3(XMVector3TransformCoord(XM(vecDivisionView), workLeft));
    leftVec /= cos(fov*0.5f*aspect);

    Vector3 vecWidth = leftVec - vecDivisionView;     // ���S���獶�ւ̒���
    Vector3 vecHeight = topVec - vecDivisionView;     // ���S�����ւ̒���
    Points.push_back(posView + vecDivisionView + vecWidth + vecHeight); // Near�̍���
    Points.push_back(posView + vecDivisionView - vecWidth + vecHeight); // Near�̉E��
    Points.push_back(posView + vecDivisionView + vecWidth - vecHeight); // Near�̍���
    Points.push_back(posView + vecDivisionView - vecWidth - vecHeight); // Near�̉E��

    // ������̉�4�_������o��
    vecDivisionView = vecView*(cFar*farTable[i+1]);                       // v1(���S)�̃x�N�g��
    workHeight = DirectX::XMMatrixRotationAxis(Xziku, fov*0.5f);
    topVec = Vec3(XMVector3TransformCoord(XM(vecDivisionView), workHeight));
    topVec /= cos(fov*0.5f);
    workLeft = DirectX::XMMatrixRotationAxis(Yziku, fov*0.5f*aspect);
    leftVec = Vec3(XMVector3TransformCoord(XM(vecDivisionView), workLeft));
    leftVec /= cos(fov*0.5f*aspect);

    vecWidth = leftVec - vecDivisionView;     // ���S���獶�ւ̒���
    vecHeight = topVec - vecDivisionView;     // ���S�����ւ̒���
    Points.push_back(posView + vecDivisionView + vecWidth + vecHeight); // Near�̍���
    Points.push_back(posView + vecDivisionView - vecWidth + vecHeight); // Near�̉E��
    Points.push_back(posView + vecDivisionView + vecWidth - vecHeight); // Near�̍���
    Points.push_back(posView + vecDivisionView - vecWidth - vecHeight); // Near�̍���

    // ���z�̃r���[���W�Ŏ�����̃o�E���f�B���O�{�b�N�X�I�Ȃ��̂���遗�B��
    float left, right, top, bottom, Near, Far;
    { // minmax�̒��덇�킹�i0�N���A���ƕs���ɂȂ肤��j
      Vector3 initPoint = Vec3(DirectX::XMVector3TransformCoord(XM(*Points.begin()), view));
      left = initPoint.x;
      right = initPoint.x;
      top = initPoint.y;
      bottom = initPoint.y;
      Far = 0;
    }
    int pCnt = 0;
    for (auto&& point : Points) {
      ImGui::Begin("DivisionPerspectiveElements", &b);
      ImGui::Text("point[%d][%d]  x : %f, y : %f, z : %f", i, pCnt, point.x, point.y, point.z);
      ImGui::End();
      pCnt++;

      Vector3 pointV = Vec3( DirectX::XMVector3TransformCoord( XM(point), view));
      left = min( pointV.x, left);
      right = max( pointV.x, right);
      top = max( pointV.y, top);
      bottom = min( pointV.y, bottom);
      Far = max( pointV.z, Far);
    }
    ImGui::Begin("OrthoElements", &b);
    ImGui::Text("Ortho[%d]  Left : %f, Right : %f, Top : %f, Bottom : %f, Far : %f", i, left, right, top, bottom, Far);
    ImGui::End();
    // �����䂪���܂鐳�ˉe�s�񐶐�
    DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, 0.01f, Far);
    m_camera[i].SetProjMtx( proj);
    m_camera[i].SetFar(Far);
    m_camera[i].SetNear(0.01f);
  }
}


///////////////////////////////////////////////////////////////////////////////
// �[�x�l�`��
void CascadeShadowManager::RenderDepth(void)
{
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();
  float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  // ViewPort�̐ݒ�
  D3D11_VIEWPORT viewport;
  D3D11_VIEWPORT oldViewport;
  UINT numViewPort = 1;
  pDeviceContext->RSGetViewports(&numViewPort, &oldViewport);
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;
  viewport.Width = ShadowMapSize;
  viewport.Height = ShadowMapSize;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  pDeviceContext->RSSetViewports(1, &viewport);

  // �[�x�`��
  for (int i = 0; i < NumDivision; i++) {
    m_camera[i].SetViewMtx(m_sun->GetViewMatrix());
    pDeviceContext->OMSetRenderTargets(1, m_ZBuffer[i]->GetView().GetAddressOf(), *m_ZBuffer[i]->GetDepthView().GetAddressOf());
    pDeviceContext->ClearRenderTargetView(*m_ZBuffer[i]->GetView().GetAddressOf(), clearColor);
    pDeviceContext->ClearDepthStencilView(*m_ZBuffer[i]->GetDepthView().GetAddressOf(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    AppManager::GetShaderManager()->SetCascadeDepthShader(*m_camera[i].GetProjMtx(), m_sun->GetViewMatrix());
    AppManager::GetRenderer()->DrawLayer(&m_camera[i], D3D11Renderer::eLayerNormalObject, D3D11Renderer::eLayerTranslucentObject);

    bool b = true;
    ImGui::Begin("Shadow Map", &b);
    auto list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    p.x = p.x + 105 * i;
    list->AddImage(*m_ZBuffer[i]->GetSRView().GetAddressOf(), p, ImVec2(p.x + 100, p.y+100), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
    ImGui::End();

    Vector3 pView = Vector3(0.0f, 0.0f, 0.0f);
    pView = Vec3(DirectX::XMVector3TransformCoord(XM(pView), m_sun->GetViewMatrix()));
    Vector3 pViewProj = Vector3(0.0f, 0.0f, 0.0f);
    DirectX::XMMATRIX proj = *m_camera[i].GetProjMtx();
    pViewProj = Vec3(DirectX::XMVector3TransformCoord(XM(pView), proj));
    ImGui::Begin("Sample", &b);
    ImGui::Text("ViewPosition[%d]  : %f, %f, %f", i, pView.x, pView.y, pView.z);
    ImGui::Text("OrthoPosition[%d] : %f, %f, %f", i, pViewProj.x, pViewProj.y, pViewProj.z);
    ImGui::End();

    ImGui::Begin("ProjectionMatrix", &b);
    ImGui::Text("sunProj[%d]\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f", i, DirectX::XMVectorGetX(proj.r[0]), DirectX::XMVectorGetY(proj.r[0]), DirectX::XMVectorGetZ(proj.r[0]), DirectX::XMVectorGetW(proj.r[0]),
      DirectX::XMVectorGetX(proj.r[1]), DirectX::XMVectorGetY(proj.r[1]), DirectX::XMVectorGetZ(proj.r[1]), DirectX::XMVectorGetW(proj.r[1]),
      DirectX::XMVectorGetX(proj.r[2]), DirectX::XMVectorGetY(proj.r[2]), DirectX::XMVectorGetZ(proj.r[2]), DirectX::XMVectorGetW(proj.r[2]),
      DirectX::XMVectorGetX(proj.r[3]), DirectX::XMVectorGetY(proj.r[3]), DirectX::XMVectorGetZ(proj.r[3]), DirectX::XMVectorGetW(proj.r[3]));
    ImGui::End();
  }

  // ViewPort�����ɖ߂�
  pDeviceContext->RSSetViewports(1, &oldViewport);
}


///////////////////////////////////////////////////////////////////////////////
// ���`��
void CascadeShadowManager::RenderObject(void)
{
  D3D11Renderer* renderer = AppManager::GetRenderer();
  renderer->GetDeviceContext()->OMSetRenderTargets(1, renderer->GetRenderTargetView(), *renderer->GetDepthStencilView());
  DirectX::XMMATRIX proj[NumDivision];
  for (int i = 0; i < NumDivision; i++) proj[i] = *m_camera[i].GetProjMtx();
  AppManager::GetShaderManager()->SetCascadeShader(&proj[0], &m_sun->GetViewMatrix(), &m_ZBuffer[0]);
  AppManager::GetRenderer()->DrawAll(CCameraMan::GetInstance()->GetCamera());
}
