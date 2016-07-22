// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSubShapeSelector.h
// Created:     21 Jul 2016
// Author:      Natalia ERMOLAEVA


#ifndef PartSet_WidgetSubShapeSelector_H
#define PartSet_WidgetSubShapeSelector_H

#include "PartSet.h"

#include <PartSet_WidgetShapeSelector.h>
#include <PartSet_MouseProcessor.h>

#include <QObject>

#include <set>
#include <map>

class ModuleBase_IWorkshop;
class Config_WidgetAPI;
class ModuleBase_IViewWindow;
class ModuleBase_ViewerPrs;

class QWidget;
class QMouseEvent;

/**
* \ingroup Modules
* Customosation of PartSet_WidgetSubShapeSelector in order to visualize sub-shape 
* by mouse move over shape in the viewer. Split of the object is performed by
* coincident points to the object. Segment between nearest coincidence is highlighted
*/
class PARTSET_EXPORT PartSet_WidgetSubShapeSelector: public PartSet_WidgetShapeSelector,
                                                     public PartSet_MouseProcessor
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetSubShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                                 const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetSubShapeSelector();

  /// Processing the mouse move event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  virtual void mouseMoved(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent);

  /// Returns values which should be highlighted when the whidget is active
  /// \param theValues a list of presentations
  virtual void getHighlighted(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

protected:
  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  //virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Return an object and geom shape by the viewer presentation
  /// \param thePrs a selection
  /// \param theObject an output object
  /// \param theShape a shape of the selection
  //virtual void getGeomSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs,
  //                              ObjectPtr& theObject,
  //                              GeomShapePtr& theShape);
  void fillObjectShapes(const ObjectPtr& theObject);

protected:
  std::shared_ptr<ModuleBase_ViewerPrs> myCurrentSubShape;
  std::map<ObjectPtr, std::set<GeomShapePtr> > myCashedShapes;
};

#endif