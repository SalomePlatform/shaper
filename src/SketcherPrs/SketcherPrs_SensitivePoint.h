// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_SensitivePoint.h
// Created:     24 March 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_SensitivePoint_H
#define SketcherPrs_SensitivePoint_H

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
  Standard_EXPORT SketcherPrs_SensitivePoint(const Handle(SelectBasics_EntityOwner)& OwnerId, int theId);
  
  /// Returns number of sub-elements
  Standard_EXPORT virtual Standard_Integer NbSubElements() Standard_OVERRIDE;

  //! Update location of the point
  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Checks whether the point overlaps current selecting volume
  //! \param theMgr selection manager
  //! \param thePickResult returns pick result
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
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

  DEFINE_STANDARD_RTTI(SketcherPrs_SensitivePoint)

private: 
  int myId;
};


#endif