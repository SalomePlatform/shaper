// Name   : ModelHighAPI_Services.h
// Purpose:
//
// History:
// 17/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_SERVICES_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_SERVICES_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <memory>
#include <string>
//--------------------------------------------------------------------------------------
class GeomAPI_Ax3;
class ModelAPI_Document;
//--------------------------------------------------------------------------------------
/// Return the main document (the Partset) created or open from the Modeler.
MODELHIGHAPI_EXPORT
std::shared_ptr<ModelAPI_Document> moduleDocument();

/** Return the active document.
 *
 *  This document can be either the main application document (i.e. the Partset) or one of documents
 *  referred to by the main document (a Part).
 */
MODELHIGHAPI_EXPORT
std::shared_ptr<ModelAPI_Document> activeDocument();

/** Return one of the three planes defined by the global coordinate system.
 *
 *  These planes are respectively referred to by name "XOY" (Z=0), "XOZ" (Y=0) or "YOZ" (X=0).
 */
MODELHIGHAPI_EXPORT
std::shared_ptr<GeomAPI_Ax3> defaultPlane(const std::string & theName);

/** Start a data structure transaction.
 *
 *  Make a control point for being able to discard or undo
 *  all operations done during this transaction.
 */
MODELHIGHAPI_EXPORT
void begin();

/** Commit the data structure transaction.
 *
 *  Make all operations done since the last control point undo-able.
 */
MODELHIGHAPI_EXPORT
void end();

/** Commit the data structure transaction and start the new one.
 *
 *  Make all operations done since the last control point undo-able
 *  and continue with the new transaction.
 */
MODELHIGHAPI_EXPORT
void apply();

/// Roll-back the data structure to the previous control point.
MODELHIGHAPI_EXPORT
void undo();

/// Restore the data structure rolled-back by the last undo.
MODELHIGHAPI_EXPORT
void redo();

/// Reset the data structure to initial state.
MODELHIGHAPI_EXPORT
void reset();

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_SERVICES_H_ */
