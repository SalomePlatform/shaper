// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchLabel.h
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_WidgetSketchLabel_H
#define PartSet_WidgetSketchLabel_H

#include "PartSet.h"

#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ViewerFilters.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_AISObject.h>

#include <StdSelect_FaceFilter.hxx>
#include <TopoDS_Shape.hxx>

class QLabel;
class QTimer;
class XGUI_OperationMgr;
class XGUI_Workshop;

class PARTSET_EXPORT PartSet_WidgetSketchLabel : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  PartSet_WidgetSketchLabel(QWidget* theParent, const Config_WidgetAPI* theData,
                            const std::string& theParentId);

  virtual ~PartSet_WidgetSketchLabel();

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue() const
  {
    return true;
  }

  virtual bool restoreValue()
  {
    return true;
  }

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  QWidget* getControl() const;

  /// The methiod called when widget is activated
  virtual void activate();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  XGUI_Workshop* workshop() const { return myWorkshop; }

  void setWorkshop(XGUI_Workshop* theWork) { myWorkshop = theWork; }

  std::shared_ptr<GeomAPI_Pln> plane() const;

signals:
  void planeSelected(const std::shared_ptr<GeomAPI_Pln>& thePln);

 private slots:
  void onPlaneSelected();
  void setSketchingMode();

 private:
  AISObjectPtr createPreviewPlane(std::shared_ptr<GeomAPI_Pnt> theOrigin, 
                                  std::shared_ptr<GeomAPI_Dir> theNorm, 
                                  const int theRGB[3]);

  std::shared_ptr<GeomAPI_Dir> setSketchPlane(const TopoDS_Shape& theShape);

  void erasePreviewPlanes();
  void showPreviewPlanes();


  QLabel* myLabel;
  QString myText;
  QString myTooltip;

  XGUI_Workshop* myWorkshop;

  AISObjectPtr myYZPlane;
  AISObjectPtr myXZPlane;
  AISObjectPtr myXYPlane;
  bool myPreviewDisplayed;

  Handle(StdSelect_FaceFilter) myFaceFilter;

  QTimer* mySelectionTimer;
};

#endif
