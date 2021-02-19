// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef CONFIG_KEYWORDS_H_
#define CONFIG_KEYWORDS_H_

#include <Config_def.h>

/*
 * Hardcoded xml entities of plugin-*.xml
 */
MAYBE_UNUSED const static char* NODE_WORKBENCH = "workbench";
MAYBE_UNUSED const static char* NODE_GROUP = "group";
MAYBE_UNUSED const static char* NODE_FEATURE = "feature";
MAYBE_UNUSED const static char* NODE_SOURCE = "source";
MAYBE_UNUSED const static char* NODE_VALIDATOR = "validator";
MAYBE_UNUSED const static char* NODE_XMLPARENT = "libxml_parent";

// Property panels
MAYBE_UNUSED const static char* PROPERTY_PANEL_ID = "property_panel_id";

// Widgets
MAYBE_UNUSED const static char* WDG_INFO = "label";
MAYBE_UNUSED const static char* WDG_UNDOLABEL = "undo_label";
MAYBE_UNUSED const static char* WDG_DOUBLEVALUE = "doublevalue";
MAYBE_UNUSED const static char* WDG_DOUBLEVALUELABEL = "labelvalue";
MAYBE_UNUSED const static char* WDG_INTEGERVALUE = "integervalue";
MAYBE_UNUSED const static char* WDG_BOOLVALUE = "boolvalue";
MAYBE_UNUSED const static char* WDG_STRINGVALUE = "stringvalue";
MAYBE_UNUSED const static char* WDG_NAMEVALUE = "namevalue";
MAYBE_UNUSED const static char* WDG_MULTISELECTOR = "multi_selector";
MAYBE_UNUSED const static char* WDG_SHAPE_SELECTOR = "shape_selector";
MAYBE_UNUSED const static char* WDG_FEATURE_SELECTOR = "feature_selector";
MAYBE_UNUSED const static char* WDG_CONCEALED_OBJECTS_VIEW = "concealed_objects_view";
MAYBE_UNUSED const static char* WDG_CHOICE = "choice";
MAYBE_UNUSED const static char* WDG_DOUBLEVALUE_EDITOR = "doublevalue_editor";
MAYBE_UNUSED const static char* WDG_FILE_SELECTOR= "file_selector";
MAYBE_UNUSED const static char* WDG_EXPR_EDITOR = "expr_editor";
MAYBE_UNUSED const static char* WDG_PLACE_HOLDER = "placeholder";
MAYBE_UNUSED const static char* WDG_ACTION = "action";
MAYBE_UNUSED const static char* WDG_POINT_INPUT = "point_input";
MAYBE_UNUSED const static char* WDG_SELECTION_FILTERS = "selection_filters";

// Containers
MAYBE_UNUSED const static char* WDG_GROUP = "groupbox";
MAYBE_UNUSED const static char* WDG_OPTIONALBOX = "optionalbox";
MAYBE_UNUSED const static char* WDG_TOOLBOX = "toolbox";
MAYBE_UNUSED const static char* WDG_RADIOBOX = "radiobox";
MAYBE_UNUSED const static char* WDG_RADIOBOX_ITEM = "radio";
MAYBE_UNUSED const static char* WDG_TOOLBOX_BOX = "box";
MAYBE_UNUSED const static char* WDG_SWITCH = "switch";
MAYBE_UNUSED const static char* WDG_SWITCH_CASE = "case";
MAYBE_UNUSED const static char* WDG_FILTER = "filter";

// Common properties (xml attributes of nodes)
MAYBE_UNUSED const static char* _ID = "id";
// NODE_WORKBENCH properties
MAYBE_UNUSED const static char* WORKBENCH_DOC = "document";
// NODE_SOURCE properties
MAYBE_UNUSED const static char* SOURCE_FILE = "path";
// NODE_FEATURE properties
MAYBE_UNUSED const static char* FEATURE_TOOLTIP = "tooltip";
MAYBE_UNUSED const static char* GROUP_TOOLBAR = "toolbar";
MAYBE_UNUSED const static char* FEATURE_ICON = "icon";
MAYBE_UNUSED const static char* FEATURE_TEXT = "title";
MAYBE_UNUSED const static char* HELP_FILE = "helpfile";
MAYBE_UNUSED const static char* ABORT_CONFIRMATION = "abort_confirmation";
MAYBE_UNUSED const static char* HIDEFACES_PANEL = "hidefaces_panel";
MAYBE_UNUSED const static char* FEATURE_KEYSEQUENCE = "keysequence";
MAYBE_UNUSED const static char* FEATURE_NESTED = "nested";
MAYBE_UNUSED const static char* FEATURE_WHEN_NESTED = "when_nested";
MAYBE_UNUSED const static char* FEATURE_WHEN_NESTED_ACCEPT = "accept";
MAYBE_UNUSED const static char* FEATURE_WHEN_NESTED_ABORT = "abort";
MAYBE_UNUSED const static char* FEATURE_DOC = WORKBENCH_DOC;
MAYBE_UNUSED const static char* FEATURE_MODAL = "modal";
MAYBE_UNUSED const static char* FEATURE_APPLY_CONTINUE = "apply_continue";
MAYBE_UNUSED const static char* FEATURE_AUTO_PREVIEW = "auto_preview";
MAYBE_UNUSED const static char* FEATURE_TITLE_IN_TOOLBAR = "titleInToolbar";
// NODE_VALIDATOR properties
MAYBE_UNUSED const static char* _PARAMETERS = "parameters";

// Widget (attribute) properties
MAYBE_UNUSED const static char* ATTR_TOOLTIP = FEATURE_TOOLTIP;
MAYBE_UNUSED const static char* ATTR_ICON = FEATURE_ICON;
MAYBE_UNUSED const static char* ATTR_LABEL = "label";
MAYBE_UNUSED const static char* ATTR_STYLE_SHEET = "styleSheet";
MAYBE_UNUSED const static char* ATTR_IS_SELECTABLE = "isSelectable";
MAYBE_UNUSED const static char* ATTR_HTML_STYLE = "isHTML";
MAYBE_UNUSED const static char* ATTR_DEFAULT = "default";
MAYBE_UNUSED const static char* ATTR_INTERNAL = "internal";
MAYBE_UNUSED const static char* ATTR_OBLIGATORY = "obligatory";
MAYBE_UNUSED const static char* ATTR_CONCEALMENT = "concealment";
MAYBE_UNUSED const static char* ATTR_USE_RESET = "use_reset";
MAYBE_UNUSED const static char* ATTR_GREED = "greed";
MAYBE_UNUSED const static char* ATTR_MODIFIED_IN_EDIT = "modified_in_edit";
MAYBE_UNUSED const static char* ATTR_MAIN_ARG = "main_argument";
MAYBE_UNUSED const static char* ATTR_GEOMETRICAL_SELECTION = "geometrical_selection";
MAYBE_UNUSED const static char* ATTR_VISUAL_CHANGED = "change_visual_attributes";


// WDG_INFO properties
MAYBE_UNUSED const static char* INFO_WDG_TEXT = FEATURE_TEXT;
MAYBE_UNUSED const static char* INFO_WDG_TOOLTIP = FEATURE_TOOLTIP;
// WDG_DOUBLEVALUE properties:
MAYBE_UNUSED const static char* DOUBLE_WDG_MIN = "min";
MAYBE_UNUSED const static char* DOUBLE_WDG_MAX = "max";
MAYBE_UNUSED const static char* DOUBLE_WDG_STEP = "step";
MAYBE_UNUSED const static char* DOUBLE_WDG_DEFAULT_COMPUTED = "computed";
MAYBE_UNUSED const static char* DOUBLE_WDG_ACCEPT_EXPRESSIONS = "accept_expressions";
MAYBE_UNUSED const static char* DOUBLE_WDG_ENABLE_VALUE = "enable_value";

MAYBE_UNUSED const static char* DOUBLE_WDG_ENABLE_VALUE_BY_PREFERENCES = "enable_by_preferences";

// WDG_TOOLBOX/WDG_SWITCH properties
MAYBE_UNUSED const static char* CONTAINER_PAGE_NAME = "title";
MAYBE_UNUSED const static char* CONTAINER_PAGE_ICON = "icon";

/*
 * Hardcoded xml entities of plugins.xml
 */

MAYBE_UNUSED const static char* PLUGIN_FILE = "plugins.xml";
MAYBE_UNUSED const static char* NODE_PLUGIN = "plugin";
MAYBE_UNUSED const static char* NODE_PLUGINS = "plugins";

MAYBE_UNUSED const static char* PLUGINS_MODULE = "module";
MAYBE_UNUSED const static char* PLUGIN_CONFIG = "configuration";
MAYBE_UNUSED const static char* PLUGIN_LIBRARY = "library";
MAYBE_UNUSED const static char* PLUGIN_SCRIPT = "script";
MAYBE_UNUSED const static char* PLUGIN_DEPENDENCY = "dependency";
MAYBE_UNUSED const static char* PLUGIN_USES = "uses";
MAYBE_UNUSED const static char* PLUGIN_DOCSECTION = "docsection";

/*
 * Hardcoded xml entities of dataModel.xml
 */
MAYBE_UNUSED const static char* DATAMODEL_FILE = "dataModel.xml";
MAYBE_UNUSED const static char* NODE_FOLDER = "folder";
MAYBE_UNUSED const static char* FOLDER_NAME = "name";
MAYBE_UNUSED const static char* GROUP_TYPE = "group_type";
MAYBE_UNUSED const static char* ROOT_DOCUMENT = "root_document";
MAYBE_UNUSED const static char* SUB_DOCUMENT = "sub_document";
MAYBE_UNUSED const static char* NODE_ICON = "icon";
MAYBE_UNUSED const static char* SHOW_EMPTY = "show_empty";
MAYBE_UNUSED const static char* LINK_ITEM = "from_result";
MAYBE_UNUSED const static char* FOLDER_FEATURES = "folder_features";

#endif /* CONFIG_KEYWORDS_H_ */
