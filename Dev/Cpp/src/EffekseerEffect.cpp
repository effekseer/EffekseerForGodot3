#include <Engine.hpp>
#include <File.hpp>
#include <ResourceLoader.hpp>
#include "EffekseerSystem.h"
#include "EffekseerEffect.h"
#include "Utils/EffekseerGodot.Utils.h"
#include "RendererGodot/EffekseerGodot.Shader.h"
#include "../Effekseer/Effekseer/IO/Effekseer.EfkEfcFactory.h"

namespace godot {

void EffekseerEffect::_register_methods()
{
	register_method("_init", &EffekseerEffect::_init);
	register_method("import", &EffekseerEffect::import);
	register_method("load", &EffekseerEffect::load);
	register_method("release", &EffekseerEffect::release);
	register_property<EffekseerEffect, PoolByteArray>("data_bytes", 
		&EffekseerEffect::set_data_bytes, &EffekseerEffect::get_data_bytes, {});
	register_property<EffekseerEffect, Dictionary>("subresources", 
		&EffekseerEffect::set_subresources, &EffekseerEffect::get_subresources, {});
	register_property<EffekseerEffect, float>("scale", 
		&EffekseerEffect::set_scale, &EffekseerEffect::get_scale, 1.0f);
}

EffekseerEffect::EffekseerEffect()
{
}

EffekseerEffect::~EffekseerEffect()
{
	release();
}

void EffekseerEffect::_init()
{
}

void EffekseerEffect::import(String path, bool shrink_binary)
{
	Ref<File> file = File::_new();
	if (file->open(path, File::READ) != Error::OK) {
		Godot::print_error(String("Failed open file: ") + path, __FUNCTION__, "", __LINE__);
		return;
	}

	int64_t size = file->get_len();
	PoolByteArray bytes = file->get_buffer(size);
	file->close();

	if (shrink_binary) {
		auto read = bytes.read();
		Effekseer::EfkEfcFile efkefc(read.ptr(), (int32_t)bytes.size());
		if (efkefc.IsValid()) {
			auto binChunk = efkefc.ReadRuntimeData();
			m_data_bytes.resize(binChunk.size);
			auto write = m_data_bytes.write();
			memcpy(write.ptr(), binChunk.data, binChunk.size);
		}
	} else {
		m_data_bytes = bytes;
	}

	auto setting = Effekseer::Setting::Create();
	auto native = Effekseer::Effect::Create(setting, m_data_bytes.read().ptr(), (int32_t)m_data_bytes.size());
	if (native == nullptr) {
		Godot::print_error(String("Failed load effect: ") + get_path(), __FUNCTION__, "", __LINE__);
		return;
	}

	auto nativeptr = native.Get();

	String materialDir = path.substr(0, path.find_last("/") + 1);
	auto loader = ResourceLoader::get_singleton();
	
	auto enumerateResouces = [&](const char16_t* (Effekseer::Effect::*getter)(int) const, int count){
		for (int i = 0; i < count; i++) {
			String path = EffekseerGodot::ToGdString((nativeptr->*getter)(i));
			m_subresources[path] = loader->load(materialDir + path);
		}
	};

	enumerateResouces(&Effekseer::Effect::GetColorImagePath, native->GetColorImageCount());
	enumerateResouces(&Effekseer::Effect::GetNormalImagePath, native->GetNormalImageCount());
	enumerateResouces(&Effekseer::Effect::GetDistortionImagePath, native->GetDistortionImageCount());
	enumerateResouces(&Effekseer::Effect::GetModelPath, native->GetModelCount());
	enumerateResouces(&Effekseer::Effect::GetCurvePath, native->GetCurveCount());
	enumerateResouces(&Effekseer::Effect::GetMaterialPath, native->GetMaterialCount());
	enumerateResouces(&Effekseer::Effect::GetWavePath, native->GetWaveCount());
}

void EffekseerEffect::load()
{
	if (m_native != nullptr) return;

	auto [system, manager] = EffekseerSystem::get_instance_manager();
	if (manager == nullptr) return;

	String path = get_path();
	String basePath = path.substr(0, path.find_last("/") + 1);
	char16_t materialPath[1024];
	EffekseerGodot::ToEfkString(materialPath, basePath, sizeof(materialPath) / sizeof(materialPath[0]));

	m_native = Effekseer::Effect::Create(manager, 
		m_data_bytes.read().ptr(), (int32_t)m_data_bytes.size(), m_scale, materialPath);
	if (m_native == nullptr) {
		Godot::print_error(String("Failed load effect: ") + get_path(), __FUNCTION__, "", __LINE__);
		return;
	}

	if (m_targetLayer == TargetLayer::Both) {
		setup_node_render(m_native->GetRoot(), TargetLayer::_2D);
		setup_node_render(m_native->GetRoot(), TargetLayer::_3D);
	} else {
		setup_node_render(m_native->GetRoot(), m_targetLayer);
	}

	if (!Engine::get_singleton()->is_editor_hint()) {
		// Release data bytes memory
		m_data_bytes = PoolByteArray();
	}
}

void EffekseerEffect::release()
{
	m_native.Reset();
}

void EffekseerEffect::setup_node_render(Effekseer::EffectNode* node, TargetLayer targetLayer)
{
	auto [system, manager] = EffekseerSystem::get_instance_manager();
	if (system == nullptr) return;

	const auto nodeType = node->GetType();

	const bool isRenderable =
		nodeType == Effekseer::EffectNodeType::Sprite ||
		nodeType == Effekseer::EffectNodeType::Ribbon ||
		nodeType == Effekseer::EffectNodeType::Ring ||
		nodeType == Effekseer::EffectNodeType::Track ||
		nodeType == Effekseer::EffectNodeType::Model;

	if (isRenderable) {
		const auto renderParams = node->GetBasicRenderParameter();
		const auto modelParams = node->GetEffectModelParameter();
		const bool isModel = nodeType == Effekseer::EffectNodeType::Model;

		EffekseerGodot::Shader* shader = nullptr;

		if (renderParams.MaterialType == Effekseer::RendererMaterialType::File) {
			auto material = m_native->GetMaterial(renderParams.MaterialIndex);
			if (material.Get() != nullptr) {
				if (nodeType == Effekseer::EffectNodeType::Model) {
					shader = static_cast<EffekseerGodot::Shader*>(material->ModelUserPtr);
				}
				else {
					shader = static_cast<EffekseerGodot::Shader*>(material->UserPtr);
				}
			}
		}
		else {
			switch (renderParams.MaterialType) {
			case Effekseer::RendererMaterialType::Default:
				shader = system->get_builtin_shader(isModel, EffekseerRenderer::RendererShaderType::Unlit);
				break;
			case Effekseer::RendererMaterialType::Lighting:
				shader = system->get_builtin_shader(isModel, EffekseerRenderer::RendererShaderType::Lit);
				break;
			case Effekseer::RendererMaterialType::BackDistortion:
				shader = system->get_builtin_shader(isModel, EffekseerRenderer::RendererShaderType::BackDistortion);
				break;
			}
		}

		if (shader != nullptr) {
			const Effekseer::CullingType cullingType = (isModel) ?
				modelParams.Culling : Effekseer::CullingType::Double;

			if (targetLayer == TargetLayer::_2D) {
				const EffekseerGodot::Shader::RenderType shaderType =
					EffekseerGodot::Shader::RenderType::CanvasItem;

				if (!shader->HasRID(shaderType, renderParams.ZTest, renderParams.ZWrite, renderParams.AlphaBlend, cullingType)) {
					RID shaderRID = shader->GetRID(shaderType, renderParams.ZTest, renderParams.ZWrite, renderParams.AlphaBlend, cullingType);
					system->load_shader(EffekseerSystem::ShaderLoadType::CanvasItem, shaderRID);
				}
			}
			else if (targetLayer == TargetLayer::_3D) {
				const bool hasSoftparticle =
					renderParams.SoftParticleDistanceFar != 0.0f ||
					renderParams.SoftParticleDistanceNear != 0.0f ||
					renderParams.SoftParticleDistanceNearOffset != 0.0f;
				const EffekseerGodot::Shader::RenderType shaderType = hasSoftparticle ?
					EffekseerGodot::Shader::RenderType::SpatialDepthFade : EffekseerGodot::Shader::RenderType::SpatialLightweight;

				if (!shader->HasRID(shaderType, renderParams.ZTest, renderParams.ZWrite, renderParams.AlphaBlend, cullingType)) {
					RID shaderRID = shader->GetRID(shaderType, renderParams.ZTest, renderParams.ZWrite, renderParams.AlphaBlend, cullingType);
					if (isModel) {
						system->load_shader(EffekseerSystem::ShaderLoadType::SpatialModel, shaderRID);
					}
					else {
						system->load_shader(EffekseerSystem::ShaderLoadType::SpatialStandard, shaderRID);
					}
				}
			}
		}
	}

	// Setup all children
	for (int childIndex = 0, childrenCount = node->GetChildrenCount();
		childIndex < childrenCount; childIndex++)
	{
		setup_node_render(node->GetChild(childIndex), targetLayer);
	}
}

}
