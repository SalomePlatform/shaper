// File:        PartSet_OperationFeatureEdit.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationFeatureEdit_H
#define PartSet_OperationFeatureEdit_H

#include "PartSet.h"

#include <PartSet_OperationFeatureBase.h>
#include <QObject>

class QMouseEvent;
class ModuleBase_ISelection;

/*!
 \class PartSet_OperationFeatureEdit
 * \brief The operation for the sketch feature creation
 */
class PARTSET_EXPORT PartSet_OperationFeatureEdit : public PartSet_OperationFeatureBase
{
Q_OBJECT
  /// Struct to define gp point, with the state is the point is initialized
  struct Point
  {
    /// Constructor
    Point()
    {
      myIsInitialized = false;
    }
    /// Constructor
    /// \param thePoint the point
    Point(gp_Pnt thePoint)
    {
      setPoint(thePoint);
    }
    ~Point()
    {
    }

    /// clear the initialized flag.
    void clear()
    {
      myIsInitialized = false;
    }
    /// set the point and switch on the initialized flag
    /// \param thePoint the point
    void setPoint(const gp_Pnt& thePoint)
    {
      myIsInitialized = true;
      myPoint = thePoint;
    }

    bool myIsInitialized;  /// the state whether the point is set
    gp_Pnt myPoint;  /// the point
  };

 public:
  /// Returns the operation type key
  static std::string Type()
  {
    return "EditLine";
  }

 public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  /// \param theFeature the parent feature
  PartSet_OperationFeatureEdit(const QString& theId, QObject* theParent, CompositeFeaturePtr theFeature);
  /// Destructor
  virtual ~PartSet_OperationFeatureEdit();


  /// Initialisation of operation with preliminary selection
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  /// \param theViewer a viewer to have the viewer the eye position
  virtual void initSelection(ModuleBase_ISelection* theSelection,
                             ModuleBase_IViewer* theViewer);

  /// Processes the mouse pressed in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mousePressed(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer, ModuleBase_ISelection* theSelection);

  /// Gives the current mouse point in the viewer
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  virtual void mouseMoved(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer);
  /// Gives the current selected objects to be processed by the operation
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mouseReleased(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer,
                             ModuleBase_ISelection* theSelection);

  /// Processes the mouse double click in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mouseDoubleClick(QMouseEvent* theEvent, Handle_V3d_View theView,
                                ModuleBase_ISelection* theSelection);

 protected:
  /// \brief Virtual method called when operation is started
  /// Virtual method called when operation started (see start() method for more description)
  /// Switch off the multi selection state
  virtual void startOperation();

  /// Virtual method called when operation stopped - committed or aborted.
  /// Restore the multi selection state
  virtual void stopOperation();

  /// Creates an operation new feature
  /// Returns NULL feature. This is an operation of edition, not creation.
  /// \param theFlushMessage the flag whether the create message should be flushed
  /// \returns the created feature
  virtual FeaturePtr createFeature(const bool theFlushMessage = true, 
    CompositeFeaturePtr theCompositeFeature = CompositeFeaturePtr());

 protected:
  /// Emits a signal about the selection blocking. Emits a signal to change the selection.
  /// If the block is true, the signal clear selection, otherwise if restore selection flag allows,
  /// the internal operation features are to be selected
  /// \param isBlocked the state whether the operation is blocked or unblocked
  /// \param isRestoreSelection the state whether the selected objects should be reselected
  //void blockSelection(bool isBlocked, const bool isRestoreSelection = true);

  /// Sends the features
  void sendFeatures();

 private:
  // the next map should be removed when selection is processed in the move function
  std::map<FeaturePtr, std::list<std::string> > myFeature2Attribute; /// a map of a feature to attributes

  Point myCurPoint;  ///< the current 3D point clicked or moved
  bool myIsBlockedSelection;  ///< the state of the last state of selection blocked signal
  bool myIsMultiOperation; // the state whether the operation is used for some features
};

#endif
