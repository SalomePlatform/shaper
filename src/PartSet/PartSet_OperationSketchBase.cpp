#include <PartSet_OperationSketchBase.h>

#include <SketchPlugin_Feature.h>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

/*!
 \brief Constructor
  \param theId an feature index
  \param theParent the object parent
 */
PartSet_OperationSketchBase::PartSet_OperationSketchBase(const QString& theId,
	                                                     QObject* theParent)
: ModuleBase_PropPanelOperation(theId, theParent)
{
}

/*!
 * \brief Destructor
 */
PartSet_OperationSketchBase::~PartSet_OperationSketchBase()
{
}

/**
 * Returns the feature preview shape
 */
const TopoDS_Shape& PartSet_OperationSketchBase::preview() const
{
  shared_ptr<SketchPlugin_Feature> aFeature = dynamic_pointer_cast<SketchPlugin_Feature>(feature());
  return *(static_cast<TopoDS_Shape*>(aFeature->preview()->implementation()));
}

/*!
 * Perform the operation start and emit signal about visualization of the operation preview
 */
void PartSet_OperationSketchBase::startOperation()
{
  ModuleBase_PropPanelOperation::startOperation();

  emit visualizePreview(true);
}

/*!
 * Perform the operation stop and emit signal about visualization stop of the operation preview
 */
void PartSet_OperationSketchBase::stopOperation()
{
  ModuleBase_PropPanelOperation::stopOperation();

  emit visualizePreview(false);
}
