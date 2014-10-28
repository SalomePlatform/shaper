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
  PartSet_OperationFeatureBase(const QString& theId, QObject* theParent, CompositeFeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_OperationFeatureBase();

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual CompositeFeaturePtr sketch() const;

  /// Gives the current selected objects to be processed by the operation
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mouseReleased(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer,
                             ModuleBase_ISelection* theSelection);


 protected:
  /// Set value to the active widget
  /// \param theFeature the feature
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  /// \return true if the point is set
  //bool setWidgetValue(ObjectPtr theFeature, double theX, double theY);

 protected:
  CompositeFeaturePtr mySketch;  ///< the sketch of the feature
};

#endif
