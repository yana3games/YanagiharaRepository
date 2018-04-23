//=============================================================================
// File : fbxfile.h
// Date : 2017/12/13(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "appManager.h"
#include "fbxfile.h"
#include <algorithm>
#include "renderer.h"
#include "camera.h"
#include "Texture.h"
#include "textureManager.h"
#include "shaderManager.h"
#include "shader.h"


///////////////////////////////////////////////////////////////////////////////
//	FBXの初期化処理
HRESULT FBXFile2::Init(void)
{
  currentFrame = 0;
  //FBXモデルの読み込み
  GetFbxModel();

  HRESULT hr = CreateConstantBuffer();
  if (FAILED(hr)) return E_FAIL;

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// 定数バッファの作成
HRESULT FBXFile2::CreateConstantBuffer(void)
{
  // マトリクスバッファの設定.
  D3D11_BUFFER_DESC bd;
  // ボーンバッファの設定
  ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
  bd.ByteWidth = sizeof(DirectX::XMMATRIX) * 512;
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;

  // ボーンバッファを生成.
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateBuffer(&bd, NULL, &m_boneCB);
  if (FAILED(hr)) {
    MessageBox(NULL, "FailedCreateConstantBuffer", "error!", MB_OK);
    return E_FAIL;
  }
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//	FBXの終了処理
void FBXFile2::Uninit(void)
{
}


///////////////////////////////////////////////////////////////////////////////
//	FBXの更新処理
void FBXFile2::Update(void)
{
  currentFrame++;
}


///////////////////////////////////////////////////////////////////////////////
//	FBXの取得
void FBXFile2::GetFbxModel(void)
{
  // FBXマネージャーの生成
  FbxManager *pManager = FbxManager::Create();

  // OSの差とかを吸収してくれるみたい？
  FbxIOSettings *ios = FbxIOSettings::Create(pManager, IOSROOT);
  pManager->SetIOSettings(ios);

  // インポーターの生成
  FbxImporter *pImporter = FbxImporter::Create(pManager, "");

  //FBXファイルの読み込み
  const char *pFileName = m_fileName2.c_str();
  if (!pImporter->Initialize(pFileName))
  {
    MessageBox(NULL, "FBXファイルの読み込みに失敗しました！", "Error!!", MB_OK);
    pImporter->Destroy();
    ios->Destroy();
    pManager->Destroy();
    return;
  }

  //シーンオブジェクトの生成
  FbxScene *pScene = FbxScene::Create(pManager, "");  // 作成
  pImporter->Import(pScene);   // シーン作成

  // 三角分割
  FbxGeometryConverter con(pManager);
  con.Triangulate(pScene, true);//コピーを取りたければfalse;
  //縮退メッシュを削除
  //  con.RemoveBadPolygonsFromMeshes(pScene);
  // マテリアル毎にメッシュを分割
  con.SplitMeshesPerMaterial(pScene, true);

  int AnimStackCount = pImporter->GetAnimStackCount();
  //pScene->GetPoseCount();
  m_bones.resize(AnimStackCount);
  for (int animCnt = 0; animCnt < AnimStackCount; animCnt++)
  {
    FbxArray<FbxString*> animationNames;
    pScene->FillAnimStackNameArray(animationNames);

    FbxTakeInfo* pTakeInfo = pScene->GetTakeInfo(animationNames[0]->Buffer());

    FbxTime startTime = pTakeInfo->mLocalTimeSpan.GetStart();
    FbxTime endTime = pTakeInfo->mLocalTimeSpan.GetStop();
    FbxTime oneFrameTime;
    oneFrameTime.SetTime(0, 0, 0, 1, 0, 0, FbxTime::eFrames60);
    FbxTime allTime = (((int)endTime.Get() - (int)startTime.Get()) / (int)oneFrameTime.Get());

    pScene->SetCurrentAnimationStack((FbxAnimStack*)pScene->GetSrcObject(FbxCriteria::ObjectType(FbxAnimStack::ClassId), animCnt));

    int ClusterCnt = pScene->GetSrcObjectCount(FbxCriteria::ObjectType(FbxCluster::ClassId));
    for (int clusterNum = 0; clusterNum < ClusterCnt; clusterNum++)
    {
      m_bones[animCnt].emplace_back(0);

      FbxCluster* pCluster = (FbxCluster*)pScene->GetSrcObject(FbxCriteria::ObjectType(FbxCluster::ClassId), clusterNum);

      // 初期姿勢行列の取得
      FbxAMatrix lReferenceGlobalInitPosition;
      FbxAMatrix lReferenceGlobalCurrentPosition;
      FbxAMatrix lReferenceGeometry;
      FbxAMatrix lClusterGlobalInitPosition;
      FbxAMatrix lClusterGlobalCurrentPosition;
      FbxAMatrix lClusterRelativeInitPosition;
      FbxAMatrix lClusterRelativeCurrentPositionInverse;

      pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
      // lReferenceGlobalCurrentPosition = pGlobalPosition; // <- たぶんワールド座標変換行列ではないかと

      // Multiply lReferenceGlobalInitPosition by Geometric Transformation
    //  lReferenceGeometry = GetGeometry(mesh->GetNode());
   //   lReferenceGlobalInitPosition *= lReferenceGeometry;

      // Get the link initial global position and the link current global position.
      pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);

      unsigned long uniqueID = pCluster->GetUniqueID();
      m_uniqueID[uniqueID] = clusterNum;

      for (FbxTime currentTime = startTime; currentTime < endTime; currentTime += oneFrameTime) {
        // lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose); // <- ポーズ行列の取り方？？？
        lClusterGlobalCurrentPosition = pCluster->GetLink()->EvaluateGlobalTransform(currentTime);

        // Compute the initial position of the link relative to the reference.
        lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

        // Compute the current position of the link relative to the reference.
        lClusterRelativeCurrentPositionInverse = lClusterGlobalCurrentPosition;

        // Compute the shift of the link relative to the reference.
        FbxAMatrix VertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
        // ↑ 初期姿勢行列も考慮されたモーションボーン行列なので、これで頂点座標を変換するだけで良い 

        DirectX::XMMATRIX d3dMtx;
        float mat[4][4];
        for (int y = 0; y < 4; y++) {
          for (int x = 0; x < 4; x++) {
            mat[x][y] = (float)VertexTransformMatrix.Get(x, y);
          }
        }
        d3dMtx = DirectX::XMMatrixSet(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
        //d3dMtx = DirectX::XMMatrixTranspose(d3dMtx);
        m_bones[animCnt].back().push_back(d3dMtx);
        //allTime++;
      }
    }
  }
//
//  int meshCnt = pScene->GetMemberCount<FbxMesh>();
//  int mtrCnt = pScene->GetMaterialCount();
//
//  // Meshの取得
//  printf("[Info] Get Mesh...\n");
//  for (int i : range(meshCnt)) ParseMesh(pScene->GetMember<FbxMesh>(i));
//
//  // Materialの取得
//  printf("[Info] Get Material...\n");
//  for (int i : range(mtrCnt)) ParseMaterial(pScene->GetMaterial(i));
//
//
//  /* ParseMesh */
//  // 使用しているマテリアルの名前取得
//  FbxLayerElementArrayTemplate<int>* matIndexs;
//  if (fbxMesh->GetMaterialIndices(&matIndexs) && matIndexs->GetCount() > 0)
//  {
//    auto material = fbxMesh->GetNode()->GetMaterial(matIndexs->GetAt(0));
//    mesh.mtrName = material->GetName();
//  }
//  else throw "Dont use Material";
//
//  /* ParseMaterial */
//  // Materialの名前とindexを紐付ける
//  mMaterialIndexDictionary[fbxMaterial->GetName()] = mMaterialIndexDictionary.size();
//
//  mesh側でmMaterialIndexDictionary[mesh.mtrName]とやると対応付けされたMaterialが取れる


  pImporter->Destroy();  // インポータの削除

  // ノード探査
  pTopMyNode = MY_NODE2::recursiveNode(pManager, pScene->GetRootNode(), m_bTexcoordVReverse, this);

  pScene->Destroy();

  // なんかOS管理の奴破棄
  ios->Destroy();

  //FBXマネージャーの破棄
  pManager->Destroy();
}


///////////////////////////////////////////////////////////////////////////////
// マテリアルの取得（どのメッシュに対応？）
void MY_NODE2::analyzeMaterial(FbxMesh* mesh)
{
  int layerNum = mesh->GetLayerCount();
  for (int layer = 0; layer < layerNum; layer++) {
    FbxLayerElementMaterial* pElementMaterial = mesh->GetLayer(layer)->GetMaterials();
    if( !pElementMaterial ) continue;

    int materialIndexCount = pElementMaterial->GetIndexArray().GetCount();
    if (materialIndexCount == 0) continue;

    FbxLayerElement::EMappingMode mappingMode = pElementMaterial->GetMappingMode();	//マッピングモードの取得
    FbxLayerElement::EReferenceMode refMode = pElementMaterial->GetReferenceMode();	//リファレンスモードの取得

    if (mappingMode == FbxLayerElement::eAllSame){
      if (refMode == FbxLayerElement::eIndexToDirect) {
        meshes.back().materialIndex = pElementMaterial->GetIndexArray().GetAt(0);
//        int materialIndex = pElementMaterial->GetIndexArray().GetAt(0);
      }
      else MessageBox(NULL, "未対応でーす", "マテリアルリファレンスモード", MB_OK);
    }
    else if (mappingMode == FbxLayerElement::eByPolygon) {
      meshes.back().materialIndex = pElementMaterial->GetIndexArray().GetAt(0);
//      if (refMode == FbxLayerElement::eIndexToDirect) {
//        // ポリゴン毎にマテリアルインデックス
//        int polygonCount = mesh->GetPolygonCount();//ポリゴン数
//
//        for (int polygonNum = 0; polygonNum < polygonCount; polygonNum++) {
//          // 面を構成する頂点数の取得
//          int materialCount = pElementMaterial->GetIndexArray().GetAt(polygonNum);
//        }
//      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// マテリアルの取得（ノードから）
void MY_NODE2::analyzeMaterial(FbxNode* node)
{
  // マテリアル数の取得
  int materialCount = node->GetMaterialCount();

  textures.reserve(materialCount);

  for (int i = 0; i < materialCount; i++) {
    // マテリアル情報の取得
    FbxSurfaceMaterial* pMaterial = node->GetMaterial(i);

    // ディフューズ情報の取得
    FbxProperty diffuseProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

    // プロパティが持っているレイヤードテクスチャの枚数をチェック
    int layeredTextureCount = diffuseProperty.GetSrcObjectCount<FbxLayeredTexture>();

    // レイヤードテクスチャが無ければ通常テクスチャ
    if (layeredTextureCount == 0) {
      // 通常テクスチャの枚数をチェック
      int textureCount = diffuseProperty.GetSrcObjectCount<FbxFileTexture>();

      // 各テクスチャについてテクスチャ情報をゲット
      for (int i = 0; i < textureCount; i++) {
        // i番目のテクスチャオブジェクト取得
        FbxFileTexture* pTexture = diffuseProperty.GetSrcObject<FbxFileTexture>(i);

        // テクスチャファイル名の取得
        std::string fileName = pTexture->GetFileName();
        std::string relFileName = pTexture->GetRelativeFileName();

        // UVSet名の取得
        std::string uvSetName = pTexture->UVSet.Get().Buffer();

        // 辞書登録
        // texfile2UVset[relFileName] = uvSetName;
        int path_i = relFileName.find_last_of("\\") + 1;
        std::string extname = relFileName.substr(path_i, relFileName.size() - path_i);

        //        std::string strPathName ="data\\MODEL\\FbxModel\\"+extname;
        //std::string strPathName = "data\\MODEL\\dude\\" + extname;
        std::string strPathName = "data\\MODEL\\kano.fbm\\" + extname;
        //std::string strPathName = "data\\MODEL\\Tree\\" + extname;
        //std::string strPathName = "data\\MODEL\\textures\\" + extname;
        //std::string strPathName = relFileName;

        // 文字列
        size_t length = strPathName.size();
        // 変換後文字数
        size_t convLength;
        // 仮の受け皿を用意
        wchar_t *wc = (wchar_t*)malloc(sizeof(wchar_t)* (length + 2));
        mbstowcs_s(&convLength, wc, length + 1, strPathName.c_str(), _TRUNCATE);

        // テクスチャの読み込みと管理
        AppManager::GetTextureManager()->Load((std::wstring)wc);
        textures.push_back(wc);

        // 仮の受け皿を破棄
        free(wc);
      }
    }
    else { // レイヤードテクスチャあり
      MessageBox(NULL, "レイヤードテクスチャ", "マテリアルの取得", MB_OK);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// アトリビュート名を頂く
std::string MY_NODE2::GetAttributeTypeName(FbxNodeAttribute::EType type)
{
  switch (type) {
  case FbxNodeAttribute::eUnknown: return "unidentified";
  case FbxNodeAttribute::eNull: return "null";
  case FbxNodeAttribute::eMarker: return "marker";
  case FbxNodeAttribute::eSkeleton: return "skeleton";
  case FbxNodeAttribute::eMesh: return "mesh";
  case FbxNodeAttribute::eNurbs: return "nurbs";
  case FbxNodeAttribute::ePatch: return "patch";
  case FbxNodeAttribute::eCamera: return "camera";
  case FbxNodeAttribute::eCameraStereo: return "stereo";
  case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
  case FbxNodeAttribute::eLight: return "light";
  case FbxNodeAttribute::eOpticalReference: return "optical reference";
  case FbxNodeAttribute::eOpticalMarker: return "marker";
  case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
  case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
  case FbxNodeAttribute::eBoundary: return "boundary";
  case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
  case FbxNodeAttribute::eShape: return "shape";
  case FbxNodeAttribute::eLODGroup: return "lodgroup";
  case FbxNodeAttribute::eSubDiv: return "subdiv";
  default: return "unknown";
  }
}

///////////////////////////////////////////////////////////////////////////////
// 頂点座標の取得
void MY_NODE2::analyzePosition(FbxMesh* mesh)
{
  // コントロールポイント数の取得
  int controlPointsCount = mesh->GetControlPointsCount(); 

  // 頂点座標用コンテナの領域予約
  meshes.back().points.reserve(controlPointsCount); 

  FbxMatrix lReferenceGeometry = GetGeometry(mesh->GetNode());
  DirectX::XMMATRIX d3dMtx;
  float mat[4][4];
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      mat[x][y] = (float)lReferenceGeometry.Get(x, y);
    }
  }
  d3dMtx = DirectX::XMMatrixSet(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
  DirectX::XMMATRIX matrix = d3dMtx;

  // 頂点データの取得 
  FbxVector4* pP = mesh->GetControlPoints();
  for( int i = 0; i < mesh->GetControlPointsCount(); i++ ) {
    Vector3 tempPos = Vector3(static_cast<float>(pP[i][0]), static_cast<float>(pP[i][1]), static_cast<float>(pP[i][2]));
    tempPos = Vec3(DirectX::XMVector3TransformCoord(XM(tempPos), matrix));
    meshes.back().points.push_back(tempPos);
  }

  /* 頂点インデックスの取得 */
  // インデックス数を取得
  int polygonVertexCount = mesh->GetPolygonVertexCount(); 
  // 頂点座標インデックス用コンテナの領域予約
  meshes.back().positionIndices.reserve(polygonVertexCount);
  // インデックスバッファの取得
  for( int i = 0; i < polygonVertexCount; i++ ) {
    meshes.back().positionIndices.push_back(static_cast<unsigned short>(mesh->GetPolygonVertices()[i]));
  }
}


///////////////////////////////////////////////////////////////////////////////
// 頂点バッファ作成
void MESH_FBX2::CreateVertexBuffer()
{
  HRESULT result;
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;
  int numVertex = positionIndices.size();

  // デバイスの取得
  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();
  // 頂点の一時バッファ確保
  FBXStruct::Vertex* vertices = new FBXStruct::Vertex[numVertex];
  for (int i = 0; i < numVertex; i++)
  {
    vertices[i].pos = points[positionIndices[i]].positions;
    vertices[i].nrm = normals[normalIndices[i]];
    vertices[i].tex = texcoords[texcoordIndices[i]];

    for (int bone = 0; bone < 4; bone++) {
      if (points[positionIndices[i]].bornRefarences.size() > bone) {
        vertices[i].weight[bone] = points[positionIndices[i]].bornRefarences[bone].weight;
        vertices[i].boneIndex[bone] = points[positionIndices[i]].bornRefarences[bone].index;
      }
      else{
        vertices[i].weight[bone] = 0.0f;
        vertices[i].boneIndex[bone] = 0;
      }
    }
  }

  // 頂点バッファの設定
  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = sizeof(FBXStruct::Vertex)* numVertex;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  // サブリソース構造体に頂点データへのポインタを格納
  vertexData.pSysMem = vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  //頂点バッファ作成
  result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
  delete[] vertices;
  // 頂点確保を失敗
  if (FAILED(result)) {
    MessageBox(NULL, "MakeVertexBufferError", "fbxfile", MB_OK);
  }
}


///////////////////////////////////////////////////////////////////////////////
// 頂点バッファ作成
void MESH_FBX2::CreateRegidVertexBuffer()
{
  HRESULT result;
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;
  int numVertex = positionIndices.size();

  // デバイスの取得
  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();
  // 頂点の一時バッファ確保
  FBXStruct::RegidVertex* vertices = new FBXStruct::RegidVertex[numVertex];
  for (int i = 0; i < numVertex; i++)
  {
    vertices[i].pos = points[positionIndices[i]].positions;
    vertices[i].nrm = normals[normalIndices[i]];
    vertices[i].tex = texcoords[texcoordIndices[i]];
  }

  // 頂点バッファの設定
  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = sizeof(FBXStruct::RegidVertex)* numVertex;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  // サブリソース構造体に頂点データへのポインタを格納
  vertexData.pSysMem = vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  //頂点バッファ作成
  result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
  delete[] vertices;
  // 頂点確保を失敗
  if (FAILED(result)) {
    MessageBox(NULL, "MakeVertexBufferError", "fbxfile", MB_OK);
  }
}


///////////////////////////////////////////////////////////////////////////////
// ノードを掘る
MY_NODE2* MY_NODE2::recursiveNode(FbxManager* pManager, FbxNode* pNode, bool bTexcoordVReverse, FBXFile2* uniqueKun)
{
  MY_NODE2* p = NULL;

  if (pNode) {
    p = new MY_NODE2;
    p->name = pNode->GetName();
    p->translation.x = static_cast<float>(pNode->LclTranslation.Get()[0]);
    p->translation.y = static_cast<float>(pNode->LclTranslation.Get()[1]);
    p->translation.z = static_cast<float>(pNode->LclTranslation.Get()[2]);
    p->rotation.x = static_cast<float>(pNode->LclRotation.Get()[0]);
    p->rotation.y = static_cast<float>(pNode->LclRotation.Get()[1]);
    p->rotation.z = static_cast<float>(pNode->LclRotation.Get()[2]);
    p->rotation = p->rotation * DirectX::XM_PI / 180;
    p->scaling.x = static_cast<float>(pNode->LclScaling.Get()[0]);
    p->scaling.y = static_cast<float>(pNode->LclScaling.Get()[1]);
    p->scaling.z = static_cast<float>(pNode->LclScaling.Get()[2]);

    DirectX::XMMATRIX mat;
    DirectX::XMMATRIX mtxScl = DirectX::XMMatrixScaling(p->scaling.x, p->scaling.y, p->scaling.z);
    DirectX::XMMATRIX mtxRotate = DirectX::XMMatrixRotationRollPitchYaw(p->rotation.x, p->rotation.y, p->rotation.z);
    DirectX::XMMATRIX mtxTrans = DirectX::XMMatrixTranslation(p->translation.x, p->translation.y, p->translation.z);
    mat = mtxScl * mtxRotate* mtxTrans;
    p->invMtx = DirectX::XMMatrixInverse(NULL, mat);

    for (int i = 0; i < pNode->GetNodeAttributeCount(); i++) {
      FbxNodeAttribute::EType type = pNode->GetNodeAttributeByIndex(i)->GetAttributeType();
      p->attributeNames.push_back(GetAttributeTypeName(type));

      if (type == FbxNodeAttribute::eMesh) {
        // マテリアル情報の解析（マテリアルリスト化）
        p->analyzeMaterial(pNode); 

        p->meshes.push_back(MESH_FBX2());

        // メッシュ情報の取得
        FbxMesh* pMesh = FbxCast<FbxMesh>(pNode->GetNodeAttributeByIndex(i)); 

        // 頂点座標解析
        p->analyzePosition(pMesh);
        // 法線解析
        p->analyzeNormal(pMesh);
        // UV解析
        p->analyzeTexcoord(pMesh, bTexcoordVReverse);
        // マテリアル解析（参照情報の取得）
        p->analyzeMaterial(pMesh);

        // スキンメッシュだったら
        if (uniqueKun->m_bones.size() != 0) {
          // ボーン解析
          p->analyzeCluster(pMesh, uniqueKun);
          // 頂点バッファ生成
          p->meshes.back().CreateVertexBuffer();
        }
        else { // 剛体だったら
               // 頂点バッファ生成
          p->meshes.back().CreateRegidVertexBuffer();
        }
      }
      else {
        // メッシュではないアトリビュート
        //MessageBox(NULL, GetAttributeTypeName(type).c_str(), "アトリビュート", MB_OK);
      }
    }

    for (int i = 0; i < pNode->GetChildCount(); i++) {
      p->children.push_back(recursiveNode(pManager, pNode->GetChild(i), bTexcoordVReverse, uniqueKun));
    }
  }
  return p;
}


///////////////////////////////////////////////////////////////////////////////
// 法線取得
void MY_NODE2::analyzeNormal(FbxMesh* pMesh)
{
  // レイヤー数の取得
  int layerCount = pMesh->GetLayerCount(); 

  for (int layer = 0; layer < layerCount; layer++) {
    // 法線の取得
    FbxLayerElementNormal* pElementNormal = pMesh->GetElementNormal(layer); 

    if (!pElementNormal) { continue; }

    // 法線数の取得
    int normalCount = pElementNormal->GetDirectArray().GetCount(); 

    // 法線格納用コンテナの領域予約
    meshes.back().normals.reserve(normalCount); 

    // 法線データの取得
    for( int i = 0; i < normalCount; i++ ) {
      meshes.back().normals.push_back(Vector3(static_cast<float>(pElementNormal->GetDirectArray()[i][0]), static_cast<float>(pElementNormal->GetDirectArray()[i][1]), static_cast<float>(pElementNormal->GetDirectArray()[i][2])));
    }

    // マッピングモード・リファレンスモード取得
    FbxLayerElement::EMappingMode mappingMode = pElementNormal->GetMappingMode();
    FbxLayerElement::EReferenceMode referenceMode = pElementNormal->GetReferenceMode(); 

    switch (mappingMode) {
    case FbxLayerElement::eNone:
      MessageBox(NULL, "Normal MappingMode = mappingMode", "未実装", MB_OK);
      break;

    case FbxLayerElement::eByControlPoint:
      // 頂点バッファと同じインデックスを使用 
      if (referenceMode == FbxLayerElement::eDirect) {
        // 法線インデックス格納用コンテナの領域予約
        meshes.back().normalIndices.reserve(meshes.back().points.size());
        // 頂点バッファと同じインデックスをコピー
        meshes.back().normalIndices.assign( meshes.back().positionIndices.begin(), meshes.back().positionIndices.end());
      }
      else if( referenceMode == FbxLayerElement::eIndexToDirect  || referenceMode == FbxLayerElement::eIndex ){
        MessageBox(NULL,  "Normal ReferenceMode = eIndexToDirect or eIndex, MappingMode = eByControlPoint",  "未実装", MB_OK);
      }
      break;

    case FbxLayerElement::eByPolygonVertex:
      /* 法線独自のインデックスを使用 */
      if (referenceMode == FbxLayerElement::eDirect) {
        // インデックス参照の必要なし => インデックスを作成 
        // 法線インデックス格納用コンテナの領域予約
        meshes.back().normalIndices.reserve(normalCount);
        // 法線インデックスの作成
        for( int i = 0; i < normalCount; i++ ) {
          meshes.back().normalIndices.push_back(i);
        }
      }
      else if( referenceMode == FbxLayerElement::eIndexToDirect  || referenceMode == FbxLayerElement::eIndex ) { 
        // 独自インデックスを所持 
        // インデックス数の取得
        int normalIndexCount = pElementNormal->GetIndexArray().GetCount();
        // 法線インデックス格納用コンテナの領域予約
        meshes.back().normalIndices.reserve(normalIndexCount);
        // 法線インデックスの取得
        for( int i = 0; i < normalIndexCount; i++ ) {
          meshes.back().normalIndices.push_back(pElementNormal->GetIndexArray()[i]);
        }
      } 
      break;

    case FbxLayerElement::eByPolygon:
      MessageBox(NULL, "Normal MappingMode = eByPolygon", "未実装", MB_OK);
      break;
    case FbxLayerElement::eByEdge:
      MessageBox(NULL, "Normal MappingMode = eByEdge", "未実装", MB_OK);
      break;
    case FbxLayerElement::eAllSame:
      MessageBox(NULL, "Normal MappingMode = eAllSame", "未実装", MB_OK);
      break;
    default:
      MessageBox(NULL, "Normal ???", "未実装", MB_OK);
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// テクスチャ座標取得
void MY_NODE2::analyzeTexcoord(FbxMesh* pMesh, bool bRevers)
{
  int layerCount = pMesh->GetLayerCount();

  if (!layerCount) {
    MessageBox(NULL, "レイヤーを持っていないメッシュを確認", "Analyze Texcoord", MB_OK);
    return;
  }

  for (int layer = 0; layer < layerCount; layer++) {
    // UVの取得
    FbxLayerElementUV* pElementUV = pMesh->GetLayer(layer)->GetUVs();
    if (!pElementUV) { 
      MessageBox(NULL, "...UVのないメッシュレイヤーを確認", "Analyze Texcoord", MB_OK);
      continue;
    }

    // UVセット名を取得
    // = pElementUV->GetName(); 

    // マッピングモード・リファレンスモード取得
    FbxLayerElement::EMappingMode mappingMode = pElementUV->GetMappingMode();
    FbxLayerElement::EReferenceMode referenceMode = pElementUV->GetReferenceMode(); 

    if (mappingMode == FbxLayerElement::eByPolygonVertex) {
      if (referenceMode == FbxLayerElement::eIndexToDirect || referenceMode == FbxLayerElement::eIndex) {
        int uvIndexCount = pElementUV->GetIndexArray().GetCount();
        meshes.back().texcoordIndices.reserve(uvIndexCount);

        for (int i = 0; i < uvIndexCount; i++) { 
          meshes.back().texcoordIndices.push_back(pElementUV->GetIndexArray().GetAt(i));
        }

        int uvCount = pElementUV->GetDirectArray().GetCount();
        meshes.back().texcoords.reserve(uvCount);

        for (int i = 0; i < uvCount; i++) {
          meshes.back().texcoords.push_back(Vector2(static_cast<float>(pElementUV->GetDirectArray().GetAt(i)[0]), static_cast<float>(bRevers ? 1 - pElementUV->GetDirectArray().GetAt(i)[1] : pElementUV->GetDirectArray().GetAt(i)[1])));
        }
      }
      else {
        MessageBox(NULL, "Texcoord::未対応のリファレンスモードを取得", "FbxLayerElement::eByPolygonVertex", MB_OK);
        break;
      }
    }
    else if (mappingMode == FbxLayerElement::eByControlPoint) { MessageBox(NULL, "...未対応マッピングモード[eByControlPoint]を取得した", "Analyze Texcoord", MB_OK); }
    else if (mappingMode == FbxLayerElement::eByPolygon) { MessageBox(NULL, "...未対応マッピングモード[eByPolygon]を取得した", "Analyze Texcoord", MB_OK); }
    else if (mappingMode == FbxLayerElement::eByEdge) { MessageBox(NULL, "...未対応マッピングモード[eByEdge]を取得した", "Analyze Texcoord", MB_OK); }
    else { MessageBox(NULL, "...知らないマッピングモードを取得した", "Analyze Texcoord", MB_OK); }

    break; // とりあえず１個めだけ     
  }
}

///////////////////////////////////////////////////////////////////////////////
// クラスター（ボーン）の取得
void MY_NODE2::analyzeCluster(FbxMesh* mesh, FBXFile2* uniqueKun)
{
  // スキンの数を取得
  int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
  for (int skinNum = 0; skinNum < skinCount; skinNum++)
  {
    // スキンを取得
    FbxSkin* pSkin = (FbxSkin*)mesh->GetDeformer(skinNum, FbxDeformer::eSkin);
    // クラスターの数を取得
    int clusterCount = pSkin->GetClusterCount();
    for (int clusterNum = 0; clusterNum < clusterCount; clusterNum++)
    {
      // クラスタを取得
      FbxCluster* pCluster = pSkin->GetCluster(clusterNum);
      // このクラスタが影響を及ぼす頂点インデックスの個数を取得
      int pointIndexCount = pCluster->GetControlPointIndicesCount();

      int* pointIndexArray = pCluster->GetControlPointIndices();
      double* weightArray = pCluster->GetControlPointWeights();

      int uniqueID = pCluster->GetUniqueID();

      int ClusterGlobalNum = uniqueKun->m_uniqueID[uniqueID];

      for (int i = 0; i < pointIndexCount; i++)
      {
        meshes.back().points[pointIndexArray[i]].bornRefarences.push_back(BornRefarence(ClusterGlobalNum, static_cast<float>(weightArray[i])));
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// ジオメトリーのオフセット取るよー
FbxAMatrix MY_NODE2::GetGeometry(FbxNode* node)
{
  const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
  const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
  const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);

  return FbxAMatrix(lT, lR, lS);
}

 

///////////////////////////////////////////////////////////////////////////////
// ノード順に描画処理を実行
void MY_NODE2::recursiveDraw(CCamera* pCamera, const DirectX::XMMATRIX& mtxWorldParent, ID3D11Buffer* boneCB)
{
  // シェーダを設定して描画
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();

  ///////////////////////////////////////////////////////////////////////////
  //	ワールドマトリクスの作成
  DirectX::XMMATRIX mtxScl = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);
  DirectX::XMMATRIX mtxRotate = DirectX::XMMatrixRotationRollPitchYaw( rotation.x, rotation.y, rotation.z);
  DirectX::XMMATRIX mtxTrans = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
  DirectX::XMMATRIX mtxWorld = mtxScl * mtxRotate* mtxTrans * mtxWorldParent;

  if (!meshes.empty()) {
    for (auto itm = meshes.begin(); itm != meshes.end(); ++itm) {
      // テクスチャ設定
      Texture* texture = nullptr;
      if (!textures.empty()) {
        texture = AppManager::GetTextureManager()->GetResource(textures[itm->materialIndex]);
      }

      DirectX::XMMATRIX mtxWIT = mtxWorld;
      DirectX::XMMatrixInverse(nullptr, mtxWIT);
      DirectX::XMMatrixTranspose(mtxWIT);

      DirectX::XMMATRIX mtxView = *pCamera->GetViewMtx();
      DirectX::XMMATRIX mtxProj = *pCamera->GetProjMtx();
      
      // 入力アセンブラに頂点バッファを設定
      UINT stride = sizeof(FBXStruct::Vertex);
      UINT offset = 0;
      pDeviceContext->IASetVertexBuffers(0, 1, &itm->vertexBuffer, &stride, &offset);

      Shader* shader = AppManager::GetShaderManager()->GetShader(ShaderManager::eShaderTypeSkin);
      shader->SetParameters(invMtx * mtxWorld, mtxView, mtxProj, mtxWIT, boneCB, texture);

      // プリミティブの種類を設定
      pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // 描画
      pDeviceContext->Draw(itm->positionIndices.size(), 0);
    }
  }
  std::for_each(children.begin(), children.end(), [&](MY_NODE2* p) { p->recursiveDraw(pCamera, mtxWorld, boneCB); });
}




///////////////////////////////////////////////////////////////////////////////
// ノード順に描画処理を実行
void MY_NODE2::regidRecursiveDraw(CCamera* pCamera, const DirectX::XMMATRIX& mtxWorldParent)
{
  // シェーダを設定して描画
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();

  ///////////////////////////////////////////////////////////////////////////
  //	ワールドマトリクスの作成
  DirectX::XMMATRIX mtxScl = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);
  DirectX::XMMATRIX mtxRotate = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
  DirectX::XMMATRIX mtxTrans = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
  DirectX::XMMATRIX mtxWorld = mtxScl * mtxRotate * mtxTrans * mtxWorldParent;

  if (!meshes.empty()) {
    for (auto itm = meshes.begin(); itm != meshes.end(); ++itm) {
      // テクスチャ設定
      Texture* texture = nullptr;
      if (!textures.empty()) {
        texture = AppManager::GetTextureManager()->GetResource(textures[itm->materialIndex]);
      }

      DirectX::XMMATRIX mtxWIT = mtxWorld;
      DirectX::XMMatrixInverse(nullptr, mtxWIT);
      DirectX::XMMatrixTranspose(mtxWIT);

      DirectX::XMMATRIX mtxView = *pCamera->GetViewMtx();
      DirectX::XMMATRIX mtxProj = *pCamera->GetProjMtx();

      // 入力アセンブラに頂点バッファを設定
      UINT stride = sizeof(FBXStruct::RegidVertex);
      UINT offset = 0;
      pDeviceContext->IASetVertexBuffers(0, 1, &itm->vertexBuffer, &stride, &offset);

      Shader* shader = AppManager::GetShaderManager()->GetShader(ShaderManager::eShaderTypeRegid);
      shader->SetParameters(mtxWorld, mtxView, mtxProj, mtxWIT, texture);

      // プリミティブの種類を設定
      pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // 描画
      pDeviceContext->Draw(itm->positionIndices.size(), 0);
    }
  }
  std::for_each(children.begin(), children.end(), [&](MY_NODE2* p) { p->regidRecursiveDraw(pCamera, mtxWorld); });
}


///////////////////////////////////////////////////////////////////////////////
// 描画
void FBXFile2::Render(CCamera* pCamera)
{
  int anim = 0;
  // 定数バッファの設定.
  DirectX::XMMATRIX bones[ 512];
  if (!m_bones.empty()) {
    anim = (currentFrame / 300) % m_bones.size();

    for (int numBone = 0; numBone < m_bones[anim].size(); numBone++) {
      bones[numBone] = m_bones[anim][numBone][currentFrame%m_bones[anim][0].size()];
    }
  }
  AppManager::GetRenderer()->GetDeviceContext()->UpdateSubresource(m_boneCB, 0, NULL, bones, 0, 0);

  ///////////////////////////////////////////////////////////////////////////
  //	ワールドマトリクスの作成
  DirectX::XMMATRIX world = GetTransform()->GetWorldMatrix();

  //  スキンメッシュ
  if (m_bones.size() != 0) {
    pTopMyNode->recursiveDraw(pCamera, world, m_boneCB);
  }
  else { // 剛体
    pTopMyNode->regidRecursiveDraw(pCamera, world);
  }
}


///////////////////////////////////////////////////////////////////////////////
// 描画
bool FBXFile2::ViewingFrustumCulling(ViewingFrustum* frustum)
{
  return frustum->CheckPoint(GetTransform()->GetPosition()); 
}

//if (!meshes.empty()) {
//  for (auto itm = meshes.begin(); itm != meshes.end(); ++itm) {
//    // テクスチャ設定1
//    if (!textures.empty()) {
//      //      textures[itm->materialIndex];
//      //      pDevice->SetTexture(0, m_pTexture);
//    }
//    // 頂点情報を設定
//    VERTEX_3D *pVtx;
//    //バッファをロックし仮想アドレスを取得    ↓仮想アドレス取得
//    itm->vertexBuffer->Lock(0, 0, (void**)&pVtx, 0);
//    if (itm->matrixes.empty()) {// 骨なし（つまり剛体の塊）
//      for (int i = 0; i < itm->positionIndices.size(); i++) {
//        pVtx[i].pos = D3DXVECTOR3(itm->points[itm->positionIndices[i]].positions.x, itm->points[itm->positionIndices[i]].positions.y, itm->points[itm->positionIndices[i]].positions.z);
//        if (!itm->normals.empty()) pVtx[i].normal = D3DXVECTOR3(itm->normals[itm->normalIndices[i]].x, itm->normals[itm->normalIndices[i]].y, itm->normals[itm->normalIndices[i]].z);
//        if (!itm->texcoords.empty()) pVtx[i].tex = D3DXVECTOR2(itm->texcoords[itm->texcoordIndices[i]].x, itm->texcoords[itm->texcoordIndices[i]].y);
//      }
//    }
//    else {// 骨あり（つまりワンスキンなど） 
//          // 頂点の座標変換
//      std::vector<Vector3> positions;
//      positions.reserve(itm->points.size());
//      DirectX::XMMATRIX mtx;
//      for (auto it = itm->points.begin(); it != itm->points.end(); ++it) {
//        ZeroMemory(&mtx, sizeof(DirectX::XMMATRIX));
//        for (auto itb = it->bornRefarences.begin(); itb != it->bornRefarences.end(); ++itb) {
//          int allFrame = itm->matrixes[itb->index].size();
//          mtx += itm->matrixes[itb->index][currentFrame%allFrame] * itb->weight;
//        }
//        DirectX::XMVECTOR pos;
//        pos = DirectX::XMVector3TransformCoord(XM(it->positions), mtx);
//        positions.push_back(Vec3(pos));
//      }
//
//      for (size_t i = 0; i < itm->positionIndices.size(); i++) {
//        pVtx[i].pos = D3DXVECTOR3(positions[itm->positionIndices[i]].x, positions[itm->positionIndices[i]].y, positions[itm->positionIndices[i]].z);
//        if (!itm->normals.empty()) pVtx[i].normal = D3DXVECTOR3(itm->normals[itm->normalIndices[i]].x, itm->normals[itm->normalIndices[i]].y, itm->normals[itm->normalIndices[i]].z);
//        if (!itm->texcoords.empty()) pVtx[i].tex = D3DXVECTOR2(itm->texcoords[itm->texcoordIndices[i]].x, itm->texcoords[itm->texcoordIndices[i]].y);
//      }
//    }
//
//    itm->vertexBuffer->Unlock();
//    //デバイスにワールド変換行列を設定
//    pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);
//    //ストリームの設定
//    pDevice->SetStreamSource(0, itm->vertexBuffer, 0, sizeof(VERTEX_3D));
//    pDevice->SetFVF(FVF_VERTEX_3D);
//    //ポリゴンの描画
//    pDevice->SetTexture(0, nullptr);
//    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, itm->positionIndices.size() / 3);
//  }
//}


