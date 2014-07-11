// File:        Model_ResultBody.cpp
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultBody.h>
#include <Model_Data.h>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>
#include <GeomAPI_Shape.h>

Model_ResultBody::Model_ResultBody(const boost::shared_ptr<ModelAPI_Feature>& theFeature)
  : myOwner(theFeature)
{
}

void Model_ResultBody::store(const boost::shared_ptr<GeomAPI_Shape>& theShape)
{
  boost::shared_ptr<Model_Data> aData =  boost::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // TODO: to add the naming mechanism for shape storage in the next iteration
    TNaming_Builder aBuilder(aShapeLab);
    if (!theShape) return; // bad shape
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (aShape.IsNull()) return; // null shape inside

    aBuilder.Generated(aShape);
  }
}

boost::shared_ptr<GeomAPI_Shape> Model_ResultBody::shape()
{
  boost::shared_ptr<Model_Data> aData =  boost::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    Handle(TNaming_NamedShape) aName;
    if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
      TopoDS_Shape aShape = aName->Get();
      if (!aShape.IsNull()) {
        boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
        aRes->setImpl(new TopoDS_Shape(aShape));
        return aRes;
      }
    }
  }
  return boost::shared_ptr<GeomAPI_Shape>();
}

boost::shared_ptr<ModelAPI_Feature> Model_ResultBody::owner()
{
  return myOwner;
}
