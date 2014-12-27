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
#include <ModelAPI_Result.h>

#include <ModuleBase_ViewerFilters.h>
#include <ModuleBase_Definitions.h>

#include <GeomAPI_Pln.h>
#include <SelectMgr_IndexedMapOfOwner.hxx>

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

  /// Obtains the current selection of the object in the workshop viewer 
  /// It includes the selection in all modes of activation, even local context - vertexes, edges
  /// The result is a list of attributes of the feature of the object
  /// In addition, it retuns a list of selection modes, where the object is activated
  /// \param theObject a feature or result object
  /// \param theSketch a current sketch feature
  /// \param theWorkshop a workshop to have an access to AIS context and displayer
  /// \param theSelectedAttributes an out list of selected attributes
  /// \param theSelectedAttributes an out list of selected results
  static void getCurrentSelection(const ObjectPtr& theObject,
                                  const FeaturePtr& theSketch,
                                  ModuleBase_IWorkshop* theWorkshop,
                                  std::list<AttributePtr>& theSelectedAttributes,
                                  std::list<ResultPtr>& theSelectedResults);

  /// Applyes the current selection to the object in the workshop viewer 
  /// It includes the selection in all modes of activation, even local context - vertexes, edges
  /// The result is a list of attributes of the feature of the object
  /// In addition, it retuns a list of selection modes, where the object is activated
  /// \param theObject a feature or result object
  /// \param theSketch a current sketch feature
  /// \param theWorkshop a workshop to have an access to AIS context and displayer
  /// \param theSelectedAttributes an out list of selected attributes
  static void getSelectionOwners(const ObjectPtr& theObject,
                                  const FeaturePtr& theSketch,
                                  ModuleBase_IWorkshop* theWorkshop,
                                  const std::list<AttributePtr>& theSelectedAttributes,
                                  const std::list<ResultPtr>& theSelectedResults,
                                  SelectMgr_IndexedMapOfOwner& anOwnersToSelect);

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