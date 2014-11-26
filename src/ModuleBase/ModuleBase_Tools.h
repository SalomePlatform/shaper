// File:        ModuleBase_Tools.h
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_Tools_H
#define ModuleBase_Tools_H

#include "ModuleBase.h"

class QWidget;
class QLayout;

namespace ModuleBase_Tools {

/*
 * Methods to adjust margins and spacings.
 */
MODULEBASE_EXPORT void adjustMargins(QWidget* theWidget);
MODULEBASE_EXPORT void adjustMargins(QLayout* theLayout);

MODULEBASE_EXPORT void zeroMargins(QWidget* theWidget);
MODULEBASE_EXPORT void zeroMargins(QLayout* theLayout);


}

#endif
