#pragma once
//=============================================================================
// File : appConfig.h
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <unordered_map>


// �A�v���̐ݒ�l
class AppConfig
{
 public:
   ~AppConfig() {};
  static AppConfig* GetInstance() {
    static AppConfig ins;
    return &ins;
  }
  unsigned int& GetScreenWidth( void) { return m_screenWidth; }
  unsigned int& GetScreenHeight(void) { return m_screenHeight; }
  float& GetFar(void) { return m_far; }
  float& GetNear(void) { return m_near; }

 private:
  unsigned int m_screenWidth;
  unsigned int m_screenHeight;
  float m_far;
  float m_near;

  AppConfig();
  AppConfig(const AppConfig& ref) {};
  AppConfig& operator=(const AppConfig& ref) {}
  void AddToken(std::unordered_map<std::string, void*>& token);
};