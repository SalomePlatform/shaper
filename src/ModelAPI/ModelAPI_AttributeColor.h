// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeColor.h
// Created:     6 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModelAPI_AttributeColor_H_
#define ModelAPI_AttributeColor_H_

#include <ModelAPI.h>
#include <ModelAPI_Attribute.h>

#include <string>


/**\class ModelAPI_AttributeColor
 * \ingroup DataModel
 * \brief API for the attribute that contains color (int, int, int). The color is in the range [0, 255]
 * There is an opportunity to fill the attribute by a random color
 */

class ModelAPI_AttributeColor : public ModelAPI_Attribute
{
 public:
  /// Defines the color value
  /// \param theRed the red part of the color
  /// \param theRed the green part of the color
  /// \param theRed the blue part of the color
  MODELAPI_EXPORT virtual void setValues(const int theRed,
                                         const int theGreen,
                                         const int theBlue) = 0;

  /// Fills the attribute values by a random color
  MODELAPI_EXPORT virtual void setValuesRandom() = 0;

  /// Returns the color value
  MODELAPI_EXPORT virtual void values(int& theRed, int& theGreen, int& theBlue) = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string type()
  {
    return "Color";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeColor();

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeColor();
};

//! Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeColor> AttributeColorPtr;

#endif
