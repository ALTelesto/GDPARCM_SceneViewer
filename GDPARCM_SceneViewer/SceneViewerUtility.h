#pragma once
#include "sceneviewer.pb.h"
#include "Dependencies.h"

namespace tinyobj
{
	struct shape_t;
}

// Convert Mesh proto to tinyobj shape and GL indices
void ProtoToMesh(const sceneviewer::Mesh& mesh_proto,
    tinyobj::shape_t& shape,
    std::vector<GLuint>& mesh_indices) {
    /*
    // Clear existing data
    shape.mesh.indices.clear();
    shape.mesh.num_face_vertices.clear();
    mesh_indices.clear();

    // Convert vertices
    auto& attrib = shape.mesh.attributes;
    attrib.vertices.resize(mesh_proto.vertices_size() * 3);
    attrib.normals.resize(mesh_proto.vertices_size() * 3);

    for (int i = 0; i < mesh_proto.vertices_size(); i++) {
        const auto& v = mesh_proto.vertices(i);

        // Position
        attrib.vertices[i * 3] = v.position().x();
        attrib.vertices[i * 3 + 1] = v.position().y();
        attrib.vertices[i * 3 + 2] = v.position().z();

        // Normal
        attrib.normals[i * 3] = v.normal().x();
        attrib.normals[i * 3 + 1] = v.normal().y();
        attrib.normals[i * 3 + 2] = v.normal().z();
    }

    // Convert indices (OBJ uses face-vertex format)
    mesh_indices = std::vector<GLuint>(
        mesh_proto.indices().begin(),
        mesh_proto.indices().end()
    );

    // Setup faces (assuming triangles)
    size_t triangle_count = mesh_proto.indices_size() / 3;
    shape.mesh.num_face_vertices.resize(triangle_count, 3);
    shape.mesh.material_ids.resize(triangle_count, -1);

    for (size_t i = 0; i < mesh_proto.indices_size(); i++) {
        tinyobj::index_t idx;
        idx.vertex_index = mesh_proto.indices(i);
        idx.normal_index = mesh_proto.indices(i); // Assuming 1:1 mapping
        idx.texcoord_index = -1;
        shape.mesh.indices.push_back(idx);
    }
	*/
}

// Convert tinyobj shape to Mesh proto
sceneviewer::Mesh MeshToProto(const tinyobj::shape_t& shape,
    const std::vector<GLuint>& mesh_indices) {
    /*
    sceneviewer::Mesh mesh_proto;
    const auto& attrib = shape.mesh.attributes;

    // Convert vertices
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        auto* vertex = mesh_proto.add_vertices();

        // Position
        vertex->mutable_position()->set_x(attrib.vertices[i]);
        vertex->mutable_position()->set_y(attrib.vertices[i + 1]);
        vertex->mutable_position()->set_z(attrib.vertices[i + 2]);

        // Normal (if available)
        if (!attrib.normals.empty()) {
            vertex->mutable_normal()->set_x(attrib.normals[i]);
            vertex->mutable_normal()->set_y(attrib.normals[i + 1]);
            vertex->mutable_normal()->set_z(attrib.normals[i + 2]);
        }
    }

    // Convert indices (use either shape.indices or direct GLuint vector)
    if (!mesh_indices.empty()) {
        // Using explicit GLuint indices
        for (GLuint index : mesh_indices) {
            mesh_proto.add_indices(index);
        }
    }
    else {
        // Fallback to tinyobj indices
        for (const auto& index : shape.mesh.indices) {
            mesh_proto.add_indices(index.vertex_index);
        }
    }

    return mesh_proto;
	*/
    return Mesh();
}