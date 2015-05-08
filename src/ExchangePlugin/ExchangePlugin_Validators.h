// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketchPlugin_Validators.h
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef EXCHANGEPLUGIN_VALIDATORS_H
#define EXCHANGEPLUGIN_VALIDATORS_H

#include "ExchangePlugin.h"
#include <ModelAPI_AttributeValidator.h>

/**\class ExchangePlugin_ImportFormatValidator
 * \ingroup Validators
 * \brief Validator for the imported formats checking
 *
 * The configuration file of import/export features contains the information
 * about which formats are supported and the extension of the associated files.
 * This validator filters out files that are out of this description.
 */
class ExchangePlugin_ImportFormatValidator : public ModelAPI_AttributeValidator
{
  /**
   * Parses input arguments "BREP:BREPImport", "STEP:STEPImport"
   * into list of file formats "BREP","STEP"
   * and list of corresponding plugins: "BREPImport", "STEPImport"
   */
  static bool parseFormats(const std::list<std::string>& theArguments,
                           std::list<std::string>& outFormats);
public:
  /**
   * Returns true is the file-name attribute correctly corresponds to the set of
   * allowed formats.
   */
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments) const;



};

#endif
