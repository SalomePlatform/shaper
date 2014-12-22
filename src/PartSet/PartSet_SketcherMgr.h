// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_SketcherMgr.h
// Created:     19 Dec 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_SketcherMgr_H
#define PartSet_SketcherMgr_H

#include "PartSet.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_CompositeFeature.h>

#include <ModuleBase_ViewerFilters.h>

#include <GeomAPI_Pln.h>

#include <QObject>
#include <QList>

class PartSet_Module;
class ModuleBase_IViewWindow;
class ModuleBase_Operation;
class QMouseEvent;


/**
* A class for management of sketch operations
*/
class PARTSET_EXPORT PartSet_SketcherMgr : public QObject
{
  Q_OBJECT
public:
  PartSet_SketcherMgr(PartSet_Module* theModule);

  virtual ~PartSet_SketcherMgr();

  static QStringList sketchOperationIdList();

  /// Launches the operation from current highlighting
  void launchEditing();

  // Returns current Sketch feature/ Returns NULL if there is no launched sketch operation
  CompositeFeaturePtr activeSketch() const { return myCurrentSketch; }

  /// Starts sketch operation
  void startSketch(ModuleBase_Operation* theOperation);

  /// Stops sketch operation
  void stopSketch(ModuleBase_Operation* theOperation);

public slots:
  void onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>& thePln);


private slots:
  void onMousePressed(ModuleBase_IViewWindow*, QMouseEvent*);
  void onMouseReleased(ModuleBase_IViewWindow*, QMouseEvent*);
  void onMouseMoved(ModuleBase_IViewWindow*, QMouseEvent*);
  void onMouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*);

private:
   /// Converts mouse position to 2d coordinates. 
   /// Member myCurrentSketch has to be correctly defined
  void get2dPoint(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent, 
                  double& theX, double& theY);


private:
  PartSet_Module* myModule;

  bool myIsDragging;
  bool myDragDone;
  double myCurX, myCurY;

  CompositeFeaturePtr myCurrentSketch;
  QList<FeaturePtr> myEditingFeatures;
  QList<AttributePtr> myEditingAttr;

  Handle(ModuleBase_ShapeInPlaneFilter) myPlaneFilter;
};


#endif