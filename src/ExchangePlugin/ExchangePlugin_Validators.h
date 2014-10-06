// File:        SketchPlugin_Validators.h
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef EXCHANGEPLUGIN_VALIDATORS_H
#define EXCHANGEPLUGIN_VALIDATORS_H

#include "ExchangePlugin.h"
#include <ModelAPI_AttributeValidator.h>

class ExchangePlugin_ImportFormatValidator : public ModelAPI_AttributeValidator
{
  /*
   * Parses input arguments "BREP:BREPImport", "STEP:STEPImport"
   * into list of file formats "BREP","STEP"
   * and list of corresponding plugins: "BREPImport", "STEPImport"
   */
  static bool parseFormats(const std::list<std::string>& theArguments,
                             std::list<std::string>& outFormats);
  static bool parsePlugins(const std::list<std::string>& theArguments,
                           std::list<std::string>& outPlugins);
 public:
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments) const;



};

#endif
