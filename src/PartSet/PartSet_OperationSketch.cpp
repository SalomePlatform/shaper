// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketch.h>

#include <SketchPlugin_Sketch.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>

#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_OperationSketch::PartSet_OperationSketch(const QString& theId,
	                                             QObject* theParent)
: PartSet_OperationSketchBase(theId, theParent)
{
}

PartSet_OperationSketch::~PartSet_OperationSketch()
{
}

int PartSet_OperationSketch::getSelectionMode() const
{
  int aMode = TopAbs_FACE;
  if (isEditMode())
    aMode = TopAbs_VERTEX;
  return aMode;
}

void PartSet_OperationSketch::setSelectedShapes(const NCollection_List<TopoDS_Shape>& theList)
{
  if (theList.IsEmpty())
    return;

  if (isEditMode())
    return;

  // get selected shape
  const TopoDS_Shape& aShape = theList.First();
  boost::shared_ptr<GeomAPI_Shape> aGShape(new GeomAPI_Shape);
  aGShape->setImpl(new TopoDS_Shape(aShape));

  // get plane parameters
  boost::shared_ptr<GeomAPI_Pln> aPlane = GeomAlgoAPI_FaceBuilder::plane(aGShape);

  // set plane parameters to feature
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  double anA, aB, aC, aD;
  aPlane->coefficients(anA, aB, aC, aD);

  boost::shared_ptr<ModelAPI_AttributeDouble> anAttr;
  /*
  aData->real(SKETCH_ATTR_PLANE_A)->setValue(anA);
  aData->real(SKETCH_ATTR_PLANE_B)->setValue(aB);
  aData->real(SKETCH_ATTR_PLANE_C)->setValue(aC);
  aData->real(SKETCH_ATTR_PLANE_D)->setValue(aD);
  */
  // temporary solution for main planes only
  boost::shared_ptr<GeomDataAPI_Point> anOrigin = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(aData->attribute(SKETCH_ATTR_ORIGIN));
  anOrigin->setValue(0, 0, 0);
  boost::shared_ptr<GeomDataAPI_Dir> aNormal = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_NORM));
  aNormal->setValue(anA, aB, aC);
  boost::shared_ptr<GeomDataAPI_Dir> aDirX = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRX));
  aDirX->setValue(aB, aC, anA);
  boost::shared_ptr<GeomDataAPI_Dir> aDirY = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRY));
  aDirY->setValue(aC, anA, aB);

  boost::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
  emit planeSelected(aDir->x(), aDir->y(), aDir->z());

  //commit();
  //SketchPlugin_Sketch::setActive(myFeature);
}
