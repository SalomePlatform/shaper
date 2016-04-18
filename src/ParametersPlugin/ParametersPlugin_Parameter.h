// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_Parameter.h
// Created:     23 MArch 2015
// Author:      sbh

#ifndef PARAMETERSPLUGIN_PARAMETER_H_
#define PARAMETERSPLUGIN_PARAMETER_H_

#include "ParametersPlugin.h"
#include <ModelAPI_Feature.h>

#include <memory>

class ParametersPlugin_PyInterp;

/**
 * \class ParametersPlugin_Parameter
 * \ingroup Plugins
 * \brief Feature for parameter.
 */
class ParametersPlugin_Parameter : public ModelAPI_Feature
{
 public:
  virtual ~ParametersPlugin_Parameter();

  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_PARAMETER_ID("Parameter");
    return MY_PARAMETER_ID;
  }
  /// attribute of parameter name
  inline static const std::string& VARIABLE_ID()
  {
    static const std::string MY_VARIABLE_ID("variable");
    return MY_VARIABLE_ID;
  }
  /// attribute of parameter expression
  inline static const std::string& EXPRESSION_ID()
  {
    static const std::string MY_EXPRESSION_ID("expression");
    return MY_EXPRESSION_ID;
  }
  /// attribute of parameter expression error
  inline static const std::string& EXPRESSION_ERROR_ID()
  {
    static const std::string MY_EXPRESSION_ERROR_ID("ExpressionError");
    return MY_EXPRESSION_ERROR_ID;
  }
  /// list of references to the arguments of this expression
  inline static const std::string& ARGUMENTS_ID()
  {
    static const std::string MY_ARGUMENTS_ID("arguments");
    return MY_ARGUMENTS_ID;
  }

  /// return comment attribute Id
  inline static const std::string& COMMENT_ID()
  {
    static const std::string MY_COMMENT_ID("comment");
    return MY_COMMENT_ID;
  }

  /// Returns the kind of a feature
  PARAMETERSPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = ParametersPlugin_Parameter::ID();
    return MY_KIND;
  }

  /// Pre-execution is not needed for parameter
  PARAMETERSPLUGIN_EXPORT virtual bool isPreviewNeeded() const;

  /// Creates a parameter in document
  PARAMETERSPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PARAMETERSPLUGIN_EXPORT virtual void initAttributes();

  /// Reimplemented from ModelAPI_Feature::isInHistory(). Returns false.
  PARAMETERSPLUGIN_EXPORT virtual bool isInHistory();

  /// Reimplemented from ModelAPI_Feature::isInHistory().
  PARAMETERSPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  ParametersPlugin_Parameter();

 protected:
  /// Evaluates theExpression and returns its value.
  double evaluate(const std::string& theExpression, std::string& theError);
  /// Updates name of the parameter
  void updateName();
  /// Updates expression of the parameter
  bool updateExpression();

 private:
  std::shared_ptr<ParametersPlugin_PyInterp> myInterp;
};

#endif
