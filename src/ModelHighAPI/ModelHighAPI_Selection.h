// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_SELECTION_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_SELECTION_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <list>
#include <memory>
#include <string>
#include <utility>
//--------------------------------------------------------------------------------------
class GeomAPI_Pnt;
class GeomAPI_Shape;
class ModelAPI_AttributeSelection;
class ModelAPI_AttributeSelectionList;
class ModelAPI_FiltersFeature;
class ModelAPI_Result;
//--------------------------------------------------------------------------------------
typedef std::pair<std::shared_ptr<ModelAPI_Result>, std::shared_ptr<GeomAPI_Shape> >
  ResultSubShapePair;
typedef std::pair<std::string, std::string> TypeSubShapeNamePair;
typedef std::pair<std::string, std::shared_ptr<GeomAPI_Pnt> > TypeInnerPointPair;
typedef std::pair<std::string, std::pair<std::string, int> > TypeWeakNamingPair;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Selection
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeSelection
 */
class ModelHighAPI_Selection
{
public:
  enum VariantType {
    VT_Empty,
    VT_ResultSubShapePair,
    VT_TypeSubShapeNamePair,
    VT_TypeInnerPointPair,
    VT_WeakNamingPair,
    VT_Filtering
  };

public:
  /// Default constructor with empty selection.
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Selection();

  /// Constructor for result and sub-shape
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Selection(const std::shared_ptr<ModelAPI_Result>& theContext,
                         const std::shared_ptr<GeomAPI_Shape>& theSubShape =
                         std::shared_ptr<GeomAPI_Shape>());
  /// Constructor for sub-shape by the textual Name
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Selection(const std::string& theType,
                         const std::string& theSubShapeName);

  /// Constructor for sub-shape by inner point coordinates
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Selection(const std::string& theType,
                         const std::shared_ptr<GeomAPI_Pnt>& theSubShapeInnerPoint);

  /// Constructor for sub-shape by inner point coordinates given by a tuple
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Selection(const std::string& theType,
                         const std::list<double>& theSubShapeInnerPoint);


  /// Constructor for sub-shape by weak naming identifier
  MODELHIGHAPI_EXPORT
    ModelHighAPI_Selection(const std::string& theType,
      const std::string& theContextName, const int theIndex);

  /// Destructor
  MODELHIGHAPI_EXPORT
  virtual ~ModelHighAPI_Selection();

  /// Fill attribute values
  MODELHIGHAPI_EXPORT virtual
    void fillAttribute(const std::shared_ptr<ModelAPI_AttributeSelection> & theAttribute) const;

  /// Append to list attribute
  MODELHIGHAPI_EXPORT virtual
    void appendToList(const std::shared_ptr<ModelAPI_AttributeSelectionList> & theAttribute) const;

  /// \return variant type.
  MODELHIGHAPI_EXPORT
  virtual VariantType variantType() const;

  /// \return pair of result and sub-shape.
  MODELHIGHAPI_EXPORT
  virtual ResultSubShapePair resultSubShapePair() const;

  /// \return pair of sub-shape type and name.
  MODELHIGHAPI_EXPORT
  virtual TypeSubShapeNamePair typeSubShapeNamePair() const;

  /// \return pair of sub-shape type and inner point to identify sub-shape.
  MODELHIGHAPI_EXPORT
  virtual TypeInnerPointPair typeInnerPointPair() const;

  /// \return pair of sub-shape type and pair of context name and sub-shape index.
  MODELHIGHAPI_EXPORT
    virtual TypeWeakNamingPair typeWeakNamingPair() const;

  /// \return shape type.
  MODELHIGHAPI_EXPORT
  virtual std::string shapeType() const;

  /// Shortcut for result()->data()->setName()
  MODELHIGHAPI_EXPORT
  void setName(const std::string& theName);

  /// Shortcut for result()->data()->name()
  MODELHIGHAPI_EXPORT
  std::string name() const;

  /// Change result's color
  MODELHIGHAPI_EXPORT
  void setColor(int theRed, int theGreen, int theBlue);

  /// Change result's deflection
  MODELHIGHAPI_EXPORT
  void setDeflection(double theValue);

  /// Change result's transparency
  MODELHIGHAPI_EXPORT
  void setTransparency(double theValue);

  /// Returns the number of sub-elements.
  MODELHIGHAPI_EXPORT
  int numberOfSubs() const;

  /// Returns the sub-result by zero-base index.
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Selection subResult(int theIndex) const;

protected:
  VariantType myVariantType;
  ResultSubShapePair myResultSubShapePair;
  TypeSubShapeNamePair myTypeSubShapeNamePair;
  TypeInnerPointPair myTypeInnerPointPair;
  TypeWeakNamingPair myWeakNamingPair;
  std::shared_ptr<ModelAPI_FiltersFeature> myFilterFeature;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_SELECTION_H_ */
