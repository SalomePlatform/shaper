// Copyright (C) 2014-2023  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef XGUI_H
#define XGUI_H

//#define BEFORE_TRIHEDRON_PATCH

#if defined XGUI_EXPORTS
#if defined WIN32
#define XGUI_EXPORT              __declspec( dllexport )
#else
#define XGUI_EXPORT
#endif
#else
#if defined WIN32
#define XGUI_EXPORT              __declspec( dllimport )
#else
#define XGUI_EXPORT
#endif
#endif

#if defined __GNUC__ || defined __clang__
#define MAYBE_UNUSED __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif

#endif
