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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GeomData_Point2D_H_
#define GeomData_Point2D_H_

#include "GeomData.h"
#include "GeomDataAPI_Point2D.h"

#include <TDF_Label.hxx>

class ModelAPI_ExpressionDouble;

/**\class GeomData_Point2D
 * \ingroup DataModel
 * \brief Attribute that contains 2D point.
 */

class GeomData_Point2D : public GeomDataAPI_Point2D
{
  enum { NUM_COMPONENTS = 2 };
  std::shared_ptr<ModelAPI_ExpressionDouble> myExpression[NUM_COMPONENTS]; ///< Expressions for X, Y
 public:
  /// Defines the double value
  GEOMDATA_EXPORT virtual void setValue(const double theX, const double theY);
  /// Defines the point
  GEOMDATA_EXPORT virtual void setValue(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Returns the X double value
  GEOMDATA_EXPORT virtual double x() const;
  /// Returns the Y double value
  GEOMDATA_EXPORT virtual double y() const;
  /// Returns the 2D point
  GEOMDATA_EXPORT virtual std::shared_ptr<GeomAPI_Pnt2d> pnt();

  /// Defines the calculated double value
  GEOMDATA_EXPORT virtual void setCalculatedValue(const double theX, const double theY);

  /// Defines the text values
  GEOMDATA_EXPORT virtual void setText(const std::string& theX,
                                       const std::string& theY);

  /// Returns the text values
  GEOMDATA_EXPORT virtual std::string textX();
  GEOMDATA_EXPORT virtual std::string textY();

  /// Allows to set expression (text) as invalid (by the parameters listener)
  GEOMDATA_EXPORT virtual void setExpressionInvalid(int, const bool theFlag);

  /// Returns true if text is invalid
  GEOMDATA_EXPORT virtual bool expressionInvalid(int);

  /// Allows to set expression (text) error (by the parameters listener)
  GEOMDATA_EXPORT virtual void setExpressionError(int theComponent, const std::string& theError);

  /// Returns an expression error
  GEOMDATA_EXPORT virtual std::string expressionError(int theComponent);

  /// Defines the used parameters
  GEOMDATA_EXPORT virtual void setUsedParameters(int theComponent,
    const std::set<std::string>& theUsedParameters);

  /// Returns the used parameters
  GEOMDATA_EXPORT virtual std::set<std::string> usedParameters(int theComponent) const;

 protected:
  /// Initializes attributes
  GEOMDATA_EXPORT GeomData_Point2D();
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  /// Resets attribute to deafult state.
  virtual void reset();

  friend class Model_Data;
};

#endif
