// Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

#ifndef PRIMITIVESAPI_TUBE_H_
#define PRIMITIVESAPI_TUBE_H_

#include "PrimitivesAPI.h"

#include <PrimitivesPlugin_Tube.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>


/// \class PrimitivesAPI_Tube
/// \ingroup CPPHighAPI
/// \brief Interface for primitive Tube feature.
class PrimitivesAPI_Tube: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Tube(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Tube(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const ModelHighAPI_Double& theRMin,
                              const ModelHighAPI_Double& theRMax,
                              const ModelHighAPI_Double& theHeight);

  /// Destructor.
  PRIMITIVESAPI_EXPORT
  virtual ~PrimitivesAPI_Tube();

  INTERFACE_3(PrimitivesPlugin_Tube::ID(),
              rmin, PrimitivesPlugin_Tube::RMIN_ID(),
              ModelAPI_AttributeDouble, /** Inner radius */,
              rmax, PrimitivesPlugin_Tube::RMAX_ID(),
              ModelAPI_AttributeDouble, /** Outer radius */,
              height, PrimitivesPlugin_Tube::HEIGHT_ID(),
              ModelAPI_AttributeDouble, /** Height */)

  /// Set radius
  PRIMITIVESAPI_EXPORT
  void setRadius(const ModelHighAPI_Double& theRMin,
                 const ModelHighAPI_Double& theRMax);

  /// Set height
  PRIMITIVESAPI_EXPORT
  void setHeight(const ModelHighAPI_Double& theHeight);

  /// Dump wrapped feature
  PRIMITIVESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on primitive Tube object
typedef std::shared_ptr<PrimitivesAPI_Tube> TubePtr;

/// \ingroup CPPHighAPI
/// \brief Create primitive Tube feature.
PRIMITIVESAPI_EXPORT
TubePtr addTube(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Double& theRMin,
                const ModelHighAPI_Double& theRMax,
                const ModelHighAPI_Double& theHeight);

#endif // PRIMITIVESAPI_TUBE_H_
