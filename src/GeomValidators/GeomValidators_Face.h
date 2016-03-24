// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Face.h
// Created:     20 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_Face_H
#define GeomValidators_Face_H

#include "GeomValidators.h"
#include "ModelAPI_AttributeValidator.h"

/**
* \ingroup Validators
* A validator of selection
*/
class GeomValidators_Face : public ModelAPI_AttributeValidator
{
 public:
   GEOMVALIDATORS_EXPORT GeomValidators_Face() {}
  //! returns true if attribute is valid
  //! \param[in] theAttribute the checked attribute
  //! \param[in] theArguments arguments of the attribute
  //! \param[out] theError error message.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const;
};

#endif
