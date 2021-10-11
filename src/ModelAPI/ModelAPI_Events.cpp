// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <ModelAPI.h>
#include <ModelAPI_Events.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Shape.h>

//#define DEBUG_OBJECT_MOVED_MESSAGE
#ifdef DEBUG_OBJECT_MOVED_MESSAGE
#include <iostream>
#endif

ModelAPI_ObjectUpdatedMessage::ModelAPI_ObjectUpdatedMessage(const Events_ID theID,
                                                             const void* theSender)
    : Events_MessageGroup(theID, theSender)
{}

ModelAPI_ObjectUpdatedMessage::~ModelAPI_ObjectUpdatedMessage()
{}

ModelAPI_ObjectDeletedMessage::ModelAPI_ObjectDeletedMessage(const Events_ID theID,
                                                             const void* theSender)
    : Events_MessageGroup(theID, theSender)
{}

ModelAPI_ObjectDeletedMessage::~ModelAPI_ObjectDeletedMessage()
{}

ModelAPI_OrderUpdatedMessage::ModelAPI_OrderUpdatedMessage(const Events_ID theID,
                                                           const void* theSender)
    : Events_Message(theID, theSender)
{}

ModelAPI_OrderUpdatedMessage::~ModelAPI_OrderUpdatedMessage()
{}

// used by GUI only
// LCOV_EXCL_START
ModelAPI_FeatureStateMessage::ModelAPI_FeatureStateMessage(const Events_ID theID,
                                                           const void* theSender)
 : Events_Message(theID, theSender)
{
  myCurrentFeature = std::shared_ptr<ModelAPI_Feature>();
}

ModelAPI_FeatureStateMessage::~ModelAPI_FeatureStateMessage()
{}

std::shared_ptr<ModelAPI_Feature> ModelAPI_FeatureStateMessage::feature() const
{
  return myCurrentFeature;
}

void ModelAPI_FeatureStateMessage::setFeature(std::shared_ptr<ModelAPI_Feature>& theFeature)
{
  myCurrentFeature = theFeature;
}

bool ModelAPI_FeatureStateMessage::hasState(const std::string& theKey) const
{
  return myFeatureState.find(theKey) != myFeatureState.end();
}

bool ModelAPI_FeatureStateMessage::state(const  std::string& theFeatureId, bool theDefault) const
{
  if(hasState(theFeatureId)) {
    return myFeatureState.at(theFeatureId);
  }
  return theDefault;
}

void ModelAPI_FeatureStateMessage::setState(const std::string& theFeatureId, bool theValue)
{
  myFeatureState[theFeatureId] = theValue;
}

std::list<std::string> ModelAPI_FeatureStateMessage::features() const
{
  std::list<std::string> result;
  std::map<std::string, bool>::const_iterator it = myFeatureState.begin();
  for( ; it != myFeatureState.end(); ++it) {
    result.push_back(it->first);
  }
  return result;
}
// LCOV_EXCL_STOP

ModelAPI_DocumentCreatedMessage::ModelAPI_DocumentCreatedMessage(
  const Events_ID theID, const void* theSender)
: Events_Message(theID, theSender)
{}

ModelAPI_DocumentCreatedMessage::~ModelAPI_DocumentCreatedMessage()
{}

DocumentPtr ModelAPI_DocumentCreatedMessage::document() const
{
  return myDocument;
}

void ModelAPI_DocumentCreatedMessage::setDocument(DocumentPtr theDocument)
{
  myDocument = theDocument;
}

ModelAPI_AttributeEvalMessage::ModelAPI_AttributeEvalMessage(
  const Events_ID theID, const void* theSender)
: Events_Message(theID, theSender)
{}

ModelAPI_AttributeEvalMessage::~ModelAPI_AttributeEvalMessage()
{}

AttributePtr ModelAPI_AttributeEvalMessage::attribute() const
{
  return myAttribute;
}

void ModelAPI_AttributeEvalMessage::setAttribute(AttributePtr theAttribute)
{
  myAttribute = theAttribute;
}

ModelAPI_ParameterEvalMessage::ModelAPI_ParameterEvalMessage(
  const Events_ID theID, const void* theSender)
  : Events_Message(theID, theSender), myIsProcessed(false)
{}

ModelAPI_ParameterEvalMessage::~ModelAPI_ParameterEvalMessage()
{}

FeaturePtr ModelAPI_ParameterEvalMessage::parameter() const
{
  return myParam;
}

void ModelAPI_ParameterEvalMessage::setParameter(FeaturePtr theParam)
{
  myParam = theParam;
}

void ModelAPI_ParameterEvalMessage::setResults(
    const std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
    const double theResult, const std::string& theError)
{
  myParamsList = theParamsList;
  myResult = theResult;
  myError = theError;
  myIsProcessed = true;
}

bool ModelAPI_ParameterEvalMessage::isProcessed()
{
  return myIsProcessed;
}

const std::list<std::shared_ptr<ModelAPI_ResultParameter> >&
  ModelAPI_ParameterEvalMessage::params() const
{
  return myParamsList;
}

const double& ModelAPI_ParameterEvalMessage::result() const
{
  return myResult;
}

const std::string& ModelAPI_ParameterEvalMessage::error() const
{
  return myError;
}

/// Creates an empty message
ModelAPI_ImportParametersMessage::ModelAPI_ImportParametersMessage(const Events_ID theID, const void* theSender)
  :Events_Message(theID, theSender)
{

}

ModelAPI_ImportParametersMessage::~ModelAPI_ImportParametersMessage()
{
}

std::string ModelAPI_ImportParametersMessage::filename() const
{
  return myFilename;
}

void ModelAPI_ImportParametersMessage::setFilename(std::string theFilename)
{
  myFilename = theFilename;
}

ModelAPI_BuildEvalMessage::ModelAPI_BuildEvalMessage(
  const Events_ID theID, const void* theSender)
  : Events_Message(theID, theSender), myIsProcessed(false)
{}

ModelAPI_BuildEvalMessage::~ModelAPI_BuildEvalMessage()
{}

FeaturePtr ModelAPI_BuildEvalMessage::parameter() const
{
  return myParam;
}

void ModelAPI_BuildEvalMessage::setParameter(FeaturePtr theParam)
{
  myParam = theParam;
}

void ModelAPI_BuildEvalMessage::setResults(
            const std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
            const std::string& theError)
{
  myParamsList = theParamsList;
  myError = theError;
  myIsProcessed = true;
}

const std::list<std::shared_ptr<ModelAPI_ResultParameter> >&
  ModelAPI_BuildEvalMessage::params() const
{
  return myParamsList;
}

bool ModelAPI_BuildEvalMessage::isProcessed()
{
  return myIsProcessed;
}

const std::string& ModelAPI_BuildEvalMessage::error() const
{
  return myError;
}

ModelAPI_ComputePositionsMessage::ModelAPI_ComputePositionsMessage(
  const Events_ID theID, const void* theSender)
  : Events_Message(theID, theSender)
{}

ModelAPI_ComputePositionsMessage::~ModelAPI_ComputePositionsMessage()
{}

const std::wstring& ModelAPI_ComputePositionsMessage::expression() const
{
  return myExpression;
}

const std::wstring& ModelAPI_ComputePositionsMessage::parameter() const
{
  return myParamName;
}

void ModelAPI_ComputePositionsMessage::set(
  const std::wstring& theExpression, const std::wstring& theParameter)
{
  myExpression = theExpression;
  myParamName = theParameter;
}

void ModelAPI_ComputePositionsMessage::setPositions(
  const std::list<std::pair<int, int> >& thePositions)
{
  myPositions = thePositions;
}

const std::list<std::pair<int, int> >& ModelAPI_ComputePositionsMessage::positions() const
{
  return myPositions;
}


ModelAPI_ObjectRenamedMessage::ModelAPI_ObjectRenamedMessage(const Events_ID theID,
                                                             const void* theSender)
: Events_Message(theID, theSender)
{}

ModelAPI_ObjectRenamedMessage::~ModelAPI_ObjectRenamedMessage()
{}

void ModelAPI_ObjectRenamedMessage::send(ObjectPtr theObject,
                                         const std::wstring& theOldName,
                                         const std::wstring& theNewName,
                                         const void* theSender)
{
  std::shared_ptr<ModelAPI_ObjectRenamedMessage> aMessage(
    new ModelAPI_ObjectRenamedMessage(eventId(), theSender));
  aMessage->setObject(theObject);
  aMessage->setOldName(theOldName);
  aMessage->setNewName(theNewName);
  Events_Loop::loop()->send(aMessage);
}

ObjectPtr ModelAPI_ObjectRenamedMessage::object() const
{
  return myObject;
}

void ModelAPI_ObjectRenamedMessage::setObject(ObjectPtr theObject)
{
  myObject = theObject;
}

std::wstring ModelAPI_ObjectRenamedMessage::oldName() const
{
  return myOldName;
}

void ModelAPI_ObjectRenamedMessage::setOldName(const std::wstring& theOldName)
{
  myOldName = theOldName;
}

std::wstring ModelAPI_ObjectRenamedMessage::newName() const
{
  return myNewName;
}

void ModelAPI_ObjectRenamedMessage::setNewName(const std::wstring& theNewName)
{
  myNewName = theNewName;
}

ModelAPI_ReplaceParameterMessage::ModelAPI_ReplaceParameterMessage(const Events_ID theID,
                                                                   const void* theSender)
: Events_Message(theID, theSender)
{}

ModelAPI_ReplaceParameterMessage::~ModelAPI_ReplaceParameterMessage()
{}

void ModelAPI_ReplaceParameterMessage::send(ObjectPtr theObject,
                                            const void* theSender)
{
  std::shared_ptr<ModelAPI_ReplaceParameterMessage> aMessage(
      new ModelAPI_ReplaceParameterMessage(eventId(), theSender));
  aMessage->setObject(theObject);
  Events_Loop::loop()->send(aMessage);
}

ObjectPtr ModelAPI_ReplaceParameterMessage::object() const
{
  return myObject;
}

void ModelAPI_ReplaceParameterMessage::setObject(ObjectPtr theObject)
{
  myObject = theObject;
}


// =====   ModelAPI_SolverFailedMessage   =====
ModelAPI_SolverFailedMessage::ModelAPI_SolverFailedMessage(const Events_ID theID,
                                                           const void* theSender)
  : Events_Message(theID, theSender),
    myDOF(-1)
{}

ModelAPI_SolverFailedMessage::~ModelAPI_SolverFailedMessage()
{}

void ModelAPI_SolverFailedMessage::setObjects(const std::set<ObjectPtr>& theObjects)
{
  myObjects = theObjects;
}

const std::set<ObjectPtr>& ModelAPI_SolverFailedMessage::objects() const
{
  return myObjects;
}


// =====   ModelAPI_ObjectMovedMessage   =====
ModelAPI_ObjectMovedMessage::ModelAPI_ObjectMovedMessage(const void* theSender)
  : Events_Message(Events_Loop::eventByName(EVENT_OBJECT_MOVED), theSender)
{}

void ModelAPI_ObjectMovedMessage::setMovedObject(const ObjectPtr& theMovedObject)
{
  myMovedObject = theMovedObject;
  myMovedAttribute = AttributePtr();
}

void ModelAPI_ObjectMovedMessage::setMovedAttribute(const AttributePtr& theMovedAttribute,
                                                    const int thePointIndex)
{
  myMovedAttribute = theMovedAttribute;
  myMovedObject = ObjectPtr();
  myMovedPointIndex = thePointIndex;
}

void ModelAPI_ObjectMovedMessage::setOriginalPosition(double theX, double theY)
{
  myOriginalPosition = std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(theX, theY));
#ifdef DEBUG_OBJECT_MOVED_MESSAGE
  std::cout << "setOriginalPosition: " << myOriginalPosition->x() << ", "
                                       << myOriginalPosition->y() << std::endl;
#endif
}

void ModelAPI_ObjectMovedMessage::setOriginalPosition(
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  myOriginalPosition = thePoint;
#ifdef DEBUG_OBJECT_MOVED_MESSAGE
  std::cout << "setOriginalPosition: " << myOriginalPosition->x() << ", "
                                       << myOriginalPosition->y() << std::endl;
#endif
}

void ModelAPI_ObjectMovedMessage::setCurrentPosition(double theX, double theY)
{
  myCurrentPosition = std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(theX, theY));
#ifdef DEBUG_OBJECT_MOVED_MESSAGE
  std::cout << "setCurrentPosition: " << myCurrentPosition->x() << ", " << myCurrentPosition->y()
            << ", myCurrentPosition - myOriginalPosition: "
            << myCurrentPosition->x() - myOriginalPosition->x() << ", "
            << myCurrentPosition->y() - myOriginalPosition->y() << std::endl;
#endif
}

void ModelAPI_ObjectMovedMessage::setCurrentPosition(
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  myCurrentPosition = thePoint;
#ifdef DEBUG_OBJECT_MOVED_MESSAGE
  std::cout << "setCurrentPosition: " << myCurrentPosition->x() << ", " << myCurrentPosition->y()
            << ", myCurrentPosition - myOriginalPosition: "
            << myCurrentPosition->x() - myOriginalPosition->x() << ", "
            << myCurrentPosition->y() - myOriginalPosition->y() << std::endl;
#endif
}


// =====   ModelAPI_ShapesFailedMessage   =====
ModelAPI_ShapesFailedMessage::ModelAPI_ShapesFailedMessage(const Events_ID theID,
                                                               const void* theSender)
  : Events_Message(theID, theSender)
{}

ModelAPI_ShapesFailedMessage::~ModelAPI_ShapesFailedMessage()
{}

void ModelAPI_ShapesFailedMessage::setShapes(const ListOfShape& theShapes)
{
  myShapes = theShapes;
}

const ListOfShape& ModelAPI_ShapesFailedMessage::shapes() const
{
  return myShapes;
}
