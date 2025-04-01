#include "SceneViewerUtility.h"

Model3D* ProtoToModel(const sceneviewer::Model& proto_model)
{
	glm::vec3 position(
		proto_model.position().x(),
		proto_model.position().y(),
		proto_model.position().z()
	);

	glm::vec3 rotation(
		proto_model.rotation().x(),
		proto_model.rotation().y(),
		proto_model.rotation().z()
	);

	glm::vec3 scale(
		proto_model.scale().x(),
		proto_model.scale().y(),
		proto_model.scale().z()
	);

	const auto& proto_mesh = proto_model.mesh();
	std::vector<GLfloat> vertices(
		proto_mesh.vertices().begin(),
		proto_mesh.vertices().end()
	);

	std::vector<GLuint> indices(
		proto_mesh.indices().begin(),
		proto_mesh.indices().end()
	);

	return new Model3D(position, rotation, scale, vertices, indices);
}

sceneviewer::Model Model3DToProto(Model3D& model)
{
	sceneviewer::Model proto_model;

	auto& proto_pos = *proto_model.mutable_position();
	proto_pos.set_x(model.getPosition().x);
	proto_pos.set_y(model.getPosition().y);
	proto_pos.set_z(model.getPosition().z);

	auto& proto_rot = *proto_model.mutable_rotation();
	proto_rot.set_x(model.getRotation().x);
	proto_rot.set_y(model.getRotation().y);
	proto_rot.set_z(model.getRotation().z);

	auto& proto_scale = *proto_model.mutable_scale();
	proto_scale.set_x(model.getScale().x);
	proto_scale.set_y(model.getScale().y);
	proto_scale.set_z(model.getScale().z);

	auto& proto_mesh = *proto_model.mutable_mesh();
	const auto& vertices = model.getVertices();
	proto_mesh.mutable_vertices()->Add(vertices.begin(), vertices.end());

	const auto& indices = model.getIndices();
	proto_mesh.mutable_indices()->Add(indices.begin(), indices.end());

	return proto_model;
}

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
