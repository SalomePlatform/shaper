// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef SketcherPrs_DimensionStyleListener_H
#define SketcherPrs_DimensionStyleListener_H

//#include <Events_Listener.h>

#include <ModelAPI_AttributeDouble.h>
#include <PrsDim_Dimension.hxx>

#include "SketcherPrs_Tools.h"

#include <Standard.hxx>

#include <string>

/**
* \class SketcherPrs_DimensionStyleListener
* \ingroup GUI
* A class for representation of linear dimension constraint.
* It supports SketchPlugin_ConstraintLength and SketchPlugin_ConstraintDistance features.
*/
class SketcherPrs_DimensionStyle
{
public:
  class DimensionValue {
  public:
    DimensionValue(double theDoubleValue, bool theHasParameters, const std::string& theTextValue);
    /// Fills internal fields by the given attribute
    /// \param theAttributeValue a model attribute
    void init(const AttributeDoublePtr& theAttributeValue);

  public:
    double myDoubleValue; ///< the angle value to be shown as custom value of presentation
    bool myHasParameters; ///< true if the atrribute value has used parameters
    std::string myTextValue; ///< the angle value depending on angle type
  };

public:
  /// Constructor
  Standard_EXPORT SketcherPrs_DimensionStyle();

  /// Destructor
  Standard_EXPORT ~SketcherPrs_DimensionStyle();

  /// Visualizes the dimension text or dimension value depending on the has parameters state
  /// \param theDimension a modified dimension
  /// \param theDimensionValue container filled by the model double attribute
  Standard_EXPORT void updateDimensions(PrsDim_Dimension* theDimension,
                                        const DimensionValue& theDimensionValue);

private:
  /// Visualizes the dimension text or dimension value depending on the has parameters state
  /// \param theDimension a modified dimension
  /// \param theHasParameters if true, the text is shown, else digit
  /// \param theTextValue a dimension text value
  /// \param theDoubleValue a dimension digit value
  void updateDimensions(PrsDim_Dimension* theDimension,
                        const bool theHasParameters,
                        const std::string& theTextValue,
                        const double theDoubleValue);
};

#endif