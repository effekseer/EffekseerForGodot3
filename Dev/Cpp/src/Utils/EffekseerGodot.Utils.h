#pragma once

#include <stdint.h>
#include <Effekseer.h>
#include <String.hpp>
#include <RID.hpp>
#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Transform.hpp>
#include <Color.hpp>
#include <Script.hpp>

namespace EffekseerGodot
{

inline int64_t RIDToInt64(godot::RID rid)
{
	int64_t val;
	memcpy(&val, &rid, sizeof(rid));
	return val;
}

inline godot::RID Int64ToRID(int64_t val)
{
	godot::RID rid;
	memcpy(&rid, &val, sizeof(rid));
	return rid;
}

inline Effekseer::Vector2D ToEfkVector2(godot::Vector2 v)
{
	return { v.x, v.y };
}

inline godot::Vector2 ToGdVector2(Effekseer::Vector2D v)
{
	return { v.X, v.Y };
}

inline Effekseer::Vector3D ToEfkVector3(godot::Vector3 v)
{
	return { v.x, v.y, v.z };
}

inline godot::Vector3 ToGdVector3(Effekseer::Vector3D v)
{
	return { v.X, v.Y, v.Z };
}

inline Effekseer::Matrix44 ToEfkMatrix44(godot::Transform transform)
{
	Effekseer::Matrix44 matrix;
	matrix.Values[0][0] = transform.basis[0][0];
	matrix.Values[0][1] = transform.basis[0][1];
	matrix.Values[0][2] = transform.basis[0][2];
	matrix.Values[0][3] = 0.0f;
	matrix.Values[1][0] = transform.basis[1][0];
	matrix.Values[1][1] = transform.basis[1][1];
	matrix.Values[1][2] = transform.basis[1][2];
	matrix.Values[1][3] = 0.0f;
	matrix.Values[2][0] = transform.basis[2][0];
	matrix.Values[2][1] = transform.basis[2][1];
	matrix.Values[2][2] = transform.basis[2][2];
	matrix.Values[2][3] = 0.0f;
	matrix.Values[3][0] = transform.origin.x;
	matrix.Values[3][1] = transform.origin.y;
	matrix.Values[3][2] = transform.origin.z;
	matrix.Values[3][3] = 1.0f;
	return matrix;
}

inline Effekseer::Matrix43 ToEfkMatrix43(godot::Transform transform)
{
	Effekseer::Matrix43 matrix;
	matrix.Value[0][0] = transform.basis[0][0];
	matrix.Value[0][1] = transform.basis[0][1];
	matrix.Value[0][2] = transform.basis[0][2];
	matrix.Value[1][0] = transform.basis[1][0];
	matrix.Value[1][1] = transform.basis[1][1];
	matrix.Value[1][2] = transform.basis[1][2];
	matrix.Value[2][0] = transform.basis[2][0];
	matrix.Value[2][1] = transform.basis[2][1];
	matrix.Value[2][2] = transform.basis[2][2];
	matrix.Value[3][0] = transform.origin.x;
	matrix.Value[3][1] = transform.origin.y;
	matrix.Value[3][2] = transform.origin.z;
	return matrix;
}

inline godot::Transform ToGdMatrix(Effekseer::Matrix44 matrix)
{
	godot::Transform transform;
	transform.basis[0][0] = matrix.Values[0][0];
	transform.basis[1][0] = matrix.Values[1][0];
	transform.basis[2][0] = matrix.Values[2][0];
	transform.basis[0][1] = matrix.Values[0][1];
	transform.basis[1][1] = matrix.Values[1][1];
	transform.basis[2][1] = matrix.Values[2][1];
	transform.basis[0][2] = matrix.Values[0][2];
	transform.basis[1][2] = matrix.Values[1][2];
	transform.basis[2][2] = matrix.Values[2][2];
	transform.origin.x = matrix.Values[3][0];
	transform.origin.y = matrix.Values[3][1];
	transform.origin.z = matrix.Values[3][2];
	return transform;
}

inline Effekseer::Color ToEfkColor(godot::Color c)
{
	return {
		(uint8_t)Effekseer::Clamp((int)(c.r * 255.0f), 255, 0),
		(uint8_t)Effekseer::Clamp((int)(c.g * 255.0f), 255, 0),
		(uint8_t)Effekseer::Clamp((int)(c.b * 255.0f), 255, 0),
		(uint8_t)Effekseer::Clamp((int)(c.a * 255.0f), 255, 0),
	};
}

inline godot::Color ToGdColor(Effekseer::Color c)
{
	return { c.R / 255.0f, c.G / 255.0f, c.B / 255.0f, c.A / 255.0f };
}

size_t ToEfkString(char16_t* to, const godot::String& from, size_t size);

godot::String ToGdString(const char16_t* from);

godot::Variant ScriptNew(godot::Ref<godot::Script> script);

}
