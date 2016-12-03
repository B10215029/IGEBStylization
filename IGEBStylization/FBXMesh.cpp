#include "FBXMesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

FBXMesh::FBXMesh()
{
	// Initialize the SDK manager. This object handles all our memory management.
	lSdkManager = FbxManager::Create();
	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	lRootNode = NULL;
	vao = vbo = ebo = 0;
	invalid = true;
}

FBXMesh::~FBXMesh()
{
	// Destroy the SDK manager and all the other objects it was handling.
	lSdkManager->Destroy();
}

void FBXMesh::Render()
{
	if (invalid) {
		UpdateMeshBuffer();
	}
	glBindVertexArray(vao);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(UINT_MAX);
	glDrawElements(GL_TRIANGLE_FAN, indicesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void FBXMesh::ReadFile(const char* fileName)
{
	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(fileName, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file is imported; so get rid of the importer.
	lImporter->Destroy();

	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node because it should
	// not contain any attributes.
	lRootNode = lScene->GetRootNode();
	//mesh = GetMesh(lRootNode);

	invalid = true;
}

void FBXMesh::WriteFile(const char* fileName)
{
	//OpenMesh::IO::write_mesh(*this, fileName);
}

void FBXMesh::LoadNodeMesh(FbxNode* node, FbxAMatrix modelMatrix) {
	FbxMesh* mesh = node->GetMesh();
	modelMatrix = modelMatrix * node->EvaluateLocalTransform();
	//printf("%s%s\n", node->GetName(), mesh?"(mesh)":"");
	//printf("\tLclTranslation: %f, %f, %f\n", node->LclTranslation.Get()[0], node->LclTranslation.Get()[1], node->LclTranslation.Get()[2]);
	//printf("\tLclRotation: %f, %f, %f\n", node->LclRotation.Get()[0], node->LclRotation.Get()[1], node->LclRotation.Get()[2]);
	//printf("\tLclScaling: %f, %f, %f\n", node->LclScaling.Get()[0], node->LclScaling.Get()[1], node->LclScaling.Get()[2]);
	if (mesh) {
		FbxAMatrix matrixGeo;
		matrixGeo.SetIdentity();
		if (node->GetNodeAttribute())
		{
			const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
			const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
			const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);
			matrixGeo.SetT(lT);
			matrixGeo.SetR(lR);
			matrixGeo.SetS(lS);
		}
		matrixGeo = modelMatrix * matrixGeo;

		int vertexDataPos = vertexData.size() / 2;
		int vertexCount = mesh->GetControlPointsCount();
		int polygonCount = mesh->GetPolygonCount();

		bool mHasNormal = mesh->GetElementNormalCount() > 0;
		// add vertex position and normal to vbo
		FbxVector4 * lControlPoints = mesh->GetControlPoints();
		FbxVector4 lCurrentVertex;
		FbxVector4 lCurrentNormal;
		FbxGeometryElementNormal * lNormalElement = NULL;
		if (mHasNormal) {
			lNormalElement = mesh->GetElementNormal(0);
		}
		for (int lIndex = 0; lIndex < vertexCount; ++lIndex) {
			// Save the vertex position.
			lCurrentVertex = lControlPoints[lIndex];
			lCurrentVertex  = matrixGeo.MultT(lCurrentVertex);
			glm::dvec4 v(lCurrentVertex[0], lCurrentVertex[1], lCurrentVertex[2], 1);
			vertexData.push_back(v);
			myMesh.add_vertex(MyMesh::Point(glm::value_ptr(v)));
			if (v.x > maxP.x) maxP.x = v.x;
			if (v.y > maxP.y) maxP.y = v.y;
			if (v.z > maxP.z) maxP.z = v.z;
			if (v.x < minP.x) minP.x = v.x;
			if (v.y < minP.y) minP.y = v.y;
			if (v.z < minP.z) minP.z = v.z;
			// Save the normal.
			if (mHasNormal) {
				int lNormalIndex = lIndex;
				if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
					lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
				glm::dvec4 vn(lCurrentNormal[0], lCurrentNormal[1], lCurrentNormal[2], lCurrentNormal[3]);
				//vn = modelMatrix * vn;
				vertexData.push_back(vn);
			}
			else {
				vertexData.push_back(glm::dvec3(1, 0, 0));
			}
		}
		std::vector<MyMesh::VertexHandle> vh;
		for (int lPolygonIndex = 0; lPolygonIndex < polygonCount; ++lPolygonIndex) {
			for (int lVerticeIndex = 0; lVerticeIndex < mesh->GetPolygonSize(lPolygonIndex); ++lVerticeIndex) {
				int lControlPointIndex = mesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
				vertexIndices.push_back(lControlPointIndex + vertexDataPos);
				vh.push_back(myMesh.vertex_handle(lControlPointIndex + vertexDataPos));
			}
			vertexIndices.push_back(UINT_MAX);
			myMesh.add_face(vh);
			vh.clear();
		}
	}
	for (int i = 0; i < node->GetChildCount(); i++) {
		LoadNodeMesh(node->GetChild(i), modelMatrix);
	}
}

void FBXMesh::UpdateMeshBuffer()
{
	if (lRootNode == NULL)
		return;
	if (vao)
		glDeleteVertexArrays(1, &vao);
	if (vbo)
		glDeleteBuffers(1, &vbo);
	if (ebo)
		glDeleteBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	maxP = glm::dvec3(-DBL_MAX);
	minP = glm::dvec3(DBL_MAX);
	vertexData.clear();
	vertexIndices.clear();
	myMesh.clean();
	FbxAMatrix modelMatrix;
	modelMatrix.SetIdentity();
	LoadNodeMesh(lRootNode, modelMatrix);
	myMesh.update_normals();

	tra = (maxP + minP) / -2.0;
	sca = 1.0 / (maxP - minP) * 0.5;
	minSca = sca.x < sca.y ? sca.x : sca.y;
	minSca = minSca < sca.z ? minSca : sca.z;

	printf("v: %d, fv: %d\n", vertexData.size(), vertexIndices.size());
	printf("max: %lf, %lf, %lf\n", maxP.x, maxP.y, maxP.z);
	printf("min: %lf, %lf, %lf\n", minP.x, minP.y, minP.z);
	for (int i = 0; i < vertexData.size(); i += 2) {
		vertexData[i] = (vertexData[i] + tra) * minSca;
		const double* n = myMesh.normal(myMesh.vertex_handle(i / 2)).data();
		vertexData[i + 1] = glm::dvec3(n[0], n[1], n[2]);
		//printf("%lf, %lf, %lf\n", vertexData[i].x, vertexData[i].y, vertexData[i].z);
	}
	//for (int i = 0; i < vertexIndices.size(); i ++) {
	//	printf("%d\n", vertexIndices[i]);
	//}
	indicesCount = vertexIndices.size();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLdouble) * 3, glm::value_ptr(vertexData[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(GLdouble) * 6, 0);
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, sizeof(GLdouble) * 6, (GLvoid*)(sizeof(GLdouble) * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	// add face to ebo
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(GLuint), &vertexIndices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	invalid = false;

}
