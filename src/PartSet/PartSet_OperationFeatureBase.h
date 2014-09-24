// File:        PartSet_OperationFeatureBase.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PARTSET_OPERATIONFEATUREBASE_H
#define PARTSET_OPERATIONFEATUREBASE_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <PartSet_Constants.h>

#include <QObject>

class GeomDataAPI_Point2D;
class QMouseEvent;
class QKeyEvent;

/*!
 \class PartSet_OperationFeatureBase
 * \brief The operation for the sketch feature creation
 */
class PARTSET_EXPORT PartSet_OperationFeatureBase : public PartSet_OperationSketchBase
{
Q_OBJECT

 public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  /// \param theSketch the parent feature
  PartSet_OperationFeatureBase(const QString& theId, QObject* theParent, FeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_OperationFeatureBase();

  /// Initialisation of operation with preliminary selection
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void initSelection(const std::list<ModuleBase_ViewerPrs>& theSelected,
                             const std::list<ModuleBase_ViewerPrs>& theHighlighted);

  /// Initializes the operation with previously created feature. It is used in sequental operations
  virtual void initFeature(FeaturePtr theFeature);

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual FeaturePtr sketch() const;

  /// Gives the current selected objects to be processed by the operation
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                             const std::list<ModuleBase_ViewerPrs>& theSelected,
                             const std::list<ModuleBase_ViewerPrs>& theHighlighted);


 public slots:
  /// Slots which listen the mode widget activation
  /// \param theWidget the model widget
  virtual void onWidgetActivated(ModuleBase_ModelWidget* theWidget);

 protected:
  ///
 void activateByPreselection();
  /// Set value to the active widget
  /// \param theFeature the feature
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  /// \return true if the point is set
  bool setWidgetValue(ObjectPtr theFeature, double theX, double theY);

 protected:
  FeaturePtr myInitFeature;  ///< the initial feature
  FeaturePtr mySketch;  ///< the sketch of the feature
  
  ModuleBase_ModelWidget* myActiveWidget;  ///< the active widget

  std::list<ModuleBase_ViewerPrs> myPreSelection;
};

#endif
