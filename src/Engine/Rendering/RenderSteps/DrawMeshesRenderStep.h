#pragma once

#include "RenderStep.h"

#include <Engine/Resource/Material/Material.h>

#include <glm/glm.hpp>

#include <memory>

namespace tactics {
namespace components {
struct Transform;
struct Mesh;
struct Material;
}

namespace resource {
class Mesh;
}

class Camera;
class EcsSystem;
}

namespace tactics::renderstep {

enum class AlphaBlendedFlag {
	WithoutAlphaBlend,
	WithAlphaBlend
};

/**
* @brief The DrawMeshes class is a render step that draws all meshes in the scene.
* It uses the ECS system to find all entities with a Transform, Mesh and Material component.
* It also supports drawing alpha blended meshes. If the drawAlphaBlendedMeshes flag is set to WithAlphaBlend
* it will draw all meshes with an AlphaBlended component.
*/
class DrawMeshes: public RenderStep {
public:
	DrawMeshes(EcsSystem& ecsSystem, AlphaBlendedFlag drawAlphaBlendedMeshes);
	void execute(RenderStepInfo& renderInfo) override;

private:
	void _drawMesh(const glm::mat4x4& viewProjection, components::Transform& transform, const components::Mesh& mesh);
	void _drawGeometry(const resource::Mesh& mesh);
	void _drawOpaqueGeometry(const glm::mat4x4& viewProjection);
	void _drawAlphaBlendedGeometry(const glm::mat4x4& viewProjection, RenderStepInfo& renderInfo);

	EcsSystem& _ecsSystem;
	AlphaBlendedFlag _alphaBlendedFlag{};
};

}
