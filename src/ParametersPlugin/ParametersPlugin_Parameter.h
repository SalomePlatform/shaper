// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_Parameter.h
// Created:     23 MArch 2015
// Author:      sbh

#ifndef PARAMETERSPLUGIN_PARAMETER_H_
#define PARAMETERSPLUGIN_PARAMETER_H_

#include "ParametersPlugin.h"
#include <ModelAPI_Feature.h>

class ParametersPlugin_PyInterp;

class ParametersPlugin_Parameter : public ModelAPI_Feature
{
 public:
  virtual ~ParametersPlugin_Parameter();

  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_EXTRUSION_ID("Parameter");
    return MY_EXTRUSION_ID;
  }
  /// attribute name of references sketch entities list, it should contain a sketch result or
  /// a pair a sketch result to sketch face
  inline static const std::string& VARIABLE_ID()
  {
    static const std::string MY_VARIABLE_ID("variable");
    return MY_VARIABLE_ID;
  }

  /// attribute name of extrusion size
  inline static const std::string& EXPRESSION_ID()
  {
    static const std::string MY_EXPRESSION_ID("expression");
    return MY_EXPRESSION_ID;
  }

  /// Returns the kind of a feature
  PARAMETERSPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = ParametersPlugin_Parameter::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  PARAMETERSPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PARAMETERSPLUGIN_EXPORT virtual void initAttributes();

  PARAMETERSPLUGIN_EXPORT virtual bool isInHistory(); //false
  //bool isI

  /// Use plugin manager for features creation
  ParametersPlugin_Parameter();

 protected:
  double evaluate(const std::string& theExpression, std::string& theError);

 private:
  ParametersPlugin_PyInterp* myInterp;
};

#endif
