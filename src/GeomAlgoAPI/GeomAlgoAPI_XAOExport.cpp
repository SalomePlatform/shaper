// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_XAOExport.cpp
// Created: Nov 27, 2015
// Author:  Sergey POKHODENKO

#include <GeomAlgoAPI_XAOExport.h>

#include "GeomAlgoAPI_Tools.h"

#include <TopoDS_Shape.hxx>

#include <XAO_XaoExporter.hxx>
#include <XAO_BrepGeometry.hxx>

//=============================================================================
bool SetShapeToXAO(const std::shared_ptr<GeomAPI_Shape>& theShape,
                   XAO::Xao* theXao,
                   std::string& theError)
{
  if (!theShape.get() || !theXao) {
    theError = "An invalid argument.";
    return false;
  }

  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  try {
    XAO::BrepGeometry* aGeometry = new XAO::BrepGeometry;
    theXao->setGeometry(aGeometry);
    aGeometry->setTopoDS_Shape(aShape);
  } catch (XAO::XAO_Exception& e) {
    theError = e.what();
    return false;
  }
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool XAOExport(const std::string& theFileName,
               XAO::Xao* theXao,
               std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "Export XAO into file " << theFileName << std::endl;
  #endif

  if (theFileName.empty() || !theXao) {
    theError = "An invalid argument.";
    return false;
  }

  try {
    XAO::XaoExporter::saveToFile(theXao, theFileName);
  } catch (XAO::XAO_Exception& e) {
    theError = e.what();
    return false;
  }
  return true;
}
