// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildPlugin_Validators.h
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#ifndef BuildPlugin_Validators_H_
#define BuildPlugin_Validators_H_

#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_FeatureValidator.h>

/// \class BuildPlugin_ValidatorBaseForWire
/// \ingroup Validators
/// \brief A validator for selection base shapes for wire. Allows to select edges on sketch and
/// wires objects that are connected to already selected shapes.
class BuildPlugin_ValidatorBaseForWire: public ModelAPI_AttributeValidator
{
public:
  //! Returns true if attribute is ok.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        std::string& theError) const;
};

#endif
