#pragma once

#include <Libs/Rendering/RenderStep/RenderStep.h>
#include <Libs/Resource/Material/Material.h>

#include <glm/glm.hpp>

#include <memory>

namespace tactics {
namespace component {
struct Camera;
struct Mesh;
struct Transform;
}

namespace resource {
class Mesh;
class SubMesh;
}

class EntityComponentSystem;
}

namespace tactics::renderstep {

enum class AlphaBlendedFlag {
	WithoutAlphaBlend,
	WithAlphaBlend
};

class DrawMeshes: public RenderStep {
public:
	DrawMeshes(EntityComponentSystem& ecs, AlphaBlendedFlag drawAlphaBlendedMeshes);
	void execute(RenderStepInfo& renderInfo) override;

private:
	void _drawMesh(const glm::mat4x4& viewProjection, component::Transform& transform, const component::Mesh& mesh, bool filterTransparent);
	void _drawGeometry(const resource::SubMesh& mesh);
	void _drawOpaqueGeometry(const glm::mat4x4& viewProjection);
	void _drawAlphaBlendedGeometry(const glm::mat4x4& viewProjection, component::Transform& cameraTransform);
	void _drawSprite(const glm::mat4x4& viewProjection);

	EntityComponentSystem& _ecs;
	AlphaBlendedFlag _alphaBlendedFlag{};
};

}
