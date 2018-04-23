#pragma once
//=============================================================================
// File : mode.h
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

// Mode���N���X
class CMode
{
 public:
  virtual void Init(void) = 0;
  virtual void Update(void) = 0;
  virtual void Draw(void) = 0;
};

// ���[�h�Ǘ��N���X
class ModeContext
{
 public:
  ModeContext() :m_next(nullptr), m_mode(nullptr) {}
  void update(void) {
    if (!m_mode) return;
    m_mode->Update();
  }

  void draw(void) {
    if (m_mode) {
      m_mode->Draw();
    }
    NextMode();
  }

  void NextMode(void){
    if (m_mode == m_next) return;
    if( !m_next) return;
    if( m_mode) delete m_mode;
    m_next->Init();
    m_mode = m_next;
    m_next = nullptr;
  }

  void SetMode( CMode* mode){
    m_next = mode;
  }

  CMode* GetMode(void) {
    return m_mode;
  }

 private:
  CMode* m_next;
  CMode* m_mode;
};