//=============================================================================
//
// ���͏��� [input.cpp]
// Author : AKIRA TANAKA
//
//=============================================================================
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	COUNT_WAIT_REPEAT	(20)	// ���s�[�g�҂�����

//*****************************************************************************
// �ÓI�ϐ�
//*****************************************************************************
LPDIRECTINPUT8	CInput::m_pDInput = NULL;	// DirectInput�I�u�W�F�N�g

//=============================================================================
// CInput�R���X�g���X�^
//=============================================================================
CInput::CInput()
{
	m_pDIDevice = NULL;
}

//=============================================================================
// CInput�f�X�g���X�^
//=============================================================================
CInput::~CInput()
{
  if (m_pDIDevice)
  {// �f�o�C�X�I�u�W�F�N�g�̊J��
    m_pDIDevice->Unacquire();

    m_pDIDevice->Release();
    m_pDIDevice = NULL;
  }

  if (m_pDInput)
  {// DirectInput�I�u�W�F�N�g�̊J��
    m_pDInput->Release();
    m_pDInput = NULL;
  }
}

//=============================================================================
// ���͏����̏���������
//=============================================================================
HRESULT CInput::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr = S_FALSE;

	// DirectInput�I�u�W�F�N�g�̍쐬
	if(!m_pDInput)
	{
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&m_pDInput, NULL);
	}

	return hr;
}

//=============================================================================
// ���͏����̍X�V����
//=============================================================================
#if 0	// ---> �������z�֐���
HRESULT CInput::Update(void)
{
	return S_OK;
}
#endif

//=============================================================================
// CInputKeyboard�R���X�g���X�^
//=============================================================================
CInputKeyboard::CInputKeyboard()
{
	// �e���[�N�̃N���A
	ZeroMemory(m_aKeyState, sizeof m_aKeyState);
	ZeroMemory(m_aKeyStateTrigger, sizeof m_aKeyStateTrigger);
	ZeroMemory(m_aKeyStateRelease, sizeof m_aKeyStateRelease);
	ZeroMemory(m_aKeyStateRepeat, sizeof m_aKeyStateRepeat);
	ZeroMemory(m_aKeyStateRepeatCnt, sizeof m_aKeyStateRepeatCnt);
}

//=============================================================================
// CInputKeyboard�f�X�g���X�^
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{
}

//=============================================================================
// �L�[�{�[�h�̏���������
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// ���͏����̏�����
	CInput::Init(hInst, hWnd);

	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, NULL);
	if(FAILED(hr) || m_pDIDevice == NULL)
	{
		MessageBox(hWnd, "�L�[�{�[�h���˂��I", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = m_pDIDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// m_pDIDevice�A�N�Z�X�����l��(���͐���J�n)
	m_pDIDevice->Acquire();

	return hr;
}

//=============================================================================
// �L�[�{�[�h�̍X�V����
//=============================================================================
void CInputKeyboard::Update(void)
{
	HRESULT hr;
	BYTE aKeyState[NUM_KEY_MAX];

	if(!m_pDIDevice)
	{
		return;
	}

	// �f�o�C�X����f�[�^���擾
	hr = m_pDIDevice->GetDeviceState(sizeof(aKeyState), aKeyState);
	if(SUCCEEDED(hr))
	{
		for(int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & ~aKeyState[nCntKey];
			m_aKeyStateRepeat[nCntKey] = m_aKeyStateTrigger[nCntKey];

			if(aKeyState[nCntKey])
			{
				m_aKeyStateRepeatCnt[nCntKey]++;
				if(m_aKeyStateRepeatCnt[nCntKey] >= COUNT_WAIT_REPEAT)
				{
					m_aKeyStateRepeat[nCntKey] = aKeyState[nCntKey];
				}
			}
			else
			{
				m_aKeyStateRepeatCnt[nCntKey] = 0;
				m_aKeyStateRepeat[nCntKey] = 0;
			}

			m_aKeyState[nCntKey] = aKeyState[nCntKey];
		}
	}
	else
	{
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(�v���X)
//=============================================================================
BOOL CInputKeyboard::GetKeyPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(�g���K�[)
//=============================================================================
BOOL CInputKeyboard::GetKeyTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(�����[�X)
//=============================================================================
BOOL CInputKeyboard::GetKeyRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(���s�[�g)
//=============================================================================
BOOL CInputKeyboard::GetKeyRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�폜(�g���K�[)
//=============================================================================
void CInputKeyboard::FlushKeyTrigger(int nKey)
{
	m_aKeyStateTrigger[nKey] = 0;
}

//=============================================================================
// �}�E�X�̏�����
//=============================================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	// ���͏����̏�����
	if (FAILED(CInput::Init(hInstance, hWnd)))
	{
		MessageBox(hWnd, "DirectInput�I�u�W�F�N�g�����˂��I", "�x���I", MB_ICONWARNING);
		return E_FAIL;
	}

	// �}�E�X�̃f�o�C�X�̍쐬
	if (FAILED(m_pDInput->CreateDevice(GUID_SysMouse, &m_pDIDevice, NULL)))
	{
		MessageBox(hWnd, "�}�E�X���˂��I", "�x���I", MB_ICONWARNING);
		return E_FAIL;
	}

	// �}�E�X�̃f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(m_pDIDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		MessageBox(hWnd, "�}�E�X�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return E_FAIL;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	if (FAILED(m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		MessageBox(hWnd, "�}�E�X�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return E_FAIL;
	}

	// �L�[�{�[�h�ւ̃A�N�Z�X�����l��(���͐���J�n)
	m_pDIDevice->Acquire();
	//��������K�v���邩��

	return S_OK;
}

//=============================================================================
// �}�E�X�̍X�V����
//=============================================================================
void CInputMouse::Update(void)
{
	DIMOUSESTATE2 dims;

	if (SUCCEEDED(m_pDIDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &dims)))
	{
		//�L�[�g���K�[�����[�X���̍쐬
		m_aMouseLeftClickTrigger = (m_aMouseLeftClick^dims.rgbButtons[0]) & dims.rgbButtons[0];
		m_aMouseLeftClickRelease = (m_aMouseLeftClick^dims.rgbButtons[0]) & m_aMouseLeftClick;
		m_aMouseLeftClick = dims.rgbButtons[0];

		m_aMouseRightClickTrigger = (m_aMouseRightClick^dims.rgbButtons[1]) & dims.rgbButtons[1];
		m_aMouseRightClickRelease = (m_aMouseRightClick^dims.rgbButtons[1]) & m_aMouseRightClick;
		m_aMouseRightClick = dims.rgbButtons[1];
		m_lX = dims.lX;
		m_lY = dims.lY;
		m_lZ = dims.lZ;
	}
	else
	{
		// �}�E�X�ւ̃A�N�Z�X�����擾
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// �}�E�X���N���b�N�̃v���X��Ԃ��擾
//=============================================================================
bool CInputMouse::GetLeftClickPress(void)
{
	return (m_aMouseLeftClick & 0x80) ? true : false;
}

//=============================================================================
// �}�E�X���N���b�N�̃g���K�[��Ԃ��擾
//=============================================================================
bool CInputMouse::GetLeftClickTrigger(void)
{
	return (m_aMouseLeftClickTrigger & 0x80) ? true : false;
}

//=============================================================================
// �}�E�X���N���b�N�̃����|�X��Ԃ��擾
//=============================================================================
bool CInputMouse::GetLeftClickRelease(void)
{
	return (m_aMouseLeftClickRelease & 0x80) ? true : false;
}
//=============================================================================
// �}�E�X�E�N���b�N�̃v���X��Ԃ��擾
//=============================================================================
bool CInputMouse::GetRightClickPress(void)
{
	return (m_aMouseRightClick & 0x80) ? true : false;
}

//=============================================================================
// �}�E�X�E�N���b�N�̃g���K�[��Ԃ��擾
//=============================================================================
bool CInputMouse::GetRightClickTrigger(void)
{
	return (m_aMouseRightClickTrigger & 0x80) ? true : false;
}

//=============================================================================
// �}�E�X�E�N���b�N�̃����|�X��Ԃ��擾
//=============================================================================
bool CInputMouse::GetRightClickRelease(void)
{
	return (m_aMouseRightClickRelease & 0x80) ? true : false;
}

//*****************************************************************************
//	�}�E�XX���W�̈ړ��ʎ擾
//*****************************************************************************
int CInputMouse::GetMouseMoveX(void)
{
	return m_lX;
}

//*****************************************************************************
//	�}�E�X���W�̈ړ��ʎ擾
//*****************************************************************************
int CInputMouse::GetMouseMoveY(void)
{
	return m_lY;
}

//*****************************************************************************
//	�}�E�X�z�C�[���̈ړ��ʎ擾( +����A-�����A0�������ĂȂ�)
//*****************************************************************************
int CInputMouse::GetMouseWheel(void)
{
	return m_lZ;
}

