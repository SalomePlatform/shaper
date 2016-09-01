// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_SelectionNaming.h
// Created:     11 Aug 2015
// Author:      Mikhail PONIKAROV

#ifndef Model_SelectionNaming_H_
#define Model_SelectionNaming_H_

#include <ModelAPI_Result.h>
#include <GeomAPI_Shape.h>
#include <Model_Document.h>
#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>
#include <Geom_Curve.hxx>
#include <TopoDS_Edge.hxx>

/**\class Model_SelectionNaming
 * \ingroup DataModel
 * \brief The set of methods that allow to generate a string-name for the selection attribute.
 */

class Model_SelectionNaming
{
  TDF_Label myLab; ///< Selection label of the selection attribute
public:
  /// Constructor for the selection attribute by the selection label
  Model_SelectionNaming(TDF_Label theSelectionLab);

  /// Produces the string-name for the selected shape
  std::string namingName(ResultPtr& theContext, std::shared_ptr<GeomAPI_Shape> theSubSh,
    const std::string& theDefaultName, const bool theAnotherDoc);
  
  /// Makes a selection by the string-name
  /// \param theType string of the type of the shape
  /// \param theSubShapeName string-identifier of the selected shape
  /// \param theDoc document where the selected shape is searched
  /// \param theShapeToBeSelected resulting selected shape
  /// \param theCont the selection context of the resulting selected shape
  bool selectSubShape(const std::string& theType, const std::string& theSubShapeName,
    std::shared_ptr<Model_Document> theDoc, std::shared_ptr<GeomAPI_Shape>& theShapeToBeSelected,
    std::shared_ptr<ModelAPI_Result>& theCont);

  /// Searches the face more appropriate to the given curves (with higher level of matched parameters)
  /// \param theConstr construction result that contains one or several  faces
  /// \param theCurves map from the face edges curves to orientation (-1 reversed, 0 unknown, 1 forward)
  /// \returns faces fron this construction if found
  static std::shared_ptr<GeomAPI_Shape> findAppropriateFace(
    std::shared_ptr<ModelAPI_Result>& theConstr, 
    NCollection_DataMap<Handle(Geom_Curve), int>& theCurves);

  /// Returns orientation of the edge in the context shape
  static int edgeOrientation(const TopoDS_Shape& theContext, TopoDS_Edge& theEdge);

  /// Returns the name of sketch sub-element, shortened by exclusion of some symbols and with added
  /// the vertex position (if needed)
  /// \param theConstr result with name - basis for the name
  /// \param theEdgeVertexPos position of the vertex on edge: 1 - first , 2 - second
  /// \returns the generated name
  static std::string shortName(std::shared_ptr<ModelAPI_ResultConstruction>& theConstr,
    const int theEdgeVertexPos = 0);

protected:
  /// Gets the stored name from the document
  std::string getShapeName(std::shared_ptr<Model_Document> theDoc, const TopoDS_Shape& theShape,
    const bool theAddContextName);
};

#endif
