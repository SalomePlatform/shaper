// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SRC_PARTSETAPI_PARTSETAPI_PART_H_
#define SRC_PARTSETAPI_PARTSETAPI_PART_H_

//--------------------------------------------------------------------------------------
#include "PartSetAPI.h"

#include <PartSetPlugin_Part.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_Document;
//--------------------------------------------------------------------------------------
/**\class PartSetAPI_Part
 * \ingroup CPPHighAPI
 * \brief Interface for Part feature
 */
class PartSetAPI_Part : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  PARTSETAPI_EXPORT
  explicit PartSetAPI_Part(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Destructor
  PARTSETAPI_EXPORT
  virtual ~PartSetAPI_Part();

  INTERFACE_0(PartSetPlugin_Part::ID())

  /// Return document
  PARTSETAPI_EXPORT
  std::shared_ptr<ModelAPI_Document> document() const;

  /// Dump wrapped feature
  PARTSETAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Part object
typedef std::shared_ptr<PartSetAPI_Part> PartPtr;

/**\ingroup CPPHighAPI
 * \brief Create Part feature
 */
PARTSETAPI_EXPORT
PartPtr addPart(const std::shared_ptr<ModelAPI_Document> & thePartSet);

/**\ingroup CPPHighAPI
 * \brief Duplicate Part feature
 */
PARTSETAPI_EXPORT
PartPtr duplicatePart(const ModelHighAPI_Interface& thePart);

/**\ingroup CPPHighAPI
 * \brief Remove Part feature
 */
PARTSETAPI_EXPORT
void removePart(const ModelHighAPI_Interface& thePart);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_PARTSETAPI_PARTSETAPI_PART_H_ */
