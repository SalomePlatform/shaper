// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->
/*
 * InitializationPlugin_PyInterp.h
 *
 *  Created on: Apr 2, 2015
 *      Author: sbh
 */

#ifndef INITIALIZATIONPLUGIN_PYINTERP_H_
#define INITIALIZATIONPLUGIN_PYINTERP_H_

#include <PyInterp_Interp.h>
#include <InitializationPlugin.h>

#include <list>
#include <string>
#include <utility>

/**
 * \class InitializationPlugin_PyInterp
 * \ingroup Plugins
 * \brief Helper class for using Python interpreter.
 */
class INITIALIZATIONPLUGIN_EXPORT InitializationPlugin_PyInterp : public PyInterp_Interp
{
 public:
  InitializationPlugin_PyInterp();
  virtual ~InitializationPlugin_PyInterp();

  /// Returns a list of positions for theName in theExpression.
  std::list<std::pair<int, int> > positions(const std::string& theExpression,
                                            const std::string& theName);
  /// Compiles theExpression and returns a list of parameters used in theExpression.
  std::list<std::string> compile(const std::string& theExpression);
  /// Extends local context with the list of parameters.
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

#endif /* INITIALIZATIONPLUGIN_PYINTERP_H_ */
