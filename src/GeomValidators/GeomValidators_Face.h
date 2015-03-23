// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Face.h
// Created:     20 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_Face_H
#define GeomValidators_Face_H

#include "GeomValidators.h"
#include "ModelAPI_AttributeValidator.h"

#include <GeomAbs_SurfaceType.hxx>

/**
* \ingroup Validators
* A validator of selection
*/
class GeomValidators_Face : public ModelAPI_AttributeValidator
{
 public:
   GEOMVALIDATORS_EXPORT GeomValidators_Face() {}
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments) const;
protected:
  /// Convert string to TypeOfFace value
  /// \param theType a string value
  GEOMVALIDATORS_EXPORT static GeomAbs_SurfaceType faceType(const std::string& theType);
};

#endif
