// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_SERVICES_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_SERVICES_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <memory>
#include <string>
//--------------------------------------------------------------------------------------
class GeomAPI_Ax3;
class GeomAPI_Dir;
class GeomAPI_Pnt;
class ModelAPI_Document;
class ModelAPI_Result;
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
std::shared_ptr<GeomAPI_Ax3> defaultPlane(const std::wstring & theName);

/// Return name of coordinate plane ("XOY", "XOZ" or "YOZ") or empty string for other planes.
MODELHIGHAPI_EXPORT
std::wstring defaultPlane(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                         const std::shared_ptr<GeomAPI_Dir>& theNormal,
                         const std::shared_ptr<GeomAPI_Dir>& theDirX);

/** Return one of the three standard results defined in PartSet document.
 *
 *  These planes are respectively referred to by name "XOY" (Z=0), "XOZ" (Y=0) or "YOZ" (X=0).
 */
MODELHIGHAPI_EXPORT
std::shared_ptr<ModelAPI_Result> standardPlane(const std::wstring & theName);

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

/** Executes features, created or updated before.
 *
 *  This method flushes creation and update events in order to make all
 *  features up to date.
 */
MODELHIGHAPI_EXPORT
void updateFeatures();

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
