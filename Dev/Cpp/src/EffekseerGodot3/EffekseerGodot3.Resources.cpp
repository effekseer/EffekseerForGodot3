
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include <VisualServer.hpp>
#include "EffekseerGodot3.Utils.h"
#include "EffekseerGodot3.Resources.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot3
{

ModelResource::ModelResource(const void* data, int32_t size)
	: Model((void*)data, size)
{
	int32_t vertexCount = GetVertexCount();
	const Vertex* vertexData = GetVertexes();
	int32_t faceCount = GetFaceCount();
	const Face* faceData = GetFaces();

	godot::PoolVector3Array positions; positions.resize(vertexCount);
	godot::PoolVector3Array normals; normals.resize(vertexCount);
	//godot::PoolVector3Array tangents; tangents.resize(vertexCount);
	godot::PoolColorArray colors; colors.resize(vertexCount);
	godot::PoolVector2Array texUVs; texUVs.resize(vertexCount);
	godot::PoolIntArray indeces; indeces.resize(faceCount * 3);

	for (int32_t i = 0; i < vertexCount; i++)
	{
		positions.set(i, Convert::Vector3(vertexData[i].Position));
		normals.set(i, Convert::Vector3(vertexData[i].Normal));
		//tangents.set(i, Convert::Vector3(vertexData[i].Tangent));
		colors.set(i, Convert::Color(vertexData[i].VColor));
		texUVs.set(i, Convert::Vector2(vertexData[i].UV));
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
	//arrays[godot::VisualServer::ARRAY_TANGENT] = tangents;
	arrays[godot::VisualServer::ARRAY_COLOR] = colors;
	arrays[godot::VisualServer::ARRAY_TEX_UV] = texUVs;
	arrays[godot::VisualServer::ARRAY_INDEX] = indeces;

	auto vs = godot::VisualServer::get_singleton();
	GodotMesh = vs->mesh_create();
	vs->mesh_add_surface_from_arrays(GodotMesh, godot::VisualServer::PRIMITIVE_TRIANGLES, arrays);
}

ModelResource::~ModelResource()
{
	if (GodotMesh.is_valid())
	{
		auto vs = godot::VisualServer::get_singleton();
		vs->free_rid(GodotMesh);
	}
}

} // namespace EffekseerGodot3
