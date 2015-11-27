// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_XAOImport.cpp
// Created: Nov 25, 2015
// Author:  Sergey POKHODENKO

#include <GeomAlgoAPI_XAOImport.h>

#include <TopoDS_Shape.hxx>

#include <XAO_XaoExporter.hxx>
#include <XAO_BrepGeometry.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================
std::shared_ptr<GeomAPI_Shape> XAOImport(const std::string& theFileName,
                                         std::string& theError,
                                         XAO::Xao* theXao)
{
  #ifdef _DEBUG
  std::cout << "Import XAO from file " << theFileName << std::endl;
  #endif
  if (theFileName.empty() || !theXao) {
    theError = "An invalid argument.";
    return std::shared_ptr<GeomAPI_Shape>();
  }

  TopoDS_Shape aShape;
  try {
    if (XAO::XaoExporter::readFromFile(theFileName, theXao)) {
      XAO::Geometry* aGeometry = theXao->getGeometry();
      XAO::Format aFormat = aGeometry->getFormat();
      if (aFormat == XAO::BREP) {
        if (XAO::BrepGeometry* aBrepGeometry = dynamic_cast<XAO::BrepGeometry*>(aGeometry))
          aShape = aBrepGeometry->getTopoDS_Shape();
      } else {
        theError = "Unsupported XAO geometry format:" + XAO::XaoUtils::shapeFormatToString(aFormat);
        aShape.Nullify();
      }
    } else {
      theError = "XAO object was not read successful";
      aShape.Nullify();
    }
  } catch (XAO::XAO_Exception& e) {
    theError = e.what();
    aShape.Nullify();
  }

  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
  aGeomShape->setImpl(new TopoDS_Shape(aShape));
  return aGeomShape;
}
