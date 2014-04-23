#ifndef PartSet_OperationSketchBase_H
#define PartSet_OperationSketchBase_H

#include "PartSet.h"

#include <TopoDS_Shape.hxx>

#include <ModuleBase_PropPanelOperation.h>
#include <QObject>

/*!
 \class PartSet_OperationSketchBase
 * \brief The base operation for the sketch features.
 *
 *  Base class for all sketch operations. It provides an access to the feature preview
*/
class PARTSET_EXPORT PartSet_OperationSketchBase : public ModuleBase_PropPanelOperation
{
Q_OBJECT
public:
  PartSet_OperationSketchBase(const QString& theId, QObject* theParent);
  virtual ~PartSet_OperationSketchBase();

  const TopoDS_Shape& preview() const;

signals:
  /**
   * The signal about preview visualization.
   * \param isDisplay a state whether the preview should be displayed or erased
   */
  void visualizePreview(bool isDisplay);

protected:
  virtual void startOperation();
  virtual void stopOperation();
};

#endif
