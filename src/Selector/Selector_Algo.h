// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Selector_Algo_H_
#define Selector_Algo_H_

#include "Selector.h"

#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <TopoDS_Shape.hxx>

class Selector_NameGenerator;

/**\class Selector_Selector
 * \ingroup DataModel
 * \brief Base class for all kinds of selection algorithms.
 */
class Selector_Algo
{
  TopAbs_ShapeEnum myShapeType; ///< type of this shape

  TDF_Label myLab; ///< label where this also may be located
  TDF_Label myBaseDocumentLab; ///< an access-label of the document that may contain initial shapes
  bool myGeometricalNaming; ///< flag that indicates that geometrical naming selection is enabled
  bool myAlwaysGeometricalNaming; ///< to enable geometrical naming from beginning, at select
  bool myUseNeighbors; ///< to use neighbors algorithms
  bool myUseIntersections; ///< to use intersections algorithms

public:
  /// Type of a selector algorithm: on this type depends what is stored in this label and how to
  /// restore it on update.
  enum Selector_Type {
    SELTYPE_CONTAINER, ///< just a container of sub-elements, keeps the shape type of container
    SELTYPE_INTERSECT, ///< sub-shape is intersection of higher level objects
    SELTYPE_PRIMITIVE, ///< sub-shape found as a primitive on some label
    SELTYPE_MODIFICATION, ///< modification of base shapes to the final label
    SELTYPE_FILTER_BY_NEIGHBOR,  ///< identification by neighbor shapes in context
    SELTYPE_WEAK_NAMING, ///< pure weak naming by weak index in context
  };

  /// Initializes the algorithm
  SELECTOR_EXPORT Selector_Algo();

  /// Initializes the selector structure on the label.
  /// Stores the name data to restore after modification.
  /// \param theContext whole shape that contains the selected sub-shape
  /// \param theValue selected subshape
  /// \param theGeometricalNaming treats selection with equal surfaces as one
  /// \param theUseNeighbors enables searching algorithm by neighbors
  /// \param theUseIntersections enables searching algorithm by intersection of higher level shapes
  SELECTOR_EXPORT static Selector_Algo* select(
    const TopoDS_Shape theContext, const TopoDS_Shape theValue,
    const TDF_Label theAccess, const TDF_Label theBaseDocument,
    const bool theGeometricalNaming = false,
    const bool theUseNeighbors = true, const bool theUseIntersections = true,
    const bool theAlwaysGeometricalNaming = false);

  /// Stores the name to the label and sub-labels tree
  SELECTOR_EXPORT virtual void store() = 0;

  /// Restores the selected shape by the topological naming kept in the data structure
  /// Returns true if it can restore structure correctly
  SELECTOR_EXPORT virtual bool restore() = 0;

  /// Restores the selected shape by the topological name string.
  /// Returns not empty label of the context.
  SELECTOR_EXPORT virtual TDF_Label restoreByName(std::string theName,
    const TopAbs_ShapeEnum theShapeType, Selector_NameGenerator* theNameGenerator) = 0;

  /// Updates the current shape by the stored topological name
  SELECTOR_EXPORT virtual bool solve(const TopoDS_Shape& theContext) = 0;

  /// Returns the naming name of the selection
  SELECTOR_EXPORT virtual std::string name(Selector_NameGenerator* theNameGenerator) = 0;
  /// Returns the current sub-shape value (null if can not resolve)
  SELECTOR_EXPORT TopoDS_Shape value();
  /// Restores sub-algorithm of a given type by the storage-label
  SELECTOR_EXPORT static Selector_Algo* restoreByLab(TDF_Label theLab, TDF_Label theBaseDocLab);
  /// Restores the selected sub-algorithm by the naming name.
  /// Returns not empty label of the context.
  SELECTOR_EXPORT static Selector_Algo* restoreByName(
    TDF_Label theLab, TDF_Label theBaseDocLab, std::string theName,
    const TopAbs_ShapeEnum theShapeType, const bool theGeomNaming,
    Selector_NameGenerator* theNameGenerator, TDF_Label& theContextLab);

  /// Returns true if the given shapes are based on the same geometry
  static bool sameGeometry(const TopoDS_Shape theShape1, const TopoDS_Shape theShape2);

  /// Creates a new selection algorithm for selection of all topology based on the same geometry
  SELECTOR_EXPORT static Selector_Algo* relesectWithAllGeometry(
    Selector_Algo* theOldAlgo, const TopoDS_Shape theContext);
  /// Sets geometrical naming flag to true
  void setGeometricalNaming()
  {myGeometricalNaming = true;}

protected:
  /// Returns label where this algorithm is attached to, or just an access label to the document
  const TDF_Label& label() const
  {return myLab;}
  /// Stores the array of references to theLab: references to elements of ref-list, then the last
  void storeBaseArray(const TDF_LabelList& theRef, const TDF_Label& theLast);
  /// Restores references to the labels: references to elements of ref-list, then the last
  bool restoreBaseArray(TDF_LabelList& theRef, TDF_Label& theLast);
  /// Stores result of selection at the given label
  void store(const TopoDS_Shape theShape);
  /// Returns an access-label of the document that may contain initial shapes
  const TDF_Label& baseDocument() const
  {return myBaseDocumentLab;}
  /// Returns the geometrical naming flag
  bool geometricalNaming() const
  {return myGeometricalNaming;}
  /// Returns always geometrical naming flag
  bool alwaysGeometricalNaming() const
  {return myAlwaysGeometricalNaming;}
  /// Returns use neighbors flag
  bool useNeighbors() const
  {return myUseNeighbors;}
  /// Returns use intersections flag
  bool useIntersections() const
  {return myUseIntersections;}
  /// Returns GUID for the weak index (integer attribute) of the sub-shape
  static const Standard_GUID& weakID()
  {
    static const Standard_GUID kWEAK_INDEX("e9373a61-cabc-4ee8-aabf-aea47c62ed87");
    return kWEAK_INDEX;
  }
  /// Returns GUID for the type of the shape, stored in case it is intersection or container
  static const Standard_GUID& shapeTypeID()
  {
    static const Standard_GUID kSHAPE_TYPE("864b3267-cb9d-4107-bf58-c3ce1775b171");
    return kSHAPE_TYPE;
  }
  /// string identifier of the weak name in modification or intersection types of algorithm
  static const std::string& weakNameID()
  {
    static const std::string kWEAK_NAME_IDENTIFIER = "weak_name_";
    return kWEAK_NAME_IDENTIFIER;
  }
  /// string identifier of the pure weak name
  static const std::string& pureWeakNameID()
  {
    static const std::string kPURE_WEAK_NAME_IDENTIFIER = "_weak_name_";
    return kPURE_WEAK_NAME_IDENTIFIER;
  }
  /// Stores the type of an algorithm in the data tree (in myLab)
  void storeType(const Selector_Type theType);

  /// Searches the newer version of the shape in the document if the base shape does not
  /// belong to context. Returns it in theResult (if any). Returns true is theResult is changed.
  bool findNewVersion(const TopoDS_Shape& theContext, TopoDS_Shape& theResult) const;
};

#endif
