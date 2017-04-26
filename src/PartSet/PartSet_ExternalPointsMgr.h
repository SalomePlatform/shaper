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

public slots:
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

  // Called on selection changed
  void onSelectionChanged();

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

  /**
  * Checks that the given object is an object of the current sketch
  * \param theRes an object to check
  * \return True if the given object is a sub-object of the current sketch
  */
  bool isSketchObject(const ObjectPtr& theRes) const;

private:
  /// Workshop
  ModuleBase_IWorkshop* myWorkshop;

  /// Current sketch
  CompositeFeaturePtr mySketch;

  /// Type for list of created AIS objects
  typedef QList<AISObjectPtr> ListOfAIS;

  /// Map of created AIS objects
  QMap<ObjectPtr, ListOfAIS> myPresentations;
};

#endif