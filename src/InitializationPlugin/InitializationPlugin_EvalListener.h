// Copyright (C) 2014-2025  CEA, EDF
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

#ifndef SRC_INITIALIZATIONPLUGIN_EVALLISTENER_H_
#define SRC_INITIALIZATIONPLUGIN_EVALLISTENER_H_

#include <InitializationPlugin.h>
#include <Events_Loop.h>
#include <string>
class ModelAPI_Attribute;
class ModelAPI_Document;
class ModelAPI_Feature;
class ModelAPI_ResultParameter;
class InitializationPlugin_Parameter;
class InitializationPlugin_PyInterp;

/**
 * \class InitializationPlugin_EvalListener
 * \ingroup Plugins
 * \brief Class which process the events from the event loop.
 */
class InitializationPlugin_EvalListener : public Events_Listener
{
 public:
  INITIALIZATIONPLUGIN_EXPORT InitializationPlugin_EvalListener();
  INITIALIZATIONPLUGIN_EXPORT virtual ~InitializationPlugin_EvalListener();

  /// Reimplemented from Events_Listener::processEvent().
  INITIALIZATIONPLUGIN_EXPORT
    virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  // performs the python call to initialize high level data model on internal data model creation
  void initDataModel();

 protected:
  /// Evaluates theExpression and returns its value.
  double evaluate(std::shared_ptr<ModelAPI_Feature> theParameter,
                  const std::wstring& theExpression, std::string& theError,
                  std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
                  const bool theIsParameter = false);

  /// Evaluates theExpression with variable local and returns its value.
  double evaluate(std::wstring& theVariable,
                  double theValueVariable,
                  std::shared_ptr<ModelAPI_Feature> theParameter,
                  const std::wstring& theExpression,
                  std::string& theError,
                  std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
                  bool theIsFirstTime);


  /// Processes Evaluation event.
  void processEvaluationEvent(const std::shared_ptr<Events_Message>& theMessage);

 private:
  std::shared_ptr<InitializationPlugin_PyInterp> myInterp;
};

#endif /* SRC_INITIALIZATIONPLUGIN_INITIALIZATIONPLUGIN_EVALLISTENER_H_ */
