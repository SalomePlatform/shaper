// File:        SketchPlugin_Sketch.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "SketchPlugin_Sketch.h"
#include "ModelAPI_Data.h"
#include "ModelAPI_AttributeDocRef.h"

using namespace std;
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

const double PLANE_U_MIN = -100;
const double PLANE_U_MAX = 100;
const double PLANE_V_MIN = -100;
const double PLANE_V_MAX = 100;

SketchPlugin_Sketch::SketchPlugin_Sketch()
{
}

void SketchPlugin_Sketch::initAttributes()
{
  data()->addAttribute(PART_ATTR_DOC_REF, ModelAPI_AttributeDocRef::type());
}

void SketchPlugin_Sketch::execute() 
{
  shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->docRef(PART_ATTR_DOC_REF);
  if (!aDocRef->value()) { // create a document if not yet created
    shared_ptr<ModelAPI_Document> aPartSetDoc = ModelAPI_PluginManager::get()->rootDocument();
    aDocRef->setValue(aPartSetDoc->subDocument(data()->getName()));
  }
}

shared_ptr<ModelAPI_Document> SketchPlugin_Sketch::documentToAdd() {
  return ModelAPI_PluginManager::get()->rootDocument();
}

const TopoDS_Shape& SketchPlugin_Sketch::preview()
{
  if (SketchPlugin_Feature::preview().IsNull())
  {
    gp_Pnt anOrigin(0, 0, 0);
    gp_Dir aDir(gp_Vec(gp_Pnt(0,0,0), gp_Pnt(1,0,0)));
    gp_Pln aPlane(anOrigin, aDir);
    BRepBuilderAPI_MakeFace aFaceBuilder(aPlane, PLANE_U_MIN, PLANE_U_MAX, PLANE_V_MIN,
                                         PLANE_V_MAX);
    setPreview(aFaceBuilder.Face());
  }
  return SketchPlugin_Feature::preview();
}
