#include "SceneViewerUtility.h"

void ConvertModel3DToProto(Model3D& src, sceneviewer::Model* dst)
{
	auto* pos = dst->mutable_position();
	pos->set_x(src.getPosition().x);
	pos->set_y(src.getPosition().y);
	pos->set_z(src.getPosition().z);

	auto* rot = dst->mutable_rotation();
	rot->set_x(src.getRotation().x);
	rot->set_y(src.getRotation().y);
	rot->set_z(src.getRotation().z);

	auto* scale = dst->mutable_scale();
	scale->set_x(src.getScale().x);
	scale->set_y(src.getScale().y);
	scale->set_z(src.getScale().z);

	auto* mesh = dst->mutable_mesh();
	const auto& vertices = src.getVertices();
	mesh->mutable_vertices()->Add(vertices.begin(), vertices.end());

	const auto& indices = src.getIndices();
	mesh->mutable_indices()->Add(indices.begin(), indices.end());
}

Model3D* ConvertProtoToModel3D(const sceneviewer::Model& src)
{
    glm::vec3 position(
        src.position().x(),
        src.position().y(),
        src.position().z()
    );
    glm::vec3 rotation(
        src.rotation().x(),
        src.rotation().y(),
        src.rotation().z()
    );
    glm::vec3 scale(
        src.scale().x(),
        src.scale().y(),
        src.scale().z()
    );

    const auto& mesh = src.mesh();
    std::vector<GLfloat> vertices(
        mesh.vertices().begin(),
        mesh.vertices().end()
    );

    std::vector<GLuint> indices(
        mesh.indices().begin(),
        mesh.indices().end()
    );

    return new Model3D(position, rotation, scale, vertices, indices);
}
