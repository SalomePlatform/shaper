// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef PartSet_OperationPrs_H
#define PartSet_OperationPrs_H

#include "PartSet.h"

#include <ModelAPI_Object.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Attribute.h>

#include <ModuleBase_IWorkshop.h>

#include <GeomAPI_Shape.h>

#include <ViewerData_AISShape.hxx>
#include <Standard_DefineHandle.hxx>

#include <Quantity_Color.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>

#include <QMap>
#include <QList>

#include <list>

DEFINE_STANDARD_HANDLE(PartSet_OperationPrs, ViewerData_AISShape)

class XGUI_Displayer;
class SelectMgr_Selection;

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide colored presentation of
* a list of shapes. It contains a shapes where the parameter feature refers. It processes the following
* types of attributes: ModelAPI_AttributeSelectionList, ModelAPI_AttributeRefAttr,
* ModelAPI_AttributeSelection and ModelAPI_AttributeReference.
* The selection for this presentation is absent.
*/
class PartSet_OperationPrs : public ViewerData_AISShape
{
public:
  /// Constructor
  Standard_EXPORT PartSet_OperationPrs(ModuleBase_IWorkshop* theWorkshop);

  /// Returns true if the feature contains shapes or results
  bool hasShapes();

  /// Sets the colors for the presentation compute
  /// \param theColor an argument shapes color
  void setShapeColor(const Quantity_Color& theColor);

  /// Switch on using of the AIS presentation with of the shape object increased on the delta
  void useAISWidth();

  DEFINE_STANDARD_RTTIEXT(PartSet_OperationPrs, ViewerData_AISShape)

protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(
            const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
            const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                                const Standard_Integer aMode) ;

protected:
  /// list of visualized shapes
  /// \return a map of shapes
  NCollection_DataMap<TopoDS_Shape, Handle(AIS_InteractiveObject)>& shapesMap();

private:
  /// Fills the map by the feature object and shapes, which should be visuaziled
  /// Gets feature attributes, collect objects to whom the attributes refer
  /// \param theFeature a current feature
  /// \param theWorkshop a current workshop
  /// \param theObjectShapes an output map
  /// \param theObjectShape an output map of objects
  static void getFeatureShapes(const FeaturePtr& theFeature,
                               ModuleBase_IWorkshop* theWorkshop,
                               QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes);

  /// Fills the map by the feature object and shapes, which should be visuaziled
  /// Gets feature attributes, collect objects to whom the attributes refer
  /// \param theFeature a current feature
  /// \param theWorkshop a current workshop
  /// \param theObjectShapes an output map
  /// \param theObjectShape an output map of objects
  static void getResultShapes(const FeaturePtr& theFeature,
                              ModuleBase_IWorkshop* theWorkshop,
                              QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes,
                              const bool theListShouldBeCleared = true);

  /// Fills the map by the feature presentation if the feature is presentable
  /// \param theFeature a current feature
  /// \param theWorkshop a current workshop
  /// \param theObjectShapes an output map
  /// \param theObjectShape an output map of objects
  static void getPresentationShapes(const FeaturePtr& theFeature,
                              ModuleBase_IWorkshop* theWorkshop,
                              QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes,
                              const bool theListShouldBeCleared = true);

  /// Fills the map by the feature object and shapes, which should be visuaziled
  /// Gets the active widget, obtain the highlighted presentations if it has such and
  /// fill map by object and shapes
  /// \param theWorkshop a current workshop
  /// \param theObjectShapes an output map of objects
  static void getHighlightedShapes(ModuleBase_IWorkshop* theWorkshop,
                                   QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes);

  /// Returns true if the attribute type belong to reference attribute
  /// \param theAttribute an attribute
  /// \return a boolean value
  static bool isSelectionAttribute(const AttributePtr& theAttribute);

  /// Appends the shape for the result into the container if the result is visible and
  /// the shape is not null.
  /// \param theWorkshop a current workshop
  /// \param theResult an object to be appended
  /// \param theGeomShape a shape to be appended
  /// \param theObjectShapes a filled container
  static void addValue(const ObjectPtr& theObject, const GeomShapePtr& theShape,
                const FeaturePtr& theFeature, ModuleBase_IWorkshop* theWorkshop,
                QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes);

  /// Appends the shape for the result into the container if the result is visible and
  /// the shape is not null.
  /// \param theWorkshop a current workshop
  /// \param theObject an object to be appended
  /// \param theGeomShape a shape to be appended
  /// \param theObjectShapes a filled container
  static void appendShapeIfVisible(ModuleBase_IWorkshop* theWorkshop,
                                   const ObjectPtr& theObject,
                                   GeomShapePtr theGeomShape,
                                   QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes);

  /// Fills the list of shapes by map of model objects
  /// \param theFeatureShape a container to find shapes
  /// \param theShapesMap an out container
  static void fillShapeList(const QMap<ObjectPtr, QList<GeomShapePtr> >& theFeatureShapes,
          ModuleBase_IWorkshop* theWorkshop,
          NCollection_DataMap<TopoDS_Shape, Handle(AIS_InteractiveObject)>& theShapeToPrsMap);

private:
  /// list of visualized shapes
  NCollection_DataMap<TopoDS_Shape, Handle(AIS_InteractiveObject)> myShapeToPrsMap;

  ModuleBase_IWorkshop* myWorkshop; /// current workshop
  Quantity_Color myShapeColor; /// color of feature depended shapes

  /// flag if the width of a shape object should be used for the shape visualization
  bool myUseAISWidth;

  friend class PartSet_CustomPrs;
};


#endif