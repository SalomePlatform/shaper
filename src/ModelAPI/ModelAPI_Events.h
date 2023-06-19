// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef MODELAPI_EVENTS_H_
#define MODELAPI_EVENTS_H_

#include <ModelAPI.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <Events_MessageGroup.h>
#include <Events_Loop.h>

#include <memory>
#include <string>
#include <set>
#include <map>


class ModelAPI_Document;
class ModelAPI_ResultParameter;
class GeomAPI_Pnt2d;
class GeomAPI_Shape;

#if defined __GNUC__ || defined __clang__
#define MAYBE_UNUSED __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif

/// Event ID that feature is created (comes with ModelAPI_ObjectUpdatedMessage)
MAYBE_UNUSED static const char * EVENT_OBJECT_CREATED = "ObjectCreated";
/// Event ID that data of feature is updated (comes with Model_ObjectUpdatedMessage)
MAYBE_UNUSED static const char * EVENT_OBJECT_UPDATED = "ObjectUpdated";
/// Event ID that data of feature is deleted (comes with Model_ObjectDeletedMessage)
MAYBE_UNUSED static const char * EVENT_OBJECT_DELETED = "ObjectDeleted";
/// Event ID that name of feature is changed (comes with Model_ObjectRenamedMessage)
MAYBE_UNUSED static const char * EVENT_OBJECT_RENAMED = "ObjectRenamed";
/// Event ID that data of feature is updated (comes with ModelAPI_ObjectUpdatedMessage)
MAYBE_UNUSED static const char * EVENT_OBJECT_MOVED = "ObjectsMoved";
/// Event ID that visualization must be redisplayed (comes with ModelAPI_ObjectUpdatedMessage)
MAYBE_UNUSED static const char * EVENT_OBJECT_TO_REDISPLAY = "ObjectsToRedisplay";
/// Event ID that plugin is loaded (comes with ModelAPI_ObjectUpdatedMessage)
MAYBE_UNUSED static const char * EVENT_PLUGIN_LOADED = "PluginLoaded";
/// The active document becomes another one
MAYBE_UNUSED static const char * EVENT_DOCUMENT_CHANGED = "CurrentDocumentChanged";
/// All documents closed
MAYBE_UNUSED static const char * EVENT_DOCUMENTS_CLOSED = "AllDocumentsClosed";

/// Event ID that order of objects in group is changed,
/// so, tree must be fully recreated (movement of feature)
MAYBE_UNUSED static const char * EVENT_ORDER_UPDATED = "OrderUpdated";
/// Event ID that the sketch is prepared and all grouped messages for the solver may be flushed
MAYBE_UNUSED static const char * EVENT_UPDATE_SELECTION = "UpdateSelection";

/// Request for the enabled/disabled actions behavior for some specific features
MAYBE_UNUSED static const char * EVENT_FEATURE_STATE_REQUEST = "FeatureStateRequest";
/// Reply for the enabled/disabled actions behavior for some specific features
MAYBE_UNUSED static const char * EVENT_FEATURE_STATE_RESPONSE = "FeatureStateResponse";

/// To block the viewer updates
MAYBE_UNUSED static const char * EVENT_UPDATE_VIEWER_BLOCKED = "UpdateViewerBlocked";
/// To unblock the viewer updates
MAYBE_UNUSED static const char * EVENT_UPDATE_VIEWER_UNBLOCKED = "UpdateViewerUnblocked";

/// To inform that there is an empty presentation in the viewer
MAYBE_UNUSED static const char * EVENT_EMPTY_AIS_PRESENTATION = "EmptyAISPresentation";
/// To inform that there is an empty operation for presentation in the viewer
MAYBE_UNUSED static const char * EVENT_EMPTY_OPERATION_PRESENTATION = "EmptyOperationPresentation";
/// To block preview
MAYBE_UNUSED static const char * EVENT_PREVIEW_BLOCKED = "PreviewBlocked";
/// To preview the current feature in the viewer (to compute the result)
MAYBE_UNUSED static const char * EVENT_PREVIEW_REQUESTED = "PreviewRequested";
/// To block automatic recomputation of any feature (by the GUI button press)
MAYBE_UNUSED static const char * EVENT_AUTOMATIC_RECOMPUTATION_DISABLE
                                                         = "DisableAutomaticRecomputation";
/// To unblock block automatic recomputation (default state: the GUI button unpressed)
MAYBE_UNUSED static const char * EVENT_AUTOMATIC_RECOMPUTATION_ENABLE
                                                         = "EnableAutomaticRecomputation";

/// Event ID that solver has conflicting constraints (comes with ModelAPI_SolverFailedMessage)
MAYBE_UNUSED static const char * EVENT_SOLVER_FAILED = "SolverFailed";
/// Event ID that the problem in solver disappeared
MAYBE_UNUSED static const char * EVENT_SOLVER_REPAIRED = "SolverRepaired";

/// Event Id that sketch has DoF = 0
MAYBE_UNUSED static const char * EVENT_SKETCH_FULLY_CONSTRAINED = "SketchFullyConstrainted";
/// Event Id that sketch has DoF > 0
MAYBE_UNUSED static const char * EVENT_SKETCH_UNDER_CONSTRAINED = "SketchUnderConstrainted";
/// Event Id that sketch has DoF < 0
MAYBE_UNUSED static const char * EVENT_SKETCH_OVER_CONSTRAINED  = "SketchOverConstrainted";

/// Event ID that informs that some object has changed the stability
MAYBE_UNUSED static const char * EVENT_STABILITY_CHANGED = "StabilityChanged";

/// Event ID that the sketch is prepared and all grouped messages for the solver may be flushed
MAYBE_UNUSED static const char * EVENT_SKETCH_PREPARED = "SketchPrepared";

/// Event ID that provides a request for list of non-fixed objects necessary for DoF = 0
MAYBE_UNUSED static const char * EVENT_GET_DOF_OBJECTS = "GetDoFObjects";

/// Event ID that provides a request for list of non-fixed objects necessary for DoF = 0
MAYBE_UNUSED static const char * EVENT_DOF_OBJECTS = "DoFObjects";

/// Event ID that requests updates visual attributes for presentations
MAYBE_UNUSED static const char * EVENT_VISUAL_ATTRIBUTES = "UpdateVisualAttributes";

/// Event ID that 1D-fillet failed (comes with ModelAPI_ShapesFailedMessage)
MAYBE_UNUSED static const char * EVENT_OPERATION_SHAPES_FAILED = "OperationShapesFailed";

MAYBE_UNUSED static const char * EVENT_CHECK_CONSTRAINTS = "CheckConstrains";

MAYBE_UNUSED static const char * EVENT_REMOVE_CONSTRAINTS = "RemoveConstrains";

/// Event ID that license of specified features is checked and valid
MAYBE_UNUSED static const char * EVENT_FEATURE_LICENSE_VALID = "FeaturesLicenseValid";

/// To send preferences information: create part on init or not
MAYBE_UNUSED static const char * EVENT_CREATE_PART_ON_START = "CreatePartOnStart";


/// Message that feature was changed (used for Object Browser update): moved, updated and deleted
class MODELAPI_EXPORT ModelAPI_ObjectUpdatedMessage : public Events_MessageGroup
{
 protected:
  /// Creates an empty message
  ModelAPI_ObjectUpdatedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  virtual ~ModelAPI_ObjectUpdatedMessage();

 public:
  /// Returns the feature that has been updated
  virtual const std::set<ObjectPtr>& objects() const = 0;

  //! Creates a new empty group (to store it in the loop before flush)
  virtual std::shared_ptr<Events_MessageGroup> newEmpty() = 0;

  //! Allows to join the given message with the current one
  virtual void Join(const std::shared_ptr<Events_MessageGroup>& theJoined) = 0;
};

/// Message that feature was deleted (used for Object Browser update)
class MODELAPI_EXPORT ModelAPI_ObjectDeletedMessage : public Events_MessageGroup
{
protected:
  /// Creates an empty message
  ModelAPI_ObjectDeletedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  virtual ~ModelAPI_ObjectDeletedMessage();

public:
  /// Returns the groups where the objects were deleted
  virtual const std::list<std::pair<std::shared_ptr<ModelAPI_Document>, std::string> >&
    groups() const = 0;

  /// Creates the new empty message of this kind
  virtual std::shared_ptr<Events_MessageGroup> newEmpty() = 0;

  /// Returns the identifier of the kind of a message
  virtual const Events_ID messageId() = 0;

  /// Appends to this message the given one.
  virtual void Join(const std::shared_ptr<Events_MessageGroup>& theJoined) = 0;
};

/// Message that order changed (used for Object Browser update)
class MODELAPI_EXPORT ModelAPI_OrderUpdatedMessage : public Events_Message
{
protected:
  /// Creates a message:
  ModelAPI_OrderUpdatedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  virtual ~ModelAPI_OrderUpdatedMessage();

public:
  /// Returns the document that has been updated
  virtual std::shared_ptr<ModelAPI_Feature> reordered() = 0;

  /// Returns the identifier of the kind of a message
  virtual const Events_ID messageId() = 0;
};

/// Allows to create ModelAPI messages
class MODELAPI_EXPORT ModelAPI_EventCreator
{
public:
  virtual ~ModelAPI_EventCreator() {}

  /// creates created, updated or moved messages and sends to the loop
  virtual void sendUpdated(const ObjectPtr& theObject, const Events_ID& theEvent,
                           const bool isGroupped = true) const = 0;
  /// creates created, updated or moved messages with the objects collection and sends to the loop
  virtual void sendUpdated(const std::list<ObjectPtr>& theObjects, const Events_ID& theEvent,
    const bool isGroupped = true) const = 0;
  /// creates deleted message and sends to the loop
  virtual void sendDeleted(const std::shared_ptr<ModelAPI_Document>& theDoc,
                           const std::string& theGroup) const = 0;
  /// creates reordered message and sends to the loop
  virtual void sendReordered(const std::shared_ptr<ModelAPI_Feature>& theReordered) const = 0;

  /// returns the creator instance
  static const ModelAPI_EventCreator* get();

  /// sets the creator instance
  static void set(const ModelAPI_EventCreator* theCreator);
};

/// Contains the state information about the feature: is it enabled or disabled.
class ModelAPI_FeatureStateMessage : public Events_Message
{
public:
  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_FeatureStateMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_FeatureStateMessage();

  /// Returns the feature this message is related to
  MODELAPI_EXPORT std::shared_ptr<ModelAPI_Feature> feature() const;
  /// Stores the feature this message is related to
  MODELAPI_EXPORT void setFeature(std::shared_ptr<ModelAPI_Feature>& theFeature);
  // For response
  /// Returns true if feature has specific state
  MODELAPI_EXPORT bool hasState(const std::string& theFeatureId) const;
  /// Returns true if feature is enabled
  MODELAPI_EXPORT bool state(const  std::string& theFeatureId, bool theDefault = false) const;
  /// Stores the feature state
  MODELAPI_EXPORT void setState(const std::string& theFeatureId, bool theValue);
  /// Returns all feature IDs with states
  MODELAPI_EXPORT std::list<std::string> features() const;

 private:
  /// For Request
  std::shared_ptr<ModelAPI_Feature> myCurrentFeature;
  /// For response
  std::map<std::string, bool> myFeatureState;
};

/// Message that document (Part, PartSet) was created
class ModelAPI_DocumentCreatedMessage : public Events_Message
{
  DocumentPtr myDocument;

 public:
  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_DocumentCreatedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_DocumentCreatedMessage();
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID eventId()
  {
    static const char * MY_DOCUMENT_CREATED_EVENT_ID("DocumentCreated");
    return Events_Loop::eventByName(MY_DOCUMENT_CREATED_EVENT_ID);
  }

  /// Returns a document stored in the message
  MODELAPI_EXPORT DocumentPtr document() const;
  /// Sets a document to the message
  MODELAPI_EXPORT void setDocument(DocumentPtr theDocument);
};

/// Message that attribute text should be evaluated in the attribute value
class ModelAPI_AttributeEvalMessage : public Events_Message
{
  AttributePtr myAttribute;

 public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char * MY_ATTRIBUTE_EVALUATION_EVENT_ID("AttributeEvaluationRequest");
    static Events_ID anId = Events_Loop::eventByName(MY_ATTRIBUTE_EVALUATION_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the AttributeEvalMessage event
  MODELAPI_EXPORT static void send(AttributePtr theAttribute, const void* theSender)
  {
    std::shared_ptr<ModelAPI_AttributeEvalMessage> aMessage =
      std::shared_ptr<ModelAPI_AttributeEvalMessage>(
      new ModelAPI_AttributeEvalMessage(eventId(), theSender));
    aMessage->setAttribute(theAttribute);
    Events_Loop::loop()->send(aMessage);
  }

  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_AttributeEvalMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeEvalMessage();

  /// Returns a document stored in the message
  MODELAPI_EXPORT AttributePtr attribute() const;
  /// Sets an attribute to the message
  MODELAPI_EXPORT void setAttribute(AttributePtr theAttribute);
};

/// Message that parameter feature expression should be evaluated: value and error producing
class ModelAPI_ParameterEvalMessage : public Events_Message
{
  FeaturePtr myParam; ///< parameters that should be evaluated
  bool myIsProcessed; ///< true if results were set
  /// result of processing, list of parameters in expression found
  std::list<std::shared_ptr<ModelAPI_ResultParameter> > myParamsList;
  double myResult; ///< result of processing, the computed value of the expression
  std::string myError; ///< error of processing, empty if there is no error

 public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char * MY_PARAMETER_EVALUATION_EVENT_ID("ParameterEvaluationRequest");
    static Events_ID anId = Events_Loop::eventByName(MY_PARAMETER_EVALUATION_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the event.
  /// Returns the message, processed, with the resulting fields filled.
  MODELAPI_EXPORT static std::shared_ptr<ModelAPI_ParameterEvalMessage>
    send(FeaturePtr theParameter, const void* theSender)
  {
    std::shared_ptr<ModelAPI_ParameterEvalMessage> aMessage =
      std::shared_ptr<ModelAPI_ParameterEvalMessage>(
      new ModelAPI_ParameterEvalMessage(eventId(), theSender));
    aMessage->setParameter(theParameter);
    Events_Loop::loop()->send(aMessage);
    return aMessage;
  }

  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_ParameterEvalMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_ParameterEvalMessage();

  /// Returns a parameter stored in the message
  MODELAPI_EXPORT FeaturePtr parameter() const;
  /// Sets a parameter to the message
  MODELAPI_EXPORT void setParameter(FeaturePtr theParam);
  /// Sets the results of processing
  MODELAPI_EXPORT void setResults(
    const std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
    const double theResult, const std::string& theError);
  /// Returns true if the expression is processed
  MODELAPI_EXPORT bool isProcessed();
  /// Returns the results of processing: list of parameters found in the expression
  MODELAPI_EXPORT const std::list<std::shared_ptr<ModelAPI_ResultParameter> >& params() const;
  /// Returns the expression result
  MODELAPI_EXPORT const double& result() const;
  /// Returns the interpreter error (empty if no error)
  MODELAPI_EXPORT const std::string& error() const;
};

class ModelAPI_ImportParametersMessage : public Events_Message
{
  std::string myFilename; ///< filename where where parameters are stored
  std::string myError; ///< error of processing, empty if there is no error

public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char* MY_PARAMETER_EVALUATION_EVENT_ID("ImportParametersMessage");
    static Events_ID anId = Events_Loop::eventByName(MY_PARAMETER_EVALUATION_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the event.
  /// Returns the message, processed, with the resulting fields filled.
  MODELAPI_EXPORT static std::shared_ptr<ModelAPI_ImportParametersMessage>
    send(std::string theParameter, const void* theSender)
  {
    std::shared_ptr<ModelAPI_ImportParametersMessage> aMessage =
      std::shared_ptr<ModelAPI_ImportParametersMessage>(
        new ModelAPI_ImportParametersMessage(eventId(), theSender));
    aMessage->setFilename(theParameter);
    Events_Loop::loop()->send(aMessage);
    return aMessage;
  }

  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_ImportParametersMessage(const Events_ID theID,
                                                   const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_ImportParametersMessage();

  /// Returns a filename stored in the message
  MODELAPI_EXPORT std::string filename() const;
  /// Sets a filename to the message
  MODELAPI_EXPORT void setFilename(std::string theFilename);
};

class ModelAPI_BuildEvalMessage : public Events_Message
{
  FeaturePtr myParam; ///< parameters that should be evaluated
  bool myIsProcessed; ///< true if results were set
  std::string myError; ///< error of processing, empty if there is no error
  /// result of processing, list of parameters in expression found
  std::list<std::shared_ptr<ModelAPI_ResultParameter> > myParamsList;

 public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char * MY_BUILD_EVALUATION_EVENT_ID("BuildEvaluationRequest");
    static Events_ID anId = Events_Loop::eventByName(MY_BUILD_EVALUATION_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the event.
  /// Returns the message, processed, with the resulting fields filled.
  MODELAPI_EXPORT static std::shared_ptr<ModelAPI_BuildEvalMessage>
    send(FeaturePtr theParameter, const void* theSender)
  {
    std::shared_ptr<ModelAPI_BuildEvalMessage> aMessage =
          std::shared_ptr<ModelAPI_BuildEvalMessage>(
                new ModelAPI_BuildEvalMessage(eventId(), theSender));
    aMessage->setParameter(theParameter);
    Events_Loop::loop()->send(aMessage);
    return aMessage;
  }

  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_BuildEvalMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_BuildEvalMessage();

  /// Returns a parameter stored in the message
  MODELAPI_EXPORT FeaturePtr parameter() const;
  /// Sets a parameter to the message
  MODELAPI_EXPORT void setParameter(FeaturePtr theParam);
  /// Sets the results of processing
  MODELAPI_EXPORT void setResults(
                       const std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
                       const std::string& theError);
  /// Returns the results of processing: list of parameters found in the expression
  MODELAPI_EXPORT const std::list<std::shared_ptr<ModelAPI_ResultParameter> >& params() const;
  /// Returns true if the expression is processed
  MODELAPI_EXPORT bool isProcessed();

  /// Returns the interpreter error (empty if no error)
  MODELAPI_EXPORT const std::string& error() const;
};

/// Message to ask compute the positions of parameters in the expression
class ModelAPI_ComputePositionsMessage : public Events_Message
{
  std::wstring myExpression; ///< the expression string
  std::wstring myParamName; ///< name of the parameter to be searched
  std::list<std::pair<int, int> > myPositions; ///< computation result: start-end position indices

public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char * MY_COMPUTE_POSITIOND_EVENT_ID("ComputePositionsRequest");
    static Events_ID anId = Events_Loop::eventByName(MY_COMPUTE_POSITIOND_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the AttributeEvalMessage event
  /// Returns the message, processed, with the resulting fields filled
  MODELAPI_EXPORT static std::shared_ptr<ModelAPI_ComputePositionsMessage>
    send(const std::wstring& theExpression, const std::wstring& theParameter, const void* theSender)
  {
    std::shared_ptr<ModelAPI_ComputePositionsMessage> aMessage =
      std::shared_ptr<ModelAPI_ComputePositionsMessage>(
      new ModelAPI_ComputePositionsMessage(eventId(), theSender));
    aMessage->set(theExpression, theParameter);
    Events_Loop::loop()->send(aMessage);
    return aMessage;
  }

  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_ComputePositionsMessage(
    const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_ComputePositionsMessage();

  /// Returns an expression stored in the message
  MODELAPI_EXPORT const std::wstring& expression() const;
  /// Returns a parameter name stored in the message
  MODELAPI_EXPORT const std::wstring& parameter() const;
  /// Sets an expression and parameter needed for computation
  MODELAPI_EXPORT void set(const std::wstring& theExpression, const std::wstring& theParameter);
  /// Sets the results of processing
  MODELAPI_EXPORT void setPositions(const std::list<std::pair<int, int> >& thePositions);
  /// Returns the results of processing: position start and end indices
  MODELAPI_EXPORT const std::list<std::pair<int, int> >& positions() const;
};

/// Message that the object is renamed
class ModelAPI_ObjectRenamedMessage : public Events_Message
{
  ObjectPtr myObject;
  std::wstring myOldName;
  std::wstring myNewName;

 public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char * MY_OBJECT_RENAMED_EVENT_ID("ObjectRenamed");
    static Events_ID anId = Events_Loop::eventByName(MY_OBJECT_RENAMED_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the AttributeEvalMessage event
  MODELAPI_EXPORT static void send(ObjectPtr theObject,
                                   const std::wstring& theOldName,
                                   const std::wstring& theNewName,
                                   const void* theSender);

  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_ObjectRenamedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_ObjectRenamedMessage();

  /// Returns an object
  MODELAPI_EXPORT ObjectPtr object() const;
  /// Sets an object
  MODELAPI_EXPORT void setObject(ObjectPtr theObject);
  /// Returns an old name
  MODELAPI_EXPORT std::wstring oldName() const;
  /// Sets an old name
  MODELAPI_EXPORT void setOldName(const std::wstring& theOldName);
  /// Returns a new name
  MODELAPI_EXPORT std::wstring newName() const;
  /// Sets a new name
  MODELAPI_EXPORT void setNewName(const std::wstring& theNewName);
};

/// Message that the parameter should be replaced with its value
class ModelAPI_ReplaceParameterMessage : public Events_Message
{
  ObjectPtr myObject;

 public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char * MY_EVENT_ID("ReplaceParameter");
    static Events_ID anId = Events_Loop::eventByName(MY_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the AttributeEvalMessage event
  MODELAPI_EXPORT static void send(ObjectPtr theObject,
                                   const void* theSender);

  /// Creates an empty message
  MODELAPI_EXPORT
    ModelAPI_ReplaceParameterMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_ReplaceParameterMessage();

  /// Returns an object
  MODELAPI_EXPORT ObjectPtr object() const;
  /// Sets an object
  MODELAPI_EXPORT void setObject(ObjectPtr theObject);
};

/// Message that sends the sketch solver and sketcher GUI processes to show in the property panel
class ModelAPI_SolverFailedMessage : public Events_Message
{
public:
  /// Creates an message
  MODELAPI_EXPORT ModelAPI_SolverFailedMessage(const Events_ID theID, const void* theSender = 0);
  /// Default destructor
  MODELAPI_EXPORT virtual ~ModelAPI_SolverFailedMessage();

  /// Sets list of conflicting constraints
  MODELAPI_EXPORT void setObjects(const std::set<ObjectPtr>& theObjects);
  /// Returns list of conflicting constraints
  MODELAPI_EXPORT const std::set<ObjectPtr>& objects() const;

  /// Sets degrees of freedom
  void dof(const int theDOF) { myDOF = theDOF; }
  /// Returns degrees of freedom
  const int& dof() const { return myDOF; }

private:
  std::set<ObjectPtr> myObjects;
  int myDOF;
};

/// Message sent when feature or attribute has been moved.
/// Stores the moving object/attribute, original and new positions of mouse.
class ModelAPI_ObjectMovedMessage : public Events_Message
{
  ObjectPtr myMovedObject;
  AttributePtr myMovedAttribute;
  int myMovedPointIndex;

  std::shared_ptr<GeomAPI_Pnt2d> myOriginalPosition;
  std::shared_ptr<GeomAPI_Pnt2d> myCurrentPosition;

public:
  MODELAPI_EXPORT ModelAPI_ObjectMovedMessage(const void* theSender = 0);

  /// Set object which is being moved (if the message already contains attribute it will be cleared)
  MODELAPI_EXPORT void setMovedObject(const ObjectPtr& theMovedObject);
  /// Set attribute which is being moved (if the message already contains object it will be cleared)
  /// \param[in] theMovedAttribute moved attribute
  /// \param[in] thePointIndex     index of the point if the moved attribute is an array of points
  MODELAPI_EXPORT void setMovedAttribute(const AttributePtr& theMovedAttribute,
                                         const int thePointIndex = -1);

  /// Return moved object
  ObjectPtr movedObject() const
  { return myMovedObject; }
  /// Return moved attribute
  AttributePtr movedAttribute() const
  { return myMovedAttribute; }
  /// Return index of the moved point
  int movedPointIndex() const
  { return myMovedPointIndex; }

  /// Set original mouse position
  MODELAPI_EXPORT void setOriginalPosition(double theX, double theY);
  /// Set original mouse position
  MODELAPI_EXPORT void setOriginalPosition(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);
  /// Return original mouse position
  const std::shared_ptr<GeomAPI_Pnt2d>& originalPosition() const
  { return myOriginalPosition; }

  /// Set current mouse position
  MODELAPI_EXPORT void setCurrentPosition(double theX, double theY);
  /// Set current mouse position
  MODELAPI_EXPORT void setCurrentPosition(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);
  /// Return current mouse position
  const std::shared_ptr<GeomAPI_Pnt2d>& currentPosition() const
  { return myCurrentPosition; }
};

/// Message that sends the failed vertices of 1D-fillet to highlight them in 3D viewer
class ModelAPI_ShapesFailedMessage : public Events_Message
{
public:
  /// Creates an message
  MODELAPI_EXPORT ModelAPI_ShapesFailedMessage(const Events_ID theID, const void* theSender = 0);
  /// Default destructor
  MODELAPI_EXPORT virtual ~ModelAPI_ShapesFailedMessage();
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID eventId()
  {
    return Events_Loop::eventByName(EVENT_OPERATION_SHAPES_FAILED);
  }

  /// Sets list of failed vertices
  MODELAPI_EXPORT void setShapes(const std::list< std::shared_ptr<GeomAPI_Shape> >& theVertices);
  /// Returns list of failed vertices
  MODELAPI_EXPORT const std::list< std::shared_ptr<GeomAPI_Shape> >& shapes() const;

private:
  std::list< std::shared_ptr<GeomAPI_Shape> > myShapes;
};

///Message that sends the constraints to check or remove
class ModelAPI_CheckConstraintsMessage : public Events_Message
{
public:
  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_CheckConstraintsMessage(
    const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_CheckConstraintsMessage();

  ///Get list of constrains
  MODELAPI_EXPORT const std::set<ObjectPtr>& constraints() const;

  ///Set list of constrains
  MODELAPI_EXPORT void setConstraints(const std::set<ObjectPtr>& theConstraints);

private:
  std::set<ObjectPtr> myConstraints;
};


/// Message that sends the features which license is checked and valid
class ModelAPI_FeaturesLicenseValidMessage : public Events_Message
{
public:
  /// Creates an message
  MODELAPI_EXPORT
  ModelAPI_FeaturesLicenseValidMessage(const Events_ID theID, const void* theSender = 0);
  /// Default destructor
  MODELAPI_EXPORT virtual ~ModelAPI_FeaturesLicenseValidMessage();
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID eventId()
  {
    return Events_Loop::eventByName(EVENT_FEATURE_LICENSE_VALID);
  }

  /// Sets list of features with valid license
  MODELAPI_EXPORT void setFeatures(const std::set<std::string>& theFeatures);
  /// Returns list of features with valid license
  MODELAPI_EXPORT const std::set<std::string>& features() const;

private:
  std::set<std::string> myFeatures;
};

#endif
