#pragma once
#include "sceneviewer.pb.h"
#include "Model3D.h"

void ConvertModel3DToProto(Model3D& src, sceneviewer::Model* dst);
Model3D* ConvertProtoToModel3D(const sceneviewer::Model& src);