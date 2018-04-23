#pragma once
//=============================================================================
// File : cameraMan.h
// Date : 2017/12/06(êÖ)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// ÉCÉìÉNÉãÅ[ÉhÉtÉ@ÉCÉã
#include <DirectXMath.h>

///////////////////////////////////////////////////////////////////////////////
// ëOï˚êÈåæ
class CCamera;

class CCameraMan
{
 private:
  CCameraMan() :m_pCamera(nullptr), m_movelate(5.0f), m_rotatelate(0.05f) {}
  CCameraMan(const CCameraMan& ref) {}
  CCameraMan& operator=(const CCameraMan& ref) {}

  CCamera* m_pCamera;
  float m_movelate;
  float m_rotatelate;

 protected:
  ///	ÇmÇnÇsÇgÇhÇmÇf	///

 public:
   static CCameraMan* GetInstance() {
     static CCameraMan ins;
     return &ins;
   }
  ~CCameraMan() {};
  void Set();
  void Update();
  void CameraSet(CCamera* pCamera);
  void SetVPMtx();
  void CameraOperation();
  CCamera* GetCamera() { return m_pCamera; }

  DirectX::XMMATRIX* GetProjMtx(void);
  void SetProjMtx(float width, float height, float Near);
  DirectX::XMMATRIX* GetViewMtx(void);
  DirectX::XMVECTOR* GetCameraZ(void);
  DirectX::XMVECTOR* GetCameraX(void);

  void ParallelShiftCameraXYZ(DirectX::XMVECTOR *VecDirect, float movelate);
  void ParallelShiftCameraXZ(DirectX::XMVECTOR *VecDirect, float movelate);
  void SpiralCameraUp(float rotatelate);
  void SpiralCameraDown(float rotatelate);
  void SpiralCameraSide(float rotatelate);
  void TurningCameraUp(float rotatelate);
  void TurningCameraDown(float rotatelate);
  void TurningCameraSide(float rotatelate);
};