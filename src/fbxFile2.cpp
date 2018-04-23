//=============================================================================
// File : fbxfile.h
// Date : 2017/12/13(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
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
//	FBX�̏���������
HRESULT FBXFile2::Init(void)
{
  currentFrame = 0;
  //FBX���f���̓ǂݍ���
  GetFbxModel();

  HRESULT hr = CreateConstantBuffer();
  if (FAILED(hr)) return E_FAIL;

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// �萔�o�b�t�@�̍쐬
HRESULT FBXFile2::CreateConstantBuffer(void)
{
  // �}�g���N�X�o�b�t�@�̐ݒ�.
  D3D11_BUFFER_DESC bd;
  // �{�[���o�b�t�@�̐ݒ�
  ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
  bd.ByteWidth = sizeof(DirectX::XMMATRIX) * 512;
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;

  // �{�[���o�b�t�@�𐶐�.
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateBuffer(&bd, NULL, &m_boneCB);
  if (FAILED(hr)) {
    MessageBox(NULL, "FailedCreateConstantBuffer", "error!", MB_OK);
    return E_FAIL;
  }
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//	FBX�̏I������
void FBXFile2::Uninit(void)
{
}


///////////////////////////////////////////////////////////////////////////////
//	FBX�̍X�V����
void FBXFile2::Update(void)
{
  currentFrame++;
}


///////////////////////////////////////////////////////////////////////////////
//	FBX�̎擾
void FBXFile2::GetFbxModel(void)
{
  // FBX�}�l�[�W���[�̐���
  FbxManager *pManager = FbxManager::Create();

  // OS�̍��Ƃ����z�����Ă����݂����H
  FbxIOSettings *ios = FbxIOSettings::Create(pManager, IOSROOT);
  pManager->SetIOSettings(ios);

  // �C���|�[�^�[�̐���
  FbxImporter *pImporter = FbxImporter::Create(pManager, "");

  //FBX�t�@�C���̓ǂݍ���
  const char *pFileName = m_fileName2.c_str();
  if (!pImporter->Initialize(pFileName))
  {
    MessageBox(NULL, "FBX�t�@�C���̓ǂݍ��݂Ɏ��s���܂����I", "Error!!", MB_OK);
    pImporter->Destroy();
    ios->Destroy();
    pManager->Destroy();
    return;
  }

  //�V�[���I�u�W�F�N�g�̐���
  FbxScene *pScene = FbxScene::Create(pManager, "");  // �쐬
  pImporter->Import(pScene);   // �V�[���쐬

  // �O�p����
  FbxGeometryConverter con(pManager);
  con.Triangulate(pScene, true);//�R�s�[����肽�����false;
  //�k�ރ��b�V�����폜
  //  con.RemoveBadPolygonsFromMeshes(pScene);
  // �}�e���A�����Ƀ��b�V���𕪊�
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

      // �����p���s��̎擾
      FbxAMatrix lReferenceGlobalInitPosition;
      FbxAMatrix lReferenceGlobalCurrentPosition;
      FbxAMatrix lReferenceGeometry;
      FbxAMatrix lClusterGlobalInitPosition;
      FbxAMatrix lClusterGlobalCurrentPosition;
      FbxAMatrix lClusterRelativeInitPosition;
      FbxAMatrix lClusterRelativeCurrentPositionInverse;

      pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
      // lReferenceGlobalCurrentPosition = pGlobalPosition; // <- ���Ԃ񃏁[���h���W�ϊ��s��ł͂Ȃ�����

      // Multiply lReferenceGlobalInitPosition by Geometric Transformation
    //  lReferenceGeometry = GetGeometry(mesh->GetNode());
   //   lReferenceGlobalInitPosition *= lReferenceGeometry;

      // Get the link initial global position and the link current global position.
      pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);

      unsigned long uniqueID = pCluster->GetUniqueID();
      m_uniqueID[uniqueID] = clusterNum;

      for (FbxTime currentTime = startTime; currentTime < endTime; currentTime += oneFrameTime) {
        // lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose); // <- �|�[�Y�s��̎����H�H�H
        lClusterGlobalCurrentPosition = pCluster->GetLink()->EvaluateGlobalTransform(currentTime);

        // Compute the initial position of the link relative to the reference.
        lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

        // Compute the current position of the link relative to the reference.
        lClusterRelativeCurrentPositionInverse = lClusterGlobalCurrentPosition;

        // Compute the shift of the link relative to the reference.
        FbxAMatrix VertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
        // �� �����p���s����l�����ꂽ���[�V�����{�[���s��Ȃ̂ŁA����Œ��_���W��ϊ����邾���ŗǂ� 

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
//  // Mesh�̎擾
//  printf("[Info] Get Mesh...\n");
//  for (int i : range(meshCnt)) ParseMesh(pScene->GetMember<FbxMesh>(i));
//
//  // Material�̎擾
//  printf("[Info] Get Material...\n");
//  for (int i : range(mtrCnt)) ParseMaterial(pScene->GetMaterial(i));
//
//
//  /* ParseMesh */
//  // �g�p���Ă���}�e���A���̖��O�擾
//  FbxLayerElementArrayTemplate<int>* matIndexs;
//  if (fbxMesh->GetMaterialIndices(&matIndexs) && matIndexs->GetCount() > 0)
//  {
//    auto material = fbxMesh->GetNode()->GetMaterial(matIndexs->GetAt(0));
//    mesh.mtrName = material->GetName();
//  }
//  else throw "Dont use Material";
//
//  /* ParseMaterial */
//  // Material�̖��O��index��R�t����
//  mMaterialIndexDictionary[fbxMaterial->GetName()] = mMaterialIndexDictionary.size();
//
//  mesh����mMaterialIndexDictionary[mesh.mtrName]�Ƃ��ƑΉ��t�����ꂽMaterial������


  pImporter->Destroy();  // �C���|�[�^�̍폜

  // �m�[�h�T��
  pTopMyNode = MY_NODE2::recursiveNode(pManager, pScene->GetRootNode(), m_bTexcoordVReverse, this);

  pScene->Destroy();

  // �Ȃ�OS�Ǘ��̓z�j��
  ios->Destroy();

  //FBX�}�l�[�W���[�̔j��
  pManager->Destroy();
}


///////////////////////////////////////////////////////////////////////////////
// �}�e���A���̎擾�i�ǂ̃��b�V���ɑΉ��H�j
void MY_NODE2::analyzeMaterial(FbxMesh* mesh)
{
  int layerNum = mesh->GetLayerCount();
  for (int layer = 0; layer < layerNum; layer++) {
    FbxLayerElementMaterial* pElementMaterial = mesh->GetLayer(layer)->GetMaterials();
    if( !pElementMaterial ) continue;

    int materialIndexCount = pElementMaterial->GetIndexArray().GetCount();
    if (materialIndexCount == 0) continue;

    FbxLayerElement::EMappingMode mappingMode = pElementMaterial->GetMappingMode();	//�}�b�s���O���[�h�̎擾
    FbxLayerElement::EReferenceMode refMode = pElementMaterial->GetReferenceMode();	//���t�@�����X���[�h�̎擾

    if (mappingMode == FbxLayerElement::eAllSame){
      if (refMode == FbxLayerElement::eIndexToDirect) {
        meshes.back().materialIndex = pElementMaterial->GetIndexArray().GetAt(0);
//        int materialIndex = pElementMaterial->GetIndexArray().GetAt(0);
      }
      else MessageBox(NULL, "���Ή��Ł[��", "�}�e���A�����t�@�����X���[�h", MB_OK);
    }
    else if (mappingMode == FbxLayerElement::eByPolygon) {
      meshes.back().materialIndex = pElementMaterial->GetIndexArray().GetAt(0);
//      if (refMode == FbxLayerElement::eIndexToDirect) {
//        // �|���S�����Ƀ}�e���A���C���f�b�N�X
//        int polygonCount = mesh->GetPolygonCount();//�|���S����
//
//        for (int polygonNum = 0; polygonNum < polygonCount; polygonNum++) {
//          // �ʂ��\�����钸�_���̎擾
//          int materialCount = pElementMaterial->GetIndexArray().GetAt(polygonNum);
//        }
//      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// �}�e���A���̎擾�i�m�[�h����j
void MY_NODE2::analyzeMaterial(FbxNode* node)
{
  // �}�e���A�����̎擾
  int materialCount = node->GetMaterialCount();

  textures.reserve(materialCount);

  for (int i = 0; i < materialCount; i++) {
    // �}�e���A�����̎擾
    FbxSurfaceMaterial* pMaterial = node->GetMaterial(i);

    // �f�B�t���[�Y���̎擾
    FbxProperty diffuseProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

    // �v���p�e�B�������Ă��郌�C���[�h�e�N�X�`���̖������`�F�b�N
    int layeredTextureCount = diffuseProperty.GetSrcObjectCount<FbxLayeredTexture>();

    // ���C���[�h�e�N�X�`����������Βʏ�e�N�X�`��
    if (layeredTextureCount == 0) {
      // �ʏ�e�N�X�`���̖������`�F�b�N
      int textureCount = diffuseProperty.GetSrcObjectCount<FbxFileTexture>();

      // �e�e�N�X�`���ɂ��ăe�N�X�`�������Q�b�g
      for (int i = 0; i < textureCount; i++) {
        // i�Ԗڂ̃e�N�X�`���I�u�W�F�N�g�擾
        FbxFileTexture* pTexture = diffuseProperty.GetSrcObject<FbxFileTexture>(i);

        // �e�N�X�`���t�@�C�����̎擾
        std::string fileName = pTexture->GetFileName();
        std::string relFileName = pTexture->GetRelativeFileName();

        // UVSet���̎擾
        std::string uvSetName = pTexture->UVSet.Get().Buffer();

        // �����o�^
        // texfile2UVset[relFileName] = uvSetName;
        int path_i = relFileName.find_last_of("\\") + 1;
        std::string extname = relFileName.substr(path_i, relFileName.size() - path_i);

        //        std::string strPathName ="data\\MODEL\\FbxModel\\"+extname;
        //std::string strPathName = "data\\MODEL\\dude\\" + extname;
        std::string strPathName = "data\\MODEL\\kano.fbm\\" + extname;
        //std::string strPathName = "data\\MODEL\\Tree\\" + extname;
        //std::string strPathName = "data\\MODEL\\textures\\" + extname;
        //std::string strPathName = relFileName;

        // ������
        size_t length = strPathName.size();
        // �ϊ��㕶����
        size_t convLength;
        // ���̎󂯎M��p��
        wchar_t *wc = (wchar_t*)malloc(sizeof(wchar_t)* (length + 2));
        mbstowcs_s(&convLength, wc, length + 1, strPathName.c_str(), _TRUNCATE);

        // �e�N�X�`���̓ǂݍ��݂ƊǗ�
        AppManager::GetTextureManager()->Load((std::wstring)wc);
        textures.push_back(wc);

        // ���̎󂯎M��j��
        free(wc);
      }
    }
    else { // ���C���[�h�e�N�X�`������
      MessageBox(NULL, "���C���[�h�e�N�X�`��", "�}�e���A���̎擾", MB_OK);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// �A�g���r���[�g���𒸂�
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
// ���_���W�̎擾
void MY_NODE2::analyzePosition(FbxMesh* mesh)
{
  // �R���g���[���|�C���g���̎擾
  int controlPointsCount = mesh->GetControlPointsCount(); 

  // ���_���W�p�R���e�i�̗̈�\��
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

  // ���_�f�[�^�̎擾 
  FbxVector4* pP = mesh->GetControlPoints();
  for( int i = 0; i < mesh->GetControlPointsCount(); i++ ) {
    Vector3 tempPos = Vector3(static_cast<float>(pP[i][0]), static_cast<float>(pP[i][1]), static_cast<float>(pP[i][2]));
    tempPos = Vec3(DirectX::XMVector3TransformCoord(XM(tempPos), matrix));
    meshes.back().points.push_back(tempPos);
  }

  /* ���_�C���f�b�N�X�̎擾 */
  // �C���f�b�N�X�����擾
  int polygonVertexCount = mesh->GetPolygonVertexCount(); 
  // ���_���W�C���f�b�N�X�p�R���e�i�̗̈�\��
  meshes.back().positionIndices.reserve(polygonVertexCount);
  // �C���f�b�N�X�o�b�t�@�̎擾
  for( int i = 0; i < polygonVertexCount; i++ ) {
    meshes.back().positionIndices.push_back(static_cast<unsigned short>(mesh->GetPolygonVertices()[i]));
  }
}


///////////////////////////////////////////////////////////////////////////////
// ���_�o�b�t�@�쐬
void MESH_FBX2::CreateVertexBuffer()
{
  HRESULT result;
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;
  int numVertex = positionIndices.size();

  // �f�o�C�X�̎擾
  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();
  // ���_�̈ꎞ�o�b�t�@�m��
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

  // ���_�o�b�t�@�̐ݒ�
  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = sizeof(FBXStruct::Vertex)* numVertex;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  // �T�u���\�[�X�\���̂ɒ��_�f�[�^�ւ̃|�C���^���i�[
  vertexData.pSysMem = vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  //���_�o�b�t�@�쐬
  result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
  delete[] vertices;
  // ���_�m�ۂ����s
  if (FAILED(result)) {
    MessageBox(NULL, "MakeVertexBufferError", "fbxfile", MB_OK);
  }
}


///////////////////////////////////////////////////////////////////////////////
// ���_�o�b�t�@�쐬
void MESH_FBX2::CreateRegidVertexBuffer()
{
  HRESULT result;
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;
  int numVertex = positionIndices.size();

  // �f�o�C�X�̎擾
  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();
  // ���_�̈ꎞ�o�b�t�@�m��
  FBXStruct::RegidVertex* vertices = new FBXStruct::RegidVertex[numVertex];
  for (int i = 0; i < numVertex; i++)
  {
    vertices[i].pos = points[positionIndices[i]].positions;
    vertices[i].nrm = normals[normalIndices[i]];
    vertices[i].tex = texcoords[texcoordIndices[i]];
  }

  // ���_�o�b�t�@�̐ݒ�
  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = sizeof(FBXStruct::RegidVertex)* numVertex;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  // �T�u���\�[�X�\���̂ɒ��_�f�[�^�ւ̃|�C���^���i�[
  vertexData.pSysMem = vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  //���_�o�b�t�@�쐬
  result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
  delete[] vertices;
  // ���_�m�ۂ����s
  if (FAILED(result)) {
    MessageBox(NULL, "MakeVertexBufferError", "fbxfile", MB_OK);
  }
}


///////////////////////////////////////////////////////////////////////////////
// �m�[�h���@��
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
        // �}�e���A�����̉�́i�}�e���A�����X�g���j
        p->analyzeMaterial(pNode); 

        p->meshes.push_back(MESH_FBX2());

        // ���b�V�����̎擾
        FbxMesh* pMesh = FbxCast<FbxMesh>(pNode->GetNodeAttributeByIndex(i)); 

        // ���_���W���
        p->analyzePosition(pMesh);
        // �@�����
        p->analyzeNormal(pMesh);
        // UV���
        p->analyzeTexcoord(pMesh, bTexcoordVReverse);
        // �}�e���A����́i�Q�Ə��̎擾�j
        p->analyzeMaterial(pMesh);

        // �X�L�����b�V����������
        if (uniqueKun->m_bones.size() != 0) {
          // �{�[�����
          p->analyzeCluster(pMesh, uniqueKun);
          // ���_�o�b�t�@����
          p->meshes.back().CreateVertexBuffer();
        }
        else { // ���̂�������
               // ���_�o�b�t�@����
          p->meshes.back().CreateRegidVertexBuffer();
        }
      }
      else {
        // ���b�V���ł͂Ȃ��A�g���r���[�g
        //MessageBox(NULL, GetAttributeTypeName(type).c_str(), "�A�g���r���[�g", MB_OK);
      }
    }

    for (int i = 0; i < pNode->GetChildCount(); i++) {
      p->children.push_back(recursiveNode(pManager, pNode->GetChild(i), bTexcoordVReverse, uniqueKun));
    }
  }
  return p;
}


///////////////////////////////////////////////////////////////////////////////
// �@���擾
void MY_NODE2::analyzeNormal(FbxMesh* pMesh)
{
  // ���C���[���̎擾
  int layerCount = pMesh->GetLayerCount(); 

  for (int layer = 0; layer < layerCount; layer++) {
    // �@���̎擾
    FbxLayerElementNormal* pElementNormal = pMesh->GetElementNormal(layer); 

    if (!pElementNormal) { continue; }

    // �@�����̎擾
    int normalCount = pElementNormal->GetDirectArray().GetCount(); 

    // �@���i�[�p�R���e�i�̗̈�\��
    meshes.back().normals.reserve(normalCount); 

    // �@���f�[�^�̎擾
    for( int i = 0; i < normalCount; i++ ) {
      meshes.back().normals.push_back(Vector3(static_cast<float>(pElementNormal->GetDirectArray()[i][0]), static_cast<float>(pElementNormal->GetDirectArray()[i][1]), static_cast<float>(pElementNormal->GetDirectArray()[i][2])));
    }

    // �}�b�s���O���[�h�E���t�@�����X���[�h�擾
    FbxLayerElement::EMappingMode mappingMode = pElementNormal->GetMappingMode();
    FbxLayerElement::EReferenceMode referenceMode = pElementNormal->GetReferenceMode(); 

    switch (mappingMode) {
    case FbxLayerElement::eNone:
      MessageBox(NULL, "Normal MappingMode = mappingMode", "������", MB_OK);
      break;

    case FbxLayerElement::eByControlPoint:
      // ���_�o�b�t�@�Ɠ����C���f�b�N�X���g�p 
      if (referenceMode == FbxLayerElement::eDirect) {
        // �@���C���f�b�N�X�i�[�p�R���e�i�̗̈�\��
        meshes.back().normalIndices.reserve(meshes.back().points.size());
        // ���_�o�b�t�@�Ɠ����C���f�b�N�X���R�s�[
        meshes.back().normalIndices.assign( meshes.back().positionIndices.begin(), meshes.back().positionIndices.end());
      }
      else if( referenceMode == FbxLayerElement::eIndexToDirect  || referenceMode == FbxLayerElement::eIndex ){
        MessageBox(NULL,  "Normal ReferenceMode = eIndexToDirect or eIndex, MappingMode = eByControlPoint",  "������", MB_OK);
      }
      break;

    case FbxLayerElement::eByPolygonVertex:
      /* �@���Ǝ��̃C���f�b�N�X���g�p */
      if (referenceMode == FbxLayerElement::eDirect) {
        // �C���f�b�N�X�Q�Ƃ̕K�v�Ȃ� => �C���f�b�N�X���쐬 
        // �@���C���f�b�N�X�i�[�p�R���e�i�̗̈�\��
        meshes.back().normalIndices.reserve(normalCount);
        // �@���C���f�b�N�X�̍쐬
        for( int i = 0; i < normalCount; i++ ) {
          meshes.back().normalIndices.push_back(i);
        }
      }
      else if( referenceMode == FbxLayerElement::eIndexToDirect  || referenceMode == FbxLayerElement::eIndex ) { 
        // �Ǝ��C���f�b�N�X������ 
        // �C���f�b�N�X���̎擾
        int normalIndexCount = pElementNormal->GetIndexArray().GetCount();
        // �@���C���f�b�N�X�i�[�p�R���e�i�̗̈�\��
        meshes.back().normalIndices.reserve(normalIndexCount);
        // �@���C���f�b�N�X�̎擾
        for( int i = 0; i < normalIndexCount; i++ ) {
          meshes.back().normalIndices.push_back(pElementNormal->GetIndexArray()[i]);
        }
      } 
      break;

    case FbxLayerElement::eByPolygon:
      MessageBox(NULL, "Normal MappingMode = eByPolygon", "������", MB_OK);
      break;
    case FbxLayerElement::eByEdge:
      MessageBox(NULL, "Normal MappingMode = eByEdge", "������", MB_OK);
      break;
    case FbxLayerElement::eAllSame:
      MessageBox(NULL, "Normal MappingMode = eAllSame", "������", MB_OK);
      break;
    default:
      MessageBox(NULL, "Normal ???", "������", MB_OK);
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// �e�N�X�`�����W�擾
void MY_NODE2::analyzeTexcoord(FbxMesh* pMesh, bool bRevers)
{
  int layerCount = pMesh->GetLayerCount();

  if (!layerCount) {
    MessageBox(NULL, "���C���[�������Ă��Ȃ����b�V�����m�F", "Analyze Texcoord", MB_OK);
    return;
  }

  for (int layer = 0; layer < layerCount; layer++) {
    // UV�̎擾
    FbxLayerElementUV* pElementUV = pMesh->GetLayer(layer)->GetUVs();
    if (!pElementUV) { 
      MessageBox(NULL, "...UV�̂Ȃ����b�V�����C���[���m�F", "Analyze Texcoord", MB_OK);
      continue;
    }

    // UV�Z�b�g�����擾
    // = pElementUV->GetName(); 

    // �}�b�s���O���[�h�E���t�@�����X���[�h�擾
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
        MessageBox(NULL, "Texcoord::���Ή��̃��t�@�����X���[�h���擾", "FbxLayerElement::eByPolygonVertex", MB_OK);
        break;
      }
    }
    else if (mappingMode == FbxLayerElement::eByControlPoint) { MessageBox(NULL, "...���Ή��}�b�s���O���[�h[eByControlPoint]���擾����", "Analyze Texcoord", MB_OK); }
    else if (mappingMode == FbxLayerElement::eByPolygon) { MessageBox(NULL, "...���Ή��}�b�s���O���[�h[eByPolygon]���擾����", "Analyze Texcoord", MB_OK); }
    else if (mappingMode == FbxLayerElement::eByEdge) { MessageBox(NULL, "...���Ή��}�b�s���O���[�h[eByEdge]���擾����", "Analyze Texcoord", MB_OK); }
    else { MessageBox(NULL, "...�m��Ȃ��}�b�s���O���[�h���擾����", "Analyze Texcoord", MB_OK); }

    break; // �Ƃ肠�����P�߂���     
  }
}

///////////////////////////////////////////////////////////////////////////////
// �N���X�^�[�i�{�[���j�̎擾
void MY_NODE2::analyzeCluster(FbxMesh* mesh, FBXFile2* uniqueKun)
{
  // �X�L���̐����擾
  int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
  for (int skinNum = 0; skinNum < skinCount; skinNum++)
  {
    // �X�L�����擾
    FbxSkin* pSkin = (FbxSkin*)mesh->GetDeformer(skinNum, FbxDeformer::eSkin);
    // �N���X�^�[�̐����擾
    int clusterCount = pSkin->GetClusterCount();
    for (int clusterNum = 0; clusterNum < clusterCount; clusterNum++)
    {
      // �N���X�^���擾
      FbxCluster* pCluster = pSkin->GetCluster(clusterNum);
      // ���̃N���X�^���e�����y�ڂ����_�C���f�b�N�X�̌����擾
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
// �W�I���g���[�̃I�t�Z�b�g����[
FbxAMatrix MY_NODE2::GetGeometry(FbxNode* node)
{
  const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
  const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
  const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);

  return FbxAMatrix(lT, lR, lS);
}

 

///////////////////////////////////////////////////////////////////////////////
// �m�[�h���ɕ`�揈�������s
void MY_NODE2::recursiveDraw(CCamera* pCamera, const DirectX::XMMATRIX& mtxWorldParent, ID3D11Buffer* boneCB)
{
  // �V�F�[�_��ݒ肵�ĕ`��
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();

  ///////////////////////////////////////////////////////////////////////////
  //	���[���h�}�g���N�X�̍쐬
  DirectX::XMMATRIX mtxScl = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);
  DirectX::XMMATRIX mtxRotate = DirectX::XMMatrixRotationRollPitchYaw( rotation.x, rotation.y, rotation.z);
  DirectX::XMMATRIX mtxTrans = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
  DirectX::XMMATRIX mtxWorld = mtxScl * mtxRotate* mtxTrans * mtxWorldParent;

  if (!meshes.empty()) {
    for (auto itm = meshes.begin(); itm != meshes.end(); ++itm) {
      // �e�N�X�`���ݒ�
      Texture* texture = nullptr;
      if (!textures.empty()) {
        texture = AppManager::GetTextureManager()->GetResource(textures[itm->materialIndex]);
      }

      DirectX::XMMATRIX mtxWIT = mtxWorld;
      DirectX::XMMatrixInverse(nullptr, mtxWIT);
      DirectX::XMMatrixTranspose(mtxWIT);

      DirectX::XMMATRIX mtxView = *pCamera->GetViewMtx();
      DirectX::XMMATRIX mtxProj = *pCamera->GetProjMtx();
      
      // ���̓A�Z���u���ɒ��_�o�b�t�@��ݒ�
      UINT stride = sizeof(FBXStruct::Vertex);
      UINT offset = 0;
      pDeviceContext->IASetVertexBuffers(0, 1, &itm->vertexBuffer, &stride, &offset);

      Shader* shader = AppManager::GetShaderManager()->GetShader(ShaderManager::eShaderTypeSkin);
      shader->SetParameters(invMtx * mtxWorld, mtxView, mtxProj, mtxWIT, boneCB, texture);

      // �v���~�e�B�u�̎�ނ�ݒ�
      pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // �`��
      pDeviceContext->Draw(itm->positionIndices.size(), 0);
    }
  }
  std::for_each(children.begin(), children.end(), [&](MY_NODE2* p) { p->recursiveDraw(pCamera, mtxWorld, boneCB); });
}




///////////////////////////////////////////////////////////////////////////////
// �m�[�h���ɕ`�揈�������s
void MY_NODE2::regidRecursiveDraw(CCamera* pCamera, const DirectX::XMMATRIX& mtxWorldParent)
{
  // �V�F�[�_��ݒ肵�ĕ`��
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();

  ///////////////////////////////////////////////////////////////////////////
  //	���[���h�}�g���N�X�̍쐬
  DirectX::XMMATRIX mtxScl = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);
  DirectX::XMMATRIX mtxRotate = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
  DirectX::XMMATRIX mtxTrans = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
  DirectX::XMMATRIX mtxWorld = mtxScl * mtxRotate * mtxTrans * mtxWorldParent;

  if (!meshes.empty()) {
    for (auto itm = meshes.begin(); itm != meshes.end(); ++itm) {
      // �e�N�X�`���ݒ�
      Texture* texture = nullptr;
      if (!textures.empty()) {
        texture = AppManager::GetTextureManager()->GetResource(textures[itm->materialIndex]);
      }

      DirectX::XMMATRIX mtxWIT = mtxWorld;
      DirectX::XMMatrixInverse(nullptr, mtxWIT);
      DirectX::XMMatrixTranspose(mtxWIT);

      DirectX::XMMATRIX mtxView = *pCamera->GetViewMtx();
      DirectX::XMMATRIX mtxProj = *pCamera->GetProjMtx();

      // ���̓A�Z���u���ɒ��_�o�b�t�@��ݒ�
      UINT stride = sizeof(FBXStruct::RegidVertex);
      UINT offset = 0;
      pDeviceContext->IASetVertexBuffers(0, 1, &itm->vertexBuffer, &stride, &offset);

      Shader* shader = AppManager::GetShaderManager()->GetShader(ShaderManager::eShaderTypeRegid);
      shader->SetParameters(mtxWorld, mtxView, mtxProj, mtxWIT, texture);

      // �v���~�e�B�u�̎�ނ�ݒ�
      pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // �`��
      pDeviceContext->Draw(itm->positionIndices.size(), 0);
    }
  }
  std::for_each(children.begin(), children.end(), [&](MY_NODE2* p) { p->regidRecursiveDraw(pCamera, mtxWorld); });
}


///////////////////////////////////////////////////////////////////////////////
// �`��
void FBXFile2::Render(CCamera* pCamera)
{
  int anim = 0;
  // �萔�o�b�t�@�̐ݒ�.
  DirectX::XMMATRIX bones[ 512];
  if (!m_bones.empty()) {
    anim = (currentFrame / 300) % m_bones.size();

    for (int numBone = 0; numBone < m_bones[anim].size(); numBone++) {
      bones[numBone] = m_bones[anim][numBone][currentFrame%m_bones[anim][0].size()];
    }
  }
  AppManager::GetRenderer()->GetDeviceContext()->UpdateSubresource(m_boneCB, 0, NULL, bones, 0, 0);

  ///////////////////////////////////////////////////////////////////////////
  //	���[���h�}�g���N�X�̍쐬
  DirectX::XMMATRIX world = GetTransform()->GetWorldMatrix();

  //  �X�L�����b�V��
  if (m_bones.size() != 0) {
    pTopMyNode->recursiveDraw(pCamera, world, m_boneCB);
  }
  else { // ����
    pTopMyNode->regidRecursiveDraw(pCamera, world);
  }
}


///////////////////////////////////////////////////////////////////////////////
// �`��
bool FBXFile2::ViewingFrustumCulling(ViewingFrustum* frustum)
{
  return frustum->CheckPoint(GetTransform()->GetPosition()); 
}

//if (!meshes.empty()) {
//  for (auto itm = meshes.begin(); itm != meshes.end(); ++itm) {
//    // �e�N�X�`���ݒ�1
//    if (!textures.empty()) {
//      //      textures[itm->materialIndex];
//      //      pDevice->SetTexture(0, m_pTexture);
//    }
//    // ���_����ݒ�
//    VERTEX_3D *pVtx;
//    //�o�b�t�@�����b�N�����z�A�h���X���擾    �����z�A�h���X�擾
//    itm->vertexBuffer->Lock(0, 0, (void**)&pVtx, 0);
//    if (itm->matrixes.empty()) {// ���Ȃ��i�܂荄�̂̉�j
//      for (int i = 0; i < itm->positionIndices.size(); i++) {
//        pVtx[i].pos = D3DXVECTOR3(itm->points[itm->positionIndices[i]].positions.x, itm->points[itm->positionIndices[i]].positions.y, itm->points[itm->positionIndices[i]].positions.z);
//        if (!itm->normals.empty()) pVtx[i].normal = D3DXVECTOR3(itm->normals[itm->normalIndices[i]].x, itm->normals[itm->normalIndices[i]].y, itm->normals[itm->normalIndices[i]].z);
//        if (!itm->texcoords.empty()) pVtx[i].tex = D3DXVECTOR2(itm->texcoords[itm->texcoordIndices[i]].x, itm->texcoords[itm->texcoordIndices[i]].y);
//      }
//    }
//    else {// ������i�܂胏���X�L���Ȃǁj 
//          // ���_�̍��W�ϊ�
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
//    //�f�o�C�X�Ƀ��[���h�ϊ��s���ݒ�
//    pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);
//    //�X�g���[���̐ݒ�
//    pDevice->SetStreamSource(0, itm->vertexBuffer, 0, sizeof(VERTEX_3D));
//    pDevice->SetFVF(FVF_VERTEX_3D);
//    //�|���S���̕`��
//    pDevice->SetTexture(0, nullptr);
//    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, itm->positionIndices.size() / 3);
//  }
//}


