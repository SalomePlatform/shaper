// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchLabel.h
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_WidgetSketchLabel_H
#define PartSet_WidgetSketchLabel_H

#include "PartSet.h"

#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_ViewerFilters.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_AISObject.h>

#include <TopoDS_Shape.hxx>

class QLabel;
class XGUI_OperationMgr;
class XGUI_Workshop;
class QCheckBox;

/// the plane edge width
#define SKETCH_WIDTH        "4"

/// face of the square-face displayed for selection of general plane
#define PLANE_SIZE          "200"

/**
* \ingroup Modules
* A model widget implementation for a label which provides specific behaviour 
* for sketcher starting and launching operations
*/
class PARTSET_EXPORT PartSet_WidgetSketchLabel : public ModuleBase_WidgetValidated
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetSketchLabel(QWidget* theParent, const Config_WidgetAPI* theData,
                            const std::string& theParentId, bool toShowConstraints);

  virtual ~PartSet_WidgetSketchLabel();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// The method is called by the current operation to process the operation preselection.
  /// It is redefined to do nothing if the plane of the sketch has been already set.
  /// \param theValues the wrapped selection values
  /// \param thePosition an index in the list of values, the values should be get from the index
  virtual bool setSelection(const QList<ModuleBase_ViewerPrs>& theValues, int& thePosition);

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

  /// This control accepts focus
  virtual bool focusTo();
  virtual void setHighlighted(bool) { /*do nothing*/ };
  virtual void enableFocusProcessing();

  void showConstraints(bool theOn);

signals:
  /// Signal on plane selection
  void planeSelected(const std::shared_ptr<GeomAPI_Pln>& thePln);

  void showConstraintToggled(bool);

protected:
  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  virtual void storeAttributeValue();

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const bool theValid);

  /// Fills the attribute with the value of the selected owner
  /// \param theOwner a selected owner
  virtual bool setSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const
  {
    return true;
  }

  /// The methiod called when widget is activated
  virtual void activateCustom();

 protected:
  /// Activate or deactivate selection
  void activateSelection(bool toActivate);

 private slots:
   /// Slot on change selection
  void onSelectionChanged();

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

  QCheckBox* myShowConstraints;
};

#endif
