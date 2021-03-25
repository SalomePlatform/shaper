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

#ifndef PartSet_WidgetFeaturePointSelector_H
#define PartSet_WidgetFeaturePointSelector_H

#include <ModelAPI_CompositeFeature.h>
#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_ViewerPrs.h>

#include "PartSet.h"
#include "PartSet_MouseProcessor.h"

#include <Quantity_Color.hxx>

#include <QObject>

#include <string>

class ModuleBase_IWorkshop;
class Config_WidgetAPI;
class ModuleBase_IViewWindow;
class ModuleBase_ViewerPrs;
class PartSet_ExternalObjectsMgr;

class GeomAPI_Pnt;
class GeomAPI_Pnt2d;
class GeomDataAPI_Point2D;

class QWidget;
class QMouseEvent;

/**
* \ingroup Modules
* Customosation of PartSet_WidgetFeaturePointSelector in order to visualize sub-shape
* by mouse move over shape in the viewer. Split of the object is performed by
* coincident points to the object. Segment between nearest coincidence is highlighted
*/
class PARTSET_EXPORT PartSet_WidgetFeaturePointSelector: public ModuleBase_WidgetShapeSelector,
                                                         public PartSet_MouseProcessor
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetFeaturePointSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                                 const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetFeaturePointSelector();

  /// Checks all widget validator if the owner is valid. Firstly it checks custom widget
  /// validating, next, the attribute's validating. It trying on the give selection to current
  /// attribute by setting the value inside and calling validators. After this, the previous
  /// attribute value is restored.The valid/invalid value is cashed.
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// Set sketcher
  /// \param theSketch a sketcher object
  void setSketcher(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Retrurns installed sketcher
  CompositeFeaturePtr sketch() const { return mySketch; }

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Processing the mouse move event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  virtual void mouseMoved(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent);

  /// Processing the mouse release event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  virtual void mouseReleased(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent);


  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  virtual bool setSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs);

  /// Fill preselection used in mouseReleased
  virtual void setPreSelection(const ModuleBase_ViewerPrsPtr& thePreSelected,
                               ModuleBase_IViewWindow* theWnd,
                               QMouseEvent* theEvent);
protected:
  /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getAttributeSelection() const;

  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Return an object and geom shape by the viewer presentation
  /// \param thePrs a selection
  /// \param theObject an output object
  /// \param theShape a shape of the selection
  virtual void getGeomSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs,
    ObjectPtr& theObject, GeomShapePtr& theShape);

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theAttribute an attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  void restoreAttributeValue(const AttributePtr& theAttribute, const bool theValid);

protected:
  bool fillFeature();

  /// Pointer to a sketch
  CompositeFeaturePtr mySketch;
  Quantity_Color myHighlightColor;
  Quantity_Color mySelectionColor;

  std::string mySelectedObjectAttribute;
  std::string mySelectedPointAttribute;
  std::string myPreviewObjectAttribute;
  std::string myPreviewPointAttribute;

  bool myHasPreview;
  std::shared_ptr<ModelAPI_Object> myPreviewObject;
  std::shared_ptr<GeomAPI_Pnt2d>   myPreviewPoint;
  PartSet_ExternalObjectsMgr* myExternalObjectMgr; ///< reference to external objects manager
};

#endif