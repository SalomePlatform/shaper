/*
 * ModuleBase_PropPanelOperation.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#include <ModuleBase_PropPanelOperation.h>
#include <QString>

/*!
 \brief Constructor
 \param XGUI_Workshop - workshop for this operation

 Constructs an empty operation. Constructor should work very fast because many
 operators may be created after starting workshop but only several from them
 may be used. As result this constructor stores given workshop in myApp field
 and set Waiting status.
 */
ModuleBase_PropPanelOperation::ModuleBase_PropPanelOperation(const QString& theId, QObject* parent)
: ModuleBase_Operation(theId, parent)
{
}

/*!
 * \brief Destructor
 */
ModuleBase_PropPanelOperation::~ModuleBase_PropPanelOperation()
{

}

