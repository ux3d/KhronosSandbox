#include "HelperUpdate.h"

#include "HelperAccess.h"


bool HelperUpdate::update(Primitive& primitive, GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	primitive.worldMatrix = parentWorldMatrix;

	return true;
}

bool HelperUpdate::update(Mesh& mesh, GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	for (size_t i = 0; i < mesh.primitives.size(); i++)
	{
		if (!HelperUpdate::update(mesh.primitives[i], glTF, parentWorldMatrix))
		{
			return false;
		}
	}

	return true;
}

bool HelperUpdate::update(Node& node, GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	glm::mat4 matrixTranslation = glm::translate(node.translation);
	glm::mat4 matrixRotation = glm::toMat4(node.rotation);
	glm::mat4 matrixScale = glm::scale(node.scale);

	node.matrix = matrixTranslation * matrixRotation * matrixScale;

	node.worldMatrix = parentWorldMatrix * node.matrix;

	if (node.mesh >= 0)
	{
		if (!HelperUpdate::update(glTF.meshes[node.mesh], glTF, node.worldMatrix))
		{
			return false;
		}
	}

	for (size_t i = 0; i < node.children.size(); i++)
	{
		if (!HelperUpdate::update(glTF.nodes[node.children[i]], glTF, node.worldMatrix))
		{
			return false;
		}
	}

	return true;
}

bool HelperUpdate::update(Scene& scene, GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	for (size_t i = 0; i < scene.nodes.size(); i++)
	{
		if (!HelperUpdate::update(glTF.nodes[scene.nodes[i]], glTF, parentWorldMatrix))
		{
			return false;
		}
	}

	return true;
}

bool HelperUpdate::update(GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	if (glTF.defaultScene < glTF.scenes.size())
	{
		if (!HelperUpdate::update(glTF.scenes[glTF.defaultScene], glTF, parentWorldMatrix))
		{
			return false;
		}
	}

	return true;
}
