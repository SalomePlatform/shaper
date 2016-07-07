// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ConstructionPlugin_Validators.h
// Created:     04 July 2016
// Author:      Dmitry Bobylev

#ifndef ConstructionPlugin_Validators_H_
#define ConstructionPlugin_Validators_H_

#include <ModelAPI_AttributeValidator.h>

/// \class ConstructionPlugin_ValidatorPointLines
/// \ingroup Validators
/// \brief A validator for selection lines for point by intersection..
class ConstructionPlugin_ValidatorPointLines: public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

#endif