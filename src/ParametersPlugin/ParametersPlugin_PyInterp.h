/*
 * ParametersPlugin_PyInterp.h
 *
 *  Created on: Apr 2, 2015
 *      Author: sbh
 */

#ifndef PARAMETERSPLUGIN_PYINTERP_H_
#define PARAMETERSPLUGIN_PYINTERP_H_

#include <PyInterp_Interp.h>
#include <ParametersPlugin.h>

#include <list>
#include <string>
#include <utility>

/**
 * \class ParametersPlugin_PyInterp
 * \ingroup Plugins
 * \brief Helper class for using Python interpreter.
 */
class PARAMETERSPLUGIN_EXPORT ParametersPlugin_PyInterp : public PyInterp_Interp
{
 public:
  ParametersPlugin_PyInterp();
  virtual ~ParametersPlugin_PyInterp();

  /// Returns a list of positions for theName in theExpression.
  std::list<std::pair<int, int> > positions(const std::string& theExpression,
                                            const std::string& theName);
  /// Compiles theExpression and returns a list of parameters from theExpression.
  std::list<std::string> compile(const std::string& theExpression);
  /// Extends local context with the list of parameters with values.
  void extendLocalContext(const std::list<std::string>& theParameters);
  /// Clears local context.
  void clearLocalContext();
  /// Evaluates theExpression and returns its value.
  double evaluate(const std::string& theExpression, std::string& theError);

 protected:
  /// Returns error message.
  std::string errorMessage();
  /// Overrides PyInterp_Interp.
  virtual bool initContext();
  /// Reimplemented from PyInterp_Interp::closeContext().
  virtual void closeContext();
};

#endif /* PARAMETERSPLUGIN_PYINTERP_H_ */
