//=============================================================================
//
// 入力処理 [input.cpp]
// Author : AKIRA TANAKA
//
//=============================================================================
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	COUNT_WAIT_REPEAT	(20)	// リピート待ち時間

//*****************************************************************************
// 静的変数
//*****************************************************************************
LPDIRECTINPUT8	CInput::m_pDInput = NULL;	// DirectInputオブジェクト

//=============================================================================
// CInputコンストラスタ
//=============================================================================
CInput::CInput()
{
	m_pDIDevice = NULL;
}

//=============================================================================
// CInputデストラスタ
//=============================================================================
CInput::~CInput()
{
  if (m_pDIDevice)
  {// デバイスオブジェクトの開放
    m_pDIDevice->Unacquire();

    m_pDIDevice->Release();
    m_pDIDevice = NULL;
  }

  if (m_pDInput)
  {// DirectInputオブジェクトの開放
    m_pDInput->Release();
    m_pDInput = NULL;
  }
}

//=============================================================================
// 入力処理の初期化処理
//=============================================================================
HRESULT CInput::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr = S_FALSE;

	// DirectInputオブジェクトの作成
	if(!m_pDInput)
	{
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&m_pDInput, NULL);
	}

	return hr;
}

//=============================================================================
// 入力処理の更新処理
//=============================================================================
#if 0	// ---> 純粋仮想関数化
HRESULT CInput::Update(void)
{
	return S_OK;
}
#endif

//=============================================================================
// CInputKeyboardコンストラスタ
//=============================================================================
CInputKeyboard::CInputKeyboard()
{
	// 各ワークのクリア
	ZeroMemory(m_aKeyState, sizeof m_aKeyState);
	ZeroMemory(m_aKeyStateTrigger, sizeof m_aKeyStateTrigger);
	ZeroMemory(m_aKeyStateRelease, sizeof m_aKeyStateRelease);
	ZeroMemory(m_aKeyStateRepeat, sizeof m_aKeyStateRepeat);
	ZeroMemory(m_aKeyStateRepeatCnt, sizeof m_aKeyStateRepeatCnt);
}

//=============================================================================
// CInputKeyboardデストラスタ
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{
}

//=============================================================================
// キーボードの初期化処理
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// 入力処理の初期化
	CInput::Init(hInst, hWnd);

	// デバイスオブジェクトを作成
	hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, NULL);
	if(FAILED(hr) || m_pDIDevice == NULL)
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = m_pDIDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// m_pDIDeviceアクセス権を獲得(入力制御開始)
	m_pDIDevice->Acquire();

	return hr;
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
void CInputKeyboard::Update(void)
{
	HRESULT hr;
	BYTE aKeyState[NUM_KEY_MAX];

	if(!m_pDIDevice)
	{
		return;
	}

	// デバイスからデータを取得
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
// キーボードデータ取得(プレス)
//=============================================================================
BOOL CInputKeyboard::GetKeyPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// キーボードデータ取得(トリガー)
//=============================================================================
BOOL CInputKeyboard::GetKeyTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// キーボードデータ取得(リリース)
//=============================================================================
BOOL CInputKeyboard::GetKeyRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// キーボードデータ取得(リピート)
//=============================================================================
BOOL CInputKeyboard::GetKeyRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// キーボードデータ削除(トリガー)
//=============================================================================
void CInputKeyboard::FlushKeyTrigger(int nKey)
{
	m_aKeyStateTrigger[nKey] = 0;
}

//=============================================================================
// マウスの初期化
//=============================================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力処理の初期化
	if (FAILED(CInput::Init(hInstance, hWnd)))
	{
		MessageBox(hWnd, "DirectInputオブジェクトが作れねぇ！", "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	// マウスのデバイスの作成
	if (FAILED(m_pDInput->CreateDevice(GUID_SysMouse, &m_pDIDevice, NULL)))
	{
		MessageBox(hWnd, "マウスがねぇ！", "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	// マウスのデータフォーマットを設定
	if (FAILED(m_pDIDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		MessageBox(hWnd, "マウスのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	if (FAILED(m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		MessageBox(hWnd, "マウスの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	m_pDIDevice->Acquire();
	//何回も取る必要あるかも

	return S_OK;
}

//=============================================================================
// マウスの更新処理
//=============================================================================
void CInputMouse::Update(void)
{
	DIMOUSESTATE2 dims;

	if (SUCCEEDED(m_pDIDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &dims)))
	{
		//キートリガーリリース情報の作成
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
		// マウスへのアクセス権を取得
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// マウス左クリックのプレス状態を取得
//=============================================================================
bool CInputMouse::GetLeftClickPress(void)
{
	return (m_aMouseLeftClick & 0x80) ? true : false;
}

//=============================================================================
// マウス左クリックのトリガー状態を取得
//=============================================================================
bool CInputMouse::GetLeftClickTrigger(void)
{
	return (m_aMouseLeftClickTrigger & 0x80) ? true : false;
}

//=============================================================================
// マウス左クリックのリリ－ス状態を取得
//=============================================================================
bool CInputMouse::GetLeftClickRelease(void)
{
	return (m_aMouseLeftClickRelease & 0x80) ? true : false;
}
//=============================================================================
// マウス右クリックのプレス状態を取得
//=============================================================================
bool CInputMouse::GetRightClickPress(void)
{
	return (m_aMouseRightClick & 0x80) ? true : false;
}

//=============================================================================
// マウス右クリックのトリガー状態を取得
//=============================================================================
bool CInputMouse::GetRightClickTrigger(void)
{
	return (m_aMouseRightClickTrigger & 0x80) ? true : false;
}

//=============================================================================
// マウス右クリックのリリ－ス状態を取得
//=============================================================================
bool CInputMouse::GetRightClickRelease(void)
{
	return (m_aMouseRightClickRelease & 0x80) ? true : false;
}

//*****************************************************************************
//	マウスX座標の移動量取得
//*****************************************************************************
int CInputMouse::GetMouseMoveX(void)
{
	return m_lX;
}

//*****************************************************************************
//	マウス座標の移動量取得
//*****************************************************************************
int CInputMouse::GetMouseMoveY(void)
{
	return m_lY;
}

//*****************************************************************************
//	マウスホイールの移動量取得( +が上、-が下、0が動いてない)
//*****************************************************************************
int CInputMouse::GetMouseWheel(void)
{
	return m_lZ;
}

