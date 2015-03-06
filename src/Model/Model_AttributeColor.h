// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeColor.h
// Created:     6 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef MODEL_ATTRIBUTECOLOR_H_
#define MODEL_ATTRIBUTECOLOR_H_

#include <Model.h>
#include <ModelAPI_AttributeColor.h>

#include <TDF_Label.hxx>
#include <TDataStd_Integer.hxx>

#include <string>

/**\class Model_AttributeColor
 * \ingroup DataModel
 * \brief Attribute that contains three integer values which define the color.
 */

class Model_AttributeColor : public ModelAPI_AttributeColor
{
  Handle_TDataStd_Integer myRed;
  Handle_TDataStd_Integer myGreen;
  Handle_TDataStd_Integer myBlue;
 public:
  /// Defines the color value
  /// \param theRed the red part of the color
  /// \param theRed the green part of the color
  /// \param theRed the blue part of the color
  MODELAPI_EXPORT virtual void setValues(const int theRed,
                                         const int theGreen,
                                         const int theBlue);

  /// Fills the attribute values by a random color
  MODELAPI_EXPORT virtual void setValuesRandom();

  /// Returns the color value
  MODELAPI_EXPORT virtual void values(int& theRed, int& theGreen, int& theBlue);

 protected:
  /// Initializes attibutes
  Model_AttributeColor(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
