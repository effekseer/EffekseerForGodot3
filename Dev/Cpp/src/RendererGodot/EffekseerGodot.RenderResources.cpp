
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include <VisualServer.hpp>
#include "../Utils/EffekseerGodot.Utils.h"
#include "EffekseerGodot.RenderResources.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot
{
	
Model::Model(const Effekseer::CustomVector<Vertex>& vertecies, const Effekseer::CustomVector<Face>& faces)
	: Effekseer::Model(vertecies, faces)
{
	UploadToEngine();
}

Model::Model(const void* data, int32_t size)
	: Effekseer::Model(data, size)
{
	UploadToEngine();
}

Model::~Model()
{
	if (meshRid_.is_valid())
	{
		auto vs = godot::VisualServer::get_singleton();
		vs->free_rid(meshRid_);
	}
}

void Model::UploadToEngine()
{
	int32_t vertexCount = GetVertexCount();
	const Vertex* vertexData = GetVertexes();
	int32_t faceCount = GetFaceCount();
	const Face* faceData = GetFaces();

	godot::PoolVector3Array positions; positions.resize(vertexCount);
	godot::PoolVector3Array normals; normals.resize(vertexCount);
	godot::PoolRealArray tangents; tangents.resize(vertexCount * 4);
	godot::PoolColorArray colors; colors.resize(vertexCount);
	godot::PoolVector2Array texUVs; texUVs.resize(vertexCount);
	godot::PoolIntArray indeces; indeces.resize(faceCount * 3);

	for (int32_t i = 0; i < vertexCount; i++)
	{
		positions.set(i, ToGdVector3(vertexData[i].Position));
		normals.set(i, ToGdVector3(vertexData[i].Normal));
		tangents.set(i * 4 + 0, vertexData[i].Tangent.X);
		tangents.set(i * 4 + 1, vertexData[i].Tangent.Y);
		tangents.set(i * 4 + 2, vertexData[i].Tangent.Z);
		tangents.set(i * 4 + 3, 1.0f);
		colors.set(i, ToGdColor(vertexData[i].VColor));
		texUVs.set(i, ToGdVector2(vertexData[i].UV));
	}
	for (int32_t i = 0; i < faceCount; i++)
	{
		indeces.set(i * 3 + 0, faceData[i].Indexes[0]);
		indeces.set(i * 3 + 1, faceData[i].Indexes[1]);
		indeces.set(i * 3 + 2, faceData[i].Indexes[2]);
	}

	godot::Array arrays;
	arrays.resize(godot::VisualServer::ARRAY_MAX);
	arrays[godot::VisualServer::ARRAY_VERTEX] = positions;
	arrays[godot::VisualServer::ARRAY_NORMAL] = normals;
	arrays[godot::VisualServer::ARRAY_TANGENT] = tangents;
	arrays[godot::VisualServer::ARRAY_COLOR] = colors;
	arrays[godot::VisualServer::ARRAY_TEX_UV] = texUVs;
	arrays[godot::VisualServer::ARRAY_INDEX] = indeces;

	auto vs = godot::VisualServer::get_singleton();
	meshRid_ = vs->mesh_create();
	vs->mesh_add_surface_from_arrays(meshRid_, godot::VisualServer::PRIMITIVE_TRIANGLES, arrays);
}

} // namespace EffekseerGodot
