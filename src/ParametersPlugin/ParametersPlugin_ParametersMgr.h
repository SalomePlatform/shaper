// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_ParametersMgr.h
// Created:     07 April 2016
// Author:      vsv

#ifndef PARAMETERSPLUGIN_PARAMETERSMGR_H_
#define PARAMETERSPLUGIN_PARAMETERSMGR_H_

#include "ParametersPlugin.h"
#include <ModelAPI_Feature.h>

/**
 * \class ParametersPlugin_ParametersMgr
 * \ingroup Plugins
 * \brief A macro feature which manages list of parameters in the current document.
 */

class ParametersPlugin_ParametersMgr : public ModelAPI_Feature
{
public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_PARAMSMGR_ID("ParametersMgr");
    return MY_PARAMSMGR_ID;
  }

  /// Use plugin manager for features creation
  ParametersPlugin_ParametersMgr();

  /// Destructor
  virtual ~ParametersPlugin_ParametersMgr();

  /// Request for initialization of data model of the feature: adding all attributes
  PARAMETERSPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the unique kind of a feature
  PARAMETERSPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = ParametersPlugin_ParametersMgr::ID();
    return MY_KIND;
  };

  /// Computes or recomputes the results
  PARAMETERSPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro(). Returns true.
  PARAMETERSPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  PARAMETERSPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

  /// Returns true if result is persistent (stored in document) and on undo-redo, save-open
  /// it is not needed to recompute it.
  PARAMETERSPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}
};


#endif