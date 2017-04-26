// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_ExternalPointsMgr.h
// Created:     26 April 2017
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_ExternalPointsMgr_H
#define PartSet_ExternalPointsMgr_H


#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Result.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_AISObject.h>

#include <QObject>
#include <QMap>

class ModuleBase_ViewerPrs;
class ModuleBase_IWorkshop;


/**
* \ingroup Modules
* A class to manage display of presentations of center points of circular
* edges outside of a current sketcher
*/
class PartSet_ExternalPointsMgr: public QObject
{
  Q_OBJECT
public:
  PartSet_ExternalPointsMgr(ModuleBase_IWorkshop* theWorkshop, const CompositeFeaturePtr& theSketch);

  virtual ~PartSet_ExternalPointsMgr();

private slots:
  /**
  * A slot which processes display of object
  * \param theObj the displayed object
  * \param theAIS its presentation
  */
  void onDisplayObject(ObjectPtr theObj, AISObjectPtr theAIS);

  /**
  * A slot which processes erase of object
  * \param theObj the displayed object
  * \param theAIS its presentation
  */
  void onEraseObject(ObjectPtr theObj, AISObjectPtr theAIS);

private:
  /**
  * Returns list of presentations which have displayed shapes with circular edges
  * (circles, arcs) which are in pane of of the given sketch
  * \param theSketch - the sketch
  */
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> findCircularEdgesInPlane();

  /// Creates presentations of centers
  void updateCenterPresentations();

  /// Returns plane of the current sketch
  GeomPlanePtr plane() const;

  bool isSketchObject(const ObjectPtr& theRes) const;

private:
  ModuleBase_IWorkshop* myWorkshop;
  CompositeFeaturePtr mySketch;

  typedef QList<AISObjectPtr> ListOfAIS;

  QMap<ObjectPtr, ListOfAIS> myPresentations;
};

#endif