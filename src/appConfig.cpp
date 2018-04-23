//=============================================================================
// File : appConfig.cpp
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "appConfig.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
AppConfig::AppConfig()
{
  std::unordered_map<std::string, void*> token;
  AddToken( token);
  m_screenWidth = 1280;
  m_screenHeight = 720;
  m_far = 5000.0f;
  m_near = 0.01f;
}


///////////////////////////////////////////////////////////////////////////////
// �g�[�N���ǉ�
void AppConfig::AddToken(std::unordered_map<std::string, void*>& token)
{
  token["ScreenWidth"]  = &m_screenWidth;
  token["ScreenHeihgt"] = &m_screenHeight;
  token["Far"]          = &m_far;
  token["Near"]         = &m_near;
}