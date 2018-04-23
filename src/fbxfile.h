#pragma once
//=============================================================================
// File : fbxfile.h
// Date : 2017/12/13(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include <vector>
#include <list>
#include "Vector.h"
#include <unordered_map>
#include "gameObject.h"


class FBXFile;
class FBXFile2;
class CCamera;

namespace FBXStruct
{
  struct Vertex
  {
    Vector3 pos;
    Vector3 nrm;
    Vector2 tex;
    float weight[4];
    unsigned int boneIndex[4];
  };

  struct RegidVertex
  {
    Vector3 pos;
    Vector3 nrm;
    Vector2 tex;
  };

  struct Matrix
  {
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
  };
}
class BornRefarence
{
 public:
  BornRefarence(unsigned char index, float weight) : index(index), weight(weight) {}
  unsigned char index;
  float weight;
};

class Point
{
 public:
  Point(const Vector3& positions) : positions(positions) {}
  Vector3 positions;
  std::vector<BornRefarence> bornRefarences;
};


struct MESH_FBX
{
 public:
  MESH_FBX() :materialIndex(-1) {}
  std::vector<Point> points;
  std::vector<Vector3> normals;
  std::vector<Vector2> texcoords;

  std::vector<unsigned short> positionIndices;
  std::vector<unsigned short> normalIndices;
  std::vector<unsigned short> texcoordIndices;

  int materialIndex;

  void CreateVertexBuffer();

  ID3D11Buffer* vertexBuffer;

};

class MY_NODE
{
 public:
  std::list <MY_NODE*> children;
  std::string name;
  std::vector< std::string> attributeNames;
  Vector3 translation;
  Vector3 rotation;
  Vector3 scaling;

  std::vector<std::wstring> textures;
  std::vector<MESH_FBX> meshes;

  static MY_NODE* recursiveNode(FbxManager* pManager, FbxNode* pNode, bool bTexcoordVReverse, FBXFile* uniqueKun);
  void recursiveDraw(const DirectX::XMMATRIX& mtxWorldParent, ID3D11Buffer* matCB, ID3D11Buffer* boneCB);
 private:
  DirectX::XMMATRIX invMtx;

  static std::string GetAttributeTypeName(FbxNodeAttribute::EType type);
  void analyzePosition(FbxMesh* mesh);
  void analyzeNormal(FbxMesh* mesh);
  void analyzeTexcoord(FbxMesh* mesh, bool bRevers = false);
  void analyzeMaterial(FbxMesh* mesh);
  void analyzeMaterial(FbxNode* node);
  void analyzeCluster(FbxMesh* mesh, FBXFile* uniqueKun);
  FbxAMatrix GetGeometry(FbxNode* node);
};

class FBXFile : public GameObject
{
 public:
  //*< public Method  >*//
  FBXFile(const std::string fileName) { m_fileName = fileName; m_bTexcoordVReverse = true; }
  FBXFile(const FBXFile &fbx): m_fileName( fbx.m_fileName){ }
  ~FBXFile() {}
  HRESULT Init(void) ;
  void Uninit(void) ;
  void Update(void) ;
  void Draw(void);
  void Save(void);
  Vector3* GetPos() { return nullptr; }

  MY_NODE* FBXFile::getModel(void);

  //*< public Member Data  >*//
  //     N O T H I N G     //

  std::unordered_map< unsigned long, int> m_uniqueID;

 private:
  //*< private Method  >*//
  void GetFbxModel(void);
  HRESULT CreateShaders(void);
  HRESULT CreateConstantBuffer(void);

  //*< private Member Data  >*//
  bool m_bTexcoordVReverse;
  std::string m_fileName;
  MY_NODE* pTopMyNode;
  std::vector<std::vector<std::vector<DirectX::XMMATRIX>>> m_bones;
  
  int currentFrame;
  FbxTime oneFrameTime;
  std::vector<int> allTime;
  FbxTime startTime;
  FbxTime endTime;

  ID3D11VertexShader*    m_pVS;      // 頂点シェーダ
  ID3D11GeometryShader*  m_pGS;      // ジオメトリシェーダ
  ID3D11PixelShader*     m_pPS;      // ピクセルシェーダ
  ID3D11InputLayout*     m_pIL;      // 入力レイアウト
  ID3D11Buffer*          m_matCB;    // コンスタントバッファ
  ID3D11Buffer*          m_boneCB;   // コンスタントバッファ
  DirectX::XMMATRIX      m_world;    // ワールドマトリクス
  DirectX::XMMATRIX      m_view;     // ビューマトリクス
  DirectX::XMMATRIX      m_proj;     // プロジェクションマトリクス
};


struct MESH_FBX2
{
public:
  MESH_FBX2() :materialIndex(-1) {}
  std::vector<Point> points;
  std::vector<Vector3> normals;
  std::vector<Vector2> texcoords;

  std::vector<unsigned short> positionIndices;
  std::vector<unsigned short> normalIndices;
  std::vector<unsigned short> texcoordIndices;

  int materialIndex;

  void CreateVertexBuffer();
  void CreateRegidVertexBuffer();

  ID3D11Buffer* vertexBuffer;
};

class MY_NODE2
{
 public:
  std::list <MY_NODE2*> children;
  std::string name;
  std::vector< std::string> attributeNames;
  Vector3 translation;
  Vector3 rotation;
  Vector3 scaling;

  std::vector<std::wstring> textures;
  std::vector<MESH_FBX2> meshes;

  static MY_NODE2* recursiveNode(FbxManager* pManager, FbxNode* pNode, bool bTexcoordVReverse, FBXFile2* uniqueKun);
  void regidRecursiveDraw(CCamera* pCamera, const DirectX::XMMATRIX& mtxWorldParent);
  void recursiveDraw(CCamera* pCamera,const DirectX::XMMATRIX& mtxWorldParent, ID3D11Buffer* boneCB);

 private:
  DirectX::XMMATRIX invMtx;

  static std::string GetAttributeTypeName(FbxNodeAttribute::EType type);
  void analyzePosition(FbxMesh* mesh);
  void analyzeNormal(FbxMesh* mesh);
  void analyzeTexcoord(FbxMesh* mesh, bool bRevers = false);
  void analyzeMaterial(FbxMesh* mesh);
  void analyzeMaterial(FbxNode* node);
  void analyzeCluster(FbxMesh* mesh, FBXFile2* uniqueKun);
  FbxAMatrix GetGeometry(FbxNode* node);
};


class FBXFile2 : public GameObject
{
 public:
  //*< public Method  >*//
  FBXFile2(const std::string fileName) { m_fileName2 = fileName; m_bTexcoordVReverse = true; }
  ~FBXFile2() {}
  HRESULT Init(void);
  void Uninit(void);
  void Update(void);
  void Render(CCamera*);
  bool ViewingFrustumCulling(ViewingFrustum* frustum) override;
  void Save(void);
  Vector3* GetPos() { return nullptr; }

  MY_NODE* getModel(void);

  //*< public Member Data  >*//
  //     N O T H I N G     //

  std::unordered_map< unsigned long, int> m_uniqueID;

  std::vector<std::vector<std::vector<DirectX::XMMATRIX>>> m_bones;
private:
  //*< private Method  >*//
  void GetFbxModel(void);
  HRESULT CreateConstantBuffer(void);

  //*< private Member Data  >*//
  bool m_bTexcoordVReverse;
  std::string m_fileName2;
  MY_NODE2* pTopMyNode;
  ID3D11Buffer* m_boneCB;
  int currentFrame;
};
