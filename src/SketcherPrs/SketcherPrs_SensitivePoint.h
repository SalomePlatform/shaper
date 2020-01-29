// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef SketcherPrs_SensitivePoint_H
#define SketcherPrs_SensitivePoint_H

#include <SelectBasics_EntityOwner.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Standard_DefineHandle.hxx>


DEFINE_STANDARD_HANDLE(SketcherPrs_SensitivePoint, Select3D_SensitiveEntity)

/**
* \ingroup GUI
* This sensitive point is done in order to optimize process of the sinzitive area update
* It referes on point array directly and this lets to avoid procedure of the point synchronisation
*/
class SketcherPrs_SensitivePoint : public Select3D_SensitiveEntity
{
public:
  //! Constructs a sensitive point object defined by the
  //! \param OwnerId an Id of the Owner.
  //! \param theId and Id of its point
  Standard_EXPORT SketcherPrs_SensitivePoint(const Handle(SelectBasics_EntityOwner)& OwnerId,
                                             int theId);

  /// Returns number of sub-elements
  Standard_EXPORT virtual Standard_Integer NbSubElements() Standard_OVERRIDE;

  //! Update location of the point
  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Checks whether the point overlaps current selecting volume
  //! \param theMgr selection manager
  //! \param thePickResult returns pick result
  Standard_EXPORT virtual Standard_Boolean Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                     SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  //! Returns the point used at the time of construction.
  Standard_EXPORT gp_Pnt Point() const;

  //! Returns center of point. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns bounding box of the point. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  /// Clear sub-elements
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(SketcherPrs_SensitivePoint, Select3D_SensitiveEntity)

private:
  int myId;
};


#endif