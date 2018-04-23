///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
#include "appManager.h"
#include "textureManager.h"
#include "renderer.h"
#include "Texture.h"


///////////////////////////////////////////////////////////////////////////////
// デストラクタ
TextureManager::~TextureManager()
{
  AllRelease();
}

///////////////////////////////////////////////////////////////////////////////
// 画像読み込み
bool TextureManager::Load(std::wstring& key)
{
  // テクスチャを検索
  std::unordered_map<std::wstring, Res>::iterator it = m_res.find(key);
  if (it == m_res.end()) {            // 見つからなかった場合
    Texture* texture = new Texture;

    // 拡張子の取り出し
    int ext_i = key.find_last_of(L".");
    std::wstring extname = key.substr(ext_i, key.size() - ext_i);

    std::shared_ptr<imageutil::Image> image;

    // TGAファイルだけ例外読み込み
    if (extname == L".tga")
    {
      auto tgaFactory = std::make_shared<imageutil::TgaFactory>();
      image = tgaFactory->Load(key);
    }
    else // その他ファイル読み込み
    {
      auto wicFactory = std::make_shared<imageutil::Factory>();
      image = wicFactory->Load(key);
    }

    // ロードチェック
    char buf[512];
    sprintf(buf, "FailedLoadTexture!\nFilePath : %ls", key.c_str());
    if (!image){
      MessageBox(NULL, buf, "LoadError!", MB_OK);
      delete texture;
      return false;
    }

    // テクスチャオブジェクト生成
    if(!texture->Initialize(AppManager::GetRenderer()->GetDevice(), image)){
      MessageBox(NULL, buf, "InitializeError!", MB_OK);
      delete texture;
      return false;
    }
    Res res;
    res.nCnt = 1;
    res.tex = texture;
    m_res[key] = res;
    return true;
  }
  it->second.nCnt += 1;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//	画像ゲッター
Texture* TextureManager::GetResource(const std::wstring& key)
{
  // テクスチャを検索
  std::unordered_map<std::wstring, Res>::iterator it = m_res.find(key);
  if (it != m_res.end()) {       // 見つかった場合
    return it->second.tex;
  }
  else return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
//	画像の開放
bool TextureManager::Release(std::wstring key)
{
  // テクスチャを検索
  std::unordered_map<std::wstring, Res>::iterator itr = m_res.find(key);
  if (itr == m_res.end()) return false;
  if ((itr->second.nCnt--) == 0) {       // 見つかった場合
    delete itr->second.tex;
    m_res.erase(itr);
    return true;
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//	全ての画像の開放
bool TextureManager::AllRelease()
{
  for (auto&& res : m_res) {
    delete res.second.tex;
  }
  m_res.clear();
  return true;
}
