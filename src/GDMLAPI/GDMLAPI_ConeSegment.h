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

#ifndef GDMLAPI_ConeSegment_H_
#define GDMLAPI_ConeSegment_H_

#include "GDMLAPI.h"

#include <GDMLPlugin_ConeSegment.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;

/// \class GDMLPlugin_ConeSegment
/// \ingroup CPPHighAPI
/// \brief Interface for primitive ConeSegment feature.
class GDMLAPI_ConeSegment: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  GDMLAPI_EXPORT
  explicit GDMLAPI_ConeSegment(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  GDMLAPI_EXPORT
  explicit GDMLAPI_ConeSegment(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                               const ModelHighAPI_Double& theRMin1,
                               const ModelHighAPI_Double& theRMax1,
                               const ModelHighAPI_Double& theRMin2,
                               const ModelHighAPI_Double& theRMax2,
                               const ModelHighAPI_Double& theZ,
                               const ModelHighAPI_Double& theStartPhi,
                               const ModelHighAPI_Double& theDeltaPhi);

  /// Destructor.
  GDMLAPI_EXPORT
  virtual ~GDMLAPI_ConeSegment();

  INTERFACE_7(GDMLPlugin_ConeSegment::ID(),
              rmin1, GDMLPlugin_ConeSegment::RMIN1_ID(),
              ModelAPI_AttributeDouble, /** Inner radius at base of cone */,
              rmax1, GDMLPlugin_ConeSegment::RMAX1_ID(),
              ModelAPI_AttributeDouble, /** Outer radius at base of cone */,
              rmin2, GDMLPlugin_ConeSegment::RMIN2_ID(),
              ModelAPI_AttributeDouble, /** Inner radius at top of cone */,
              rmax2, GDMLPlugin_ConeSegment::RMAX2_ID(),
              ModelAPI_AttributeDouble, /** Outer radius at top of cone */,
              z, GDMLPlugin_ConeSegment::Z_ID(),
              ModelAPI_AttributeDouble, /** Height of the cone segment */,
              startphi, GDMLPlugin_ConeSegment::STARTPHI_ID(),
              ModelAPI_AttributeDouble, /** Start angle of the segment */,
              deltaphi, GDMLPlugin_ConeSegment::DELTAPHI_ID(),
              ModelAPI_AttributeDouble, /** Angle of the segment */)

  /// Set dimensions
  GDMLAPI_EXPORT
  void setAttributes(const ModelHighAPI_Double& theRMin1,
                     const ModelHighAPI_Double& theRMax1,
                     const ModelHighAPI_Double& theRMin2,
                     const ModelHighAPI_Double& theRMax2,
                     const ModelHighAPI_Double& theZ,
                     const ModelHighAPI_Double& theStartPhi,
                     const ModelHighAPI_Double& theDeltaPhi);

  /// Dump wrapped feature
  GDMLAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on primitive ConeSegment object
typedef std::shared_ptr<GDMLAPI_ConeSegment> ConeSegmentPtr;

/// \ingroup CPPHighAPI
/// \brief Create GDML ConeSegment feature.
GDMLAPI_EXPORT
ConeSegmentPtr addConeSegment(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Double& theRMin1,
                              const ModelHighAPI_Double& theRMax1,
                              const ModelHighAPI_Double& theRMin2,
                              const ModelHighAPI_Double& theRMax2,
                              const ModelHighAPI_Double& theZ,
                              const ModelHighAPI_Double& theStartPhi,
                              const ModelHighAPI_Double& theDeltaPhi);

#endif // GDMLAPI_ConeSegment_H_
