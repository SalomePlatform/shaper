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

#ifndef GDMLAPI_Ellipsoid_H_
#define GDMLAPI_Ellipsoid_H_

#include "GDMLAPI.h"

#include <GDMLPlugin_Ellipsoid.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;

/// \class GDMLAPI_Ellipsoid
/// \ingroup CPPHighAPI
/// \brief Interface for primitive Ellipsoid feature.
class GDMLAPI_Ellipsoid: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  GDMLAPI_EXPORT
  explicit GDMLAPI_Ellipsoid(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  GDMLAPI_EXPORT
  explicit GDMLAPI_Ellipsoid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_Double& theAX,
                             const ModelHighAPI_Double& theBY,
                             const ModelHighAPI_Double& theCZ);

  /// Constructor with values.
  GDMLAPI_EXPORT
  explicit GDMLAPI_Ellipsoid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_Double& theAX,
                             const ModelHighAPI_Double& theBY,
                             const ModelHighAPI_Double& theCZ,
                             const ModelHighAPI_Double& theZCut1,
                             const ModelHighAPI_Double& theZCut2);

  /// Destructor.
  GDMLAPI_EXPORT
  virtual ~GDMLAPI_Ellipsoid();

  INTERFACE_7(GDMLPlugin_Ellipsoid::ID(),
              ax, GDMLPlugin_Ellipsoid::AX_ID(),
                  ModelAPI_AttributeDouble,
                  /** Ellipsoid size along X axis */,
              by, GDMLPlugin_Ellipsoid::BY_ID(),
                  ModelAPI_AttributeDouble,
                  /** Ellipsoid size along Y axis */,
              cz, GDMLPlugin_Ellipsoid::CZ_ID(),
                  ModelAPI_AttributeDouble,
                  /** Ellipsoid size along Z axis */,
              useZCut1, GDMLPlugin_Ellipsoid::USE_ZCUT1_ID(),
                  ModelAPI_AttributeString,
                  /** First cut of the ellipsoid along Z axis */,
              zCut1, GDMLPlugin_Ellipsoid::ZCUT1_ID(),
                  ModelAPI_AttributeDouble,
                  /** Position of the first cut */,
              useZCut2, GDMLPlugin_Ellipsoid::USE_ZCUT2_ID(),
                  ModelAPI_AttributeString,
                  /** Second cut of the ellipsoid along Z axis */,
              zCut2, GDMLPlugin_Ellipsoid::ZCUT2_ID(),
                  ModelAPI_AttributeDouble,
                  /** Position of the second cut */)

  /// Set dimensions
  GDMLAPI_EXPORT
  void setSizes(const ModelHighAPI_Double& theAX,
                const ModelHighAPI_Double& theBY,
                const ModelHighAPI_Double& theCZ);

  /// Set Z cut no.1
  GDMLAPI_EXPORT
  void setZCut1(const ModelHighAPI_Double& theZCut1);

  /// Set Z cut no.2
  GDMLAPI_EXPORT
  void setZCut2(const ModelHighAPI_Double& theZCut2);

  /// Dump wrapped feature
  GDMLAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on primitive Ellipsoid object
typedef std::shared_ptr<GDMLAPI_Ellipsoid> EllipsoidPtr;

/// \ingroup CPPHighAPI
/// \brief Create GDML Ellipsoid feature.
GDMLAPI_EXPORT
EllipsoidPtr addEllipsoid(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Double& theAX,
                          const ModelHighAPI_Double& theBY,
                          const ModelHighAPI_Double& theCZ);

/// \ingroup CPPHighAPI
/// \brief Create GDML Ellipsoid feature.
GDMLAPI_EXPORT
EllipsoidPtr addEllipsoid(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Double& theAX,
                          const ModelHighAPI_Double& theBY,
                          const ModelHighAPI_Double& theCZ,
                          const ModelHighAPI_Double& theZCut1,
                          const ModelHighAPI_Double& theZCut2);
#endif // GDMLAPI_Ellipsoid_H_
