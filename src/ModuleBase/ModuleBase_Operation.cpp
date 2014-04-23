/*
 * ModuleBase_Operation.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#include "ModuleBase_Operation.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>

#ifdef _DEBUG
#include <QDebug>
#endif

/*!
 \brief Constructor
 \param XGUI_Workshop - workshop for this operation

 Constructs an empty operation. Constructor should work very fast because many
 operators may be created after starting workshop but only several from them
 may be used. As result this constructor stores given workshop in myApp field
 and set Waiting status.
 */
ModuleBase_Operation::ModuleBase_Operation(const QString& theId, QObject* parent)
    : QObject(parent),
      myFlags(Transaction),
      myState(Waiting),
      myExecStatus(Rejected),
      myOperationId(theId)
{
}

/*!
 * \brief Destructor
 */
ModuleBase_Operation::~ModuleBase_Operation()
{

}

/*!
 * \brief Unique name of the operation
 *
 *  Returns string name of the operation.
 */
QString ModuleBase_Operation::operationId() const
{
  return myOperationId;
}

/*!
 * \brief Replied whether the operation should be commited after the start, or the operation itself
 *  do that. The default realization provides the check by the operation having the xml prepresentation 
 *  @return the boolean value
 */
bool ModuleBase_Operation::isPerformedImmediately() const
{
  return xmlRepresentation().isEmpty();
}

std::shared_ptr<ModelAPI_Feature> ModuleBase_Operation::feature() const
{
  return myFeature;
}

/*!
 * \brief Gets state of operation
 * \return Value from OperationState enumeration
 *
 * Gets state of operation (see OperationState enumeration)
 */
ModuleBase_Operation::OperationState ModuleBase_Operation::state() const
{
  return myState;
}

/*!
 * \brief Verifies whether operation is an ran one (state()==Running)
 * \return TRUE if operation is active, FALSE otherwise
 *
 * Verifies whether operation is an running. Returns TRUE if state of operator
 * is Running
 */
bool ModuleBase_Operation::isRunning() const
{
  return state() == Running;
}

/*!
 * \brief Verifies whether given operator is valid for this one
 * \param theOtherOp - other operation
 * \return Returns TRUE if the given operator is valid for this one
 *
 * Verifies whether given operator is valid for this one (i.e. can be started "above"
 * this operator)
 */
bool ModuleBase_Operation::isValid(ModuleBase_Operation*) const
{
  return false;
}

/*!
 * \brief Verifies whether this operator can be always started above any already running one
 * \return Returns TRUE if current operation must not be checked for ActiveOperation->IsValid( this )
 *
 * This method must be redefined in derived operation if operation of derived class
 * must be always can start above any launched one. Default implementation returns FALSE,
 * so it is being checked for IsValid, but some operations may overload IsGranted()
 * In this case they will always start, no matter what operation is running.
 */
bool ModuleBase_Operation::isGranted() const
{
  return false;
}

/*
 * Returns pointer to the root document.
 */
std::shared_ptr<ModelAPI_Document> ModuleBase_Operation::document() const
{
  return ModelAPI_PluginManager::get()->rootDocument();
}

/*!
 * \brief Sets slot which is called when operation is started
 * \param theReceiver - object containing slot
 * \param theSlot - slot of theReceiver object
 * \return TR if slot was connected successfully, FALSE otherwise
 *
 * Sets slot which is called when operation is started. There is no point in
 * using this method. It would be better to inherit own operator from base
 * one and redefine startOperation method
 */
bool ModuleBase_Operation::setSlot(const QObject* theReceiver, const char* theSlot)
{
  return connect(this, SIGNAL(callSlot()), theReceiver, theSlot);
}

/*!
 * \brief Sets the flags of operation
 * \param f - flags of operation to be set
 *
 *  Sets flags of operation (see Flags enumeration)
 */
void ModuleBase_Operation::setFlags(const int f)
{
  myFlags = myFlags | f;
}

/*!
 * \brief Clears the flags of operation
 * \param f - flags of operation to be cleared
 *
 *  Clears flags of operation (see Flags enumeration)
 */
void ModuleBase_Operation::clearFlags(const int f)
{
  myFlags = myFlags & ~f;
}

/*!
 * \brief Test the flags of operation
 * \param f - flags of operation to be tested
 *
 *  Returns TRUE if the specified flags set in the operation (see Flags enumeration)
 */
bool ModuleBase_Operation::testFlags(const int f) const
{
  return (myFlags & f) == f;
}

/*!
 * \brief Gets execution status
 * \return Execution status
 *
 * Gets execution status
 */
int ModuleBase_Operation::execStatus() const
{
  return myExecStatus;
}

/*!
 *  \brief Returns XML representation of the operation's widget.
 *  \return XML QString
 *
 *  Returns XML representation of the operation's widget.
 */
const QString& ModuleBase_Operation::xmlRepresentation() const
{
  return myXmlRepr;
}

/*!
 *  \brief Sets XML representation of the operation's widget.
 *  \param xmlRepr - XML QString
 *
 *  Sets XML representation of the operation's widget.
 */
void ModuleBase_Operation::setXmlRepresentation(const QString& xmlRepr)
{
  myXmlRepr = xmlRepr;
}

/*!
 * \brief Starts operation
 *
 * Public slot. Verifies whether operation can be started and starts operation.
 * This slot is not virtual and cannot be redefined. Redefine startOperation method
 * to change behavior of operation. There is no point in using this method. It would
 * be better to inherit own operator from base one and redefine startOperation method
 * instead.
 */
void ModuleBase_Operation::start()
{
  //document()->start(this);
  document()->startOperation();

  startOperation();
  emit started();
}

/*!
 * \brief Aborts operation
 *
 * Public slot. Aborts operation. This slot is not virtual and cannot be redefined.
 * Redefine abortOperation method to change behavior of operation instead
 */
void ModuleBase_Operation::abort()
{
  abortOperation();
  myState = Waiting;
  emit aborted();

  stopOperation();

  document()->abortOperation();
  emit stopped();
}

/*!
 * \brief Commits operation
 *
 * Public slot. Commits operation. This slot is not virtual and cannot be redefined.
 * Redefine commitOperation method to change behavior of operation instead
 */
void ModuleBase_Operation::commit()
{
  commitOperation();
  myState = Waiting;
  emit committed();

  stopOperation();

  document()->finishOperation();
  emit stopped();
}

/*
 * \brief Alias for start/abort slots
 *
 * Public slot. Aborts operation if false, else does nothing.
 * Provided for S/S compatibility with QAction's toggle(bool)
 */
void ModuleBase_Operation::setRunning(bool on)
{
  if (!on) {
    abort();
  }
}

/*!
 * \brief Stores a real value in model.
 * \param theValue - to store
 *
 * Public slot. Passes theValue into the model.
 */
void ModuleBase_Operation::storeReal(double theValue)
{
  if(!myFeature){
    #ifdef _DEBUG
    qDebug() << "ModuleBase_Operation::storeReal: " <<
        "trying to store value without opening a transaction.";
    #endif
    return;
  }
  QString anId = sender()->objectName();
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeDouble> aReal = aData->real(anId.toStdString());
  aReal->setValue(theValue);
}

/*!
 * \brief Verifies whether operator is ready to start.
 * \return TRUE if operation is ready to start
 *
 * Default implementation returns TRUE. Redefine this method to add own verifications
 */
bool ModuleBase_Operation::isReadyToStart() const
{
  return true;
}

/*!
 * \brief Virtual method called when operation is started
 *
 * Virtual method called when operation started (see start() method for more description)
 * Default implementation calls corresponding slot and commits immediately.
 */
void ModuleBase_Operation::startOperation()
{
  std::shared_ptr<ModelAPI_Document> aDoc = ModelAPI_PluginManager::get()->rootDocument();
  myFeature = aDoc->addFeature(myOperationId.toStdString());
  if (myFeature) // TODO: generate an error if feature was not created
    myFeature->execute();
  //emit callSlot();
  //commit();
}

/*!
 * \brief Virtual method called when operation is started
 *
 * Virtual method called when operation stopped - committed or aborted.
 */
void ModuleBase_Operation::stopOperation()
{
}

/*!
 * \brief Virtual method called when operation aborted
 *
 * Virtual method called when operation aborted (see abort() method for more description)
 */
void ModuleBase_Operation::abortOperation()
{
}

/*!
 * \brief Virtual method called when operation committed
 *
 * Virtual method called when operation committed (see commit() method for more description)
 */
void ModuleBase_Operation::commitOperation()
{
  if (myFeature) myFeature->execute();
}

/*!
 * \brief Sets execution status
 * \param theStatus - execution status
 *
 * Sets myExecStatus to the given value
 */
void ModuleBase_Operation::setExecStatus(const int theVal)
{
  myExecStatus = (ExecStatus) theVal;
}

/*!
 * \brief Sets state of operation
 * \param theState - state of operation to be set
 *
 *  Sets state of operation (see OperationState enumeration)
 */
void ModuleBase_Operation::setState(const ModuleBase_Operation::OperationState theState)
{
  myState = theState;
}
