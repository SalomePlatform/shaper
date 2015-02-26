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

/// the plane edge width
#define SKETCH_WIDTH        "4"

/// face of the square-face displayed for selection of general plane
#define PLANE_SIZE          "200"

/**
* \ingroup Modules
* A model widget implementation for a label which provides specific behaviour 
* for sketcher starting and launching operations
*/
class PARTSET_EXPORT PartSet_WidgetSketchLabel : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetSketchLabel(QWidget* theParent, const Config_WidgetAPI* theData,
                            const std::string& theParentId);

  virtual ~PartSet_WidgetSketchLabel();

  virtual bool restoreValue()
  {
    return true;
  }

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Returns pointer to workshop
  XGUI_Workshop* workshop() const { return myWorkshop; }

  /// Set pointer to workshop
  /// \param theWork a pointer to workshop
  void setWorkshop(XGUI_Workshop* theWork) { myWorkshop = theWork; }

  /// Returns sketcher plane
  std::shared_ptr<GeomAPI_Pln> plane() const;

signals:
  /// Signal on plane selection
  void planeSelected(const std::shared_ptr<GeomAPI_Pln>& thePln);

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValue() const
  {
    return true;
  }

  /// The methiod called when widget is activated
  virtual void activateCustom();

 private slots:
   /// Slot on plane selection
  void onPlaneSelected();

  /// Set sketch specific mode of selection
  void setSketchingMode();

 private:
   /// Create preview of planes for sketch plane selection
   /// \param theOrigin an origin of the plane
   /// \param theNorm a normal vector of the plane
   /// \param theRGB a color of plane presentation [r, g, b] array
  AISObjectPtr createPreviewPlane(std::shared_ptr<GeomAPI_Pnt> theOrigin, 
                                  std::shared_ptr<GeomAPI_Dir> theNorm, 
                                  const int theRGB[3]);

  /// Set sketch plane by shape
  /// \param theShape a planar face
  std::shared_ptr<GeomAPI_Dir> setSketchPlane(const TopoDS_Shape& theShape);

  /// Erase preview planes
  void erasePreviewPlanes();

  /// Show preview planes
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
