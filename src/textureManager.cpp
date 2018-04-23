///////////////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
#include "appManager.h"
#include "textureManager.h"
#include "renderer.h"
#include "Texture.h"


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
TextureManager::~TextureManager()
{
  AllRelease();
}

///////////////////////////////////////////////////////////////////////////////
// �摜�ǂݍ���
bool TextureManager::Load(std::wstring& key)
{
  // �e�N�X�`��������
  std::unordered_map<std::wstring, Res>::iterator it = m_res.find(key);
  if (it == m_res.end()) {            // ������Ȃ������ꍇ
    Texture* texture = new Texture;

    // �g���q�̎��o��
    int ext_i = key.find_last_of(L".");
    std::wstring extname = key.substr(ext_i, key.size() - ext_i);

    std::shared_ptr<imageutil::Image> image;

    // TGA�t�@�C��������O�ǂݍ���
    if (extname == L".tga")
    {
      auto tgaFactory = std::make_shared<imageutil::TgaFactory>();
      image = tgaFactory->Load(key);
    }
    else // ���̑��t�@�C���ǂݍ���
    {
      auto wicFactory = std::make_shared<imageutil::Factory>();
      image = wicFactory->Load(key);
    }

    // ���[�h�`�F�b�N
    char buf[512];
    sprintf(buf, "FailedLoadTexture!\nFilePath : %ls", key.c_str());
    if (!image){
      MessageBox(NULL, buf, "LoadError!", MB_OK);
      delete texture;
      return false;
    }

    // �e�N�X�`���I�u�W�F�N�g����
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
//	�摜�Q�b�^�[
Texture* TextureManager::GetResource(const std::wstring& key)
{
  // �e�N�X�`��������
  std::unordered_map<std::wstring, Res>::iterator it = m_res.find(key);
  if (it != m_res.end()) {       // ���������ꍇ
    return it->second.tex;
  }
  else return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
//	�摜�̊J��
bool TextureManager::Release(std::wstring key)
{
  // �e�N�X�`��������
  std::unordered_map<std::wstring, Res>::iterator itr = m_res.find(key);
  if (itr == m_res.end()) return false;
  if ((itr->second.nCnt--) == 0) {       // ���������ꍇ
    delete itr->second.tex;
    m_res.erase(itr);
    return true;
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//	�S�Ẳ摜�̊J��
bool TextureManager::AllRelease()
{
  for (auto&& res : m_res) {
    delete res.second.tex;
  }
  m_res.clear();
  return true;
}
