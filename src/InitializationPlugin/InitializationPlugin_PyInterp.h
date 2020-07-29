// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
  std::list<std::pair<int, int> > positions(const std::wstring& theExpression,
                                            const std::wstring& theName);
  /// Compiles theExpression and returns a list of parameters used in theExpression.
  std::list<std::wstring> compile(const std::wstring& theExpression);
  /// Extends local context with the list of parameters.
  void extendLocalContext(const std::list<std::wstring>& theParameters);
  /// Clears local context.
  void clearLocalContext();
  /// Evaluates theExpression and returns its value.
  double evaluate(const std::wstring& theExpression, std::string& theError);

  /// Runs the string command in the python interpreter. Returns true if no error is in result.
  bool runString(std::string theString);

 protected:
  /// Returns error message.
  std::string errorMessage();
  /// Overrides PyInterp_Interp.
  virtual bool initContext();
  /// Reimplemented from PyInterp_Interp::closeContext().
  virtual void closeContext();
};

#endif /* INITIALIZATIONPLUGIN_PYINTERP_H_ */
