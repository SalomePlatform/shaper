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
  /// It includes the selection in all modes of activation, even local context - vertices, edges
  /// It gets all results of the feature, find an AIS object in the viewer and takes all BRep
  /// selection owners. If the owner is vertex, the corresponded attribute is seached in
  /// the feature, if the owner is edge, the current result is added to the container of results.
  /// \param theObject a feature or result object
  /// \param theSketch a current sketch feature
  /// \param theWorkshop a workshop to have an access to AIS context and displayer
  /// \param theSelectedAttributes an out list of selected attributes
  /// \param theSelectedResults an out list of selected results
  static void getCurrentSelection(const ObjectPtr& theObject,
                                  const FeaturePtr& theSketch,
                                  ModuleBase_IWorkshop* theWorkshop,
                                  std::set<AttributePtr>& theSelectedAttributes,
                                  std::set<ResultPtr>& theSelectedResults);

  /// Applyes the current selection to the object in the workshop viewer 
  /// It includes the selection in all modes of activation, even local context - vertexes, edges
  /// It gets all results of the feature, find an AIS object in the viewer and takes all BRep
  /// selection owners. If the owner is vertex, the corresponded attribute is seached in
  /// the feature and if it is in the container of selected attributes, the owner is put in the
  /// out container. If the owner is edge and the current result is in the container of selected
  /// results, the owner is put in the out container.
  /// \param theObject a feature or result object
  /// \param theSketch a current sketch feature
  /// \param theWorkshop a workshop to have an access to AIS context and displayer
  /// \param theSelectedAttributes an list of selected attributes
  /// \param theSelectedResults an list of selected results
  /// \param theOwnersToSelect an out container of found owners
  static void getSelectionOwners(const ObjectPtr& theObject,
                                  const FeaturePtr& theSketch,
                                  ModuleBase_IWorkshop* theWorkshop,
                                  const std::set<AttributePtr>& theSelectedAttributes,
                                  const std::set<ResultPtr>& theSelectedResults,
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