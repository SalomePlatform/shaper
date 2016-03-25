// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_BaseForGeneration.h
// Created:     18 March 2016
// Author:      Dmitry Bobylev

#ifndef GeomValidators_BaseForGeneration_H
#define GeomValidators_BaseForGeneration_H

#include "GeomValidators.h"

#include <GeomAPI_Shape.h>

#include <ModelAPI_AttributeValidator.h>

/// \ingroup Validators
/// A validator for selection base for generation. Allows to select faces on sketch,
/// whole sketch(if it has at least one face), and following objects: vertex, edge, wire, face.
class GeomValidators_BaseForGeneration : public ModelAPI_AttributeValidator
{
public:
  //! Returns true if attribute has selection type listed in the parameter arguments.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const;

private:
  bool isValidAttribute(const AttributePtr& theAttribute,
                        std::string& theError) const;
};

#endif
