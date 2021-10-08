// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef ModelAPI_Result_H_
#define ModelAPI_Result_H_

#include "ModelAPI_Object.h"

class GeomAPI_Shape;
class ModelAPI_Feature;

/**\class ModelAPI_Result
 * \ingroup DataModel
 * \brief The result of a feature.
 *
 * A generic class that .
 */
class ModelAPI_Result : public ModelAPI_Object
{
  bool myIsConcealed; ///< the result is concealed from the data tree (referenced by other objects)
 protected:
  bool myIsDisabled; ///< the result is disabled: removed for the user, but keeps the general info
 public:

  /// Reference to the color of the result.
  /// The integer array is used. It contains three values for red, green and blue values.
  /// The values are in [0, 255] range
  inline static const std::string& COLOR_ID()
  {
    static const std::string MY_COLOR_ID("Color");
    return MY_COLOR_ID;
  }

  /// Reference to the deflection of the result.
  /// The double value is used. The value is in [0, 1] range
  inline static const std::string& DEFLECTION_ID()
  {
    static const std::string MY_DEFLECTION_ID("Deflection");
    return MY_DEFLECTION_ID;
  }

  /// Reference to the transparency of the result.
  /// The double value is used. The value is in [0, 1] range
  inline static const std::string& TRANSPARENCY_ID()
  {
    static const std::string MY_TRANSPARENCY_ID("Transparency");
    return MY_TRANSPARENCY_ID;
  }

  /// Reference to the transparency of the result.
  /// The double value is used. The value is in [0, 1] range
  inline static const std::string& ISO_LINES_ID()
  {
    static const std::string MY_ISO_LINES_ID("Iso_lines");
    return MY_ISO_LINES_ID;
  }

  /// Reference to the transparency of the result.
  /// The double value is used. The value is in [0, 1] range
  inline static const std::string& SHOW_ISO_LINES_ID()
  {
    static const std::string MY_SHOW_ISO_LINES_ID("Show_Iso_lines");
    return MY_SHOW_ISO_LINES_ID;
  }

  /// Returns true if the result is concealed from the data tree (referenced by other objects)
  MODELAPI_EXPORT virtual bool isConcealed();

  /// Sets the result as concealed in the data tree (referenced by other objects)
  MODELAPI_EXPORT virtual void setIsConcealed(const bool theValue, const bool theForced = false);

  /// Enables/disables the result. The disabled result does not participate in any calculation
  /// and visualization: like it was removed. But it keeps the general parameters: colors,
  /// visibility, etc.
  /// \param theThis pointer to this object, needed to generate all events if it is necessary
  /// \param theFlag makes disabled if it is true
  /// \returns true if state is really changed
  MODELAPI_EXPORT virtual bool setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
    const bool theFlag);

  /// Returns the result is disabled or not.
  MODELAPI_EXPORT virtual bool isDisabled();

  /// Request for initialization of data model of the result: adding all attributes
  MODELAPI_EXPORT virtual void initAttributes();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Result();

  /// Returns the shape-result produced by this feature (or null if no shapes)
  MODELAPI_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// On change of attribute of the result update presentation of this result:
  /// for the current moment there are only presentation attributes assigned to results
  MODELAPI_EXPORT virtual void attributeChanged(const std::string& theID);

protected:
  /// This method is called just after creation of the object: it must initialize
  /// all fields, normally initialized in the constructor
  MODELAPI_EXPORT virtual void init();

friend class Model_Objects;
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_Result> ResultPtr;

#endif
