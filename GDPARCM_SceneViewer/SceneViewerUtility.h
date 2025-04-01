#pragma once
#include "sceneviewer.pb.h"
#include "Model3D.h"

Model3D* ProtoToModel(const sceneviewer::Model& proto_model);

sceneviewer::Model Model3DToProto(Model3D& model);

void ConvertModel3DToProto(const Model3D& src, sceneviewer::Model* dst);
