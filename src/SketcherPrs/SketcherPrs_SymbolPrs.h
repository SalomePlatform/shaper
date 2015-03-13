// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_SymbolPrs.h
// Created:     12 March 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_SymbolPrs_H
#define SketcherPrs_SymbolPrs_H

#include <AIS_InteractiveObject.hxx>
#include <GeomAPI_Ax3.h>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Image_AlienPixMap.hxx>

#include <Standard_DefineHandle.hxx>
#include <map>

class SketchPlugin_Constraint;


DEFINE_STANDARD_HANDLE(SketcherPrs_SymbolPrs, AIS_InteractiveObject)

/**
* \ingroup GUI
* A base class of constraint presentation which is represented by an icon
*/
class SketcherPrs_SymbolPrs: public AIS_InteractiveObject
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_SymbolPrs(SketchPlugin_Constraint* theConstraint, 
                        const std::shared_ptr<GeomAPI_Ax3>& thePlane);
  
  //! Method which clear all selected owners belonging
  //! to this selectable object ( for fast presentation draw )
  Standard_EXPORT virtual void ClearSelected();

  Standard_EXPORT std::shared_ptr<GeomAPI_Ax3> plane() const { return myPlane; }

  Standard_EXPORT SketchPlugin_Constraint* feature() const { return myConstraint; }

  DEFINE_STANDARD_RTTI(SketcherPrs_SymbolPrs)

protected:
  /// Returns an icon file name. Has to be redefined in successors
  virtual const char* iconName() const = 0;

  /// Check and creates if it is necessary myAspect member.
  /// It has to be called before the object computation
  virtual void prepareAspect();

  /// Returns icon corresponded to the current constraint type
  Handle(Image_AlienPixMap) icon();

  /// Add a line into the given group
  /// \param theGroup a group for drawing
  /// \param theAttrName an attribute name which corresponds to referenced line
  void addLine(const Handle(Graphic3d_Group)& theGroup, std::string theAttrName) const;

protected:
  /// Constraint feature
  SketchPlugin_Constraint* myConstraint;

  /// Plane of the current sketcher
  std::shared_ptr<GeomAPI_Ax3> myPlane;

  /// Aspect for entities drawing
  Handle(Graphic3d_AspectMarker3d) myAspect;

private: 
  /// Static map to collect constraints icons {IconName : IconPixMap}
  static std::map<const char*, Handle(Image_AlienPixMap)> myIconsMap;
};

#endif