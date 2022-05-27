#include "EffekseerGodot.ProceduralModelGenerator.h"
#include "../RendererGodot/EffekseerGodot.RenderResources.h"

namespace EffekseerGodot
{
	
Effekseer::ModelRef ProceduralModelGenerator::CreateModel(
	const Effekseer::CustomVector<Effekseer::Model::Vertex>& vs, 
	const Effekseer::CustomVector<Effekseer::Model::Face>& faces)
{
	return ::Effekseer::MakeRefPtr<Model>(vs, faces);
}

} // namespace EffekseerGodot

