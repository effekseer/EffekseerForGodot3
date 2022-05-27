#pragma once

#include <Effekseer.h>
#include <Effekseer/Model/ProceduralModelGenerator.h>

namespace EffekseerGodot
{

class ProceduralModelGenerator : public ::Effekseer::ProceduralModelGenerator
{
public:
	ProceduralModelGenerator() = default;

	virtual ~ProceduralModelGenerator() = default;

protected:
	virtual Effekseer::ModelRef CreateModel(
		const Effekseer::CustomVector<Effekseer::Model::Vertex>& vs, 
		const Effekseer::CustomVector<Effekseer::Model::Face>& faces) override;
};

} // namespace EffekseerGodot

