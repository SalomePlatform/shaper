// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketch.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintLength.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Events.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAPI_XYZ.h>

#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IViewer.h>
#include <Events_Loop.h>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationSketch::PartSet_OperationSketch(const QString& theId, QObject* theParent)
    : ModuleBase_Operation(theId, theParent)
{
}

PartSet_OperationSketch::~PartSet_OperationSketch()
{
}

bool PartSet_OperationSketch::isNestedOperationsEnabled() const
{
  bool aHasSketchPlane = false;
  if (feature()) {
    std::shared_ptr<ModelAPI_Data> aData = feature()->data();
    AttributeDoublePtr anAttr;
    std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    aHasSketchPlane = aNormal && !(aNormal->x() == 0 && aNormal->y() == 0 && aNormal->z() == 0);
  }
  return aHasSketchPlane;
}
