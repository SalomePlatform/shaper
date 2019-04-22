// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

/*
 * Hardcoded xml entities of plugin-*.xml
 */
const static char* NODE_WORKBENCH = "workbench";
const static char* NODE_GROUP = "group";
const static char* NODE_FEATURE = "feature";
const static char* NODE_SOURCE = "source";
const static char* NODE_VALIDATOR = "validator";
const static char* NODE_XMLPARENT = "libxml_parent";

// Property panels
const static char* PROPERTY_PANEL_ID = "property_panel_id";

// Widgets
const static char* WDG_INFO = "label";
const static char* WDG_DOUBLEVALUE = "doublevalue";
const static char* WDG_DOUBLEVALUELABEL = "labelvalue";
const static char* WDG_INTEGERVALUE = "integervalue";
const static char* WDG_BOOLVALUE = "boolvalue";
const static char* WDG_STRINGVALUE = "stringvalue";
const static char* WDG_NAMEVALUE = "namevalue";
const static char* WDG_MULTISELECTOR = "multi_selector";
const static char* WDG_SHAPE_SELECTOR = "shape_selector";
const static char* WDG_FEATURE_SELECTOR = "feature_selector";
const static char* WDG_CONCEALED_OBJECTS_VIEW = "concealed_objects_view";
const static char* WDG_CHOICE = "choice";
const static char* WDG_DOUBLEVALUE_EDITOR = "doublevalue_editor";
const static char* WDG_FILE_SELECTOR= "file_selector";
const static char* WDG_EXPR_EDITOR = "expr_editor";
const static char* WDG_PLACE_HOLDER = "placeholder";
const static char* WDG_ACTION = "action";
const static char* WDG_POINT_INPUT = "point_input";

// Containers
const static char* WDG_GROUP = "groupbox";
const static char* WDG_OPTIONALBOX = "optionalbox";
const static char* WDG_TOOLBOX = "toolbox";
const static char* WDG_RADIOBOX = "radiobox";
const static char* WDG_RADIOBOX_ITEM = "radio";
const static char* WDG_TOOLBOX_BOX = "box";
const static char* WDG_SWITCH = "switch";
const static char* WDG_SWITCH_CASE = "case";
// Common properties (xml attributes of nodes)
const static char* _ID = "id";
// NODE_WORKBENCH properties
const static char* WORKBENCH_DOC = "document";
// NODE_SOURCE properties
const static char* SOURCE_FILE = "path";
// NODE_FEATURE properties
const static char* FEATURE_TOOLTIP = "tooltip";
const static char* GROUP_TOOLBAR = "toolbar";
const static char* FEATURE_ICON = "icon";
const static char* FEATURE_TEXT = "title";
const static char* HELP_FILE = "helpfile";
const static char* FEATURE_KEYSEQUENCE = "keysequence";
const static char* FEATURE_NESTED = "nested";
const static char* FEATURE_WHEN_NESTED = "when_nested";
const static char* FEATURE_WHEN_NESTED_ACCEPT = "accept";
const static char* FEATURE_WHEN_NESTED_ABORT = "abort";
const static char* FEATURE_DOC = WORKBENCH_DOC;
const static char* FEATURE_MODAL = "modal";
const static char* FEATURE_APPLY_CONTINUE = "apply_continue";
const static char* FEATURE_AUTO_PREVIEW = "auto_preview";
const static char* FEATURE_TITLE_IN_TOOLBAR = "titleInToolbar";
// NODE_VALIDATOR properties
const static char* _PARAMETERS = "parameters";

// Widget (attribute) properties
const static char* ATTR_TOOLTIP = FEATURE_TOOLTIP;
const static char* ATTR_ICON = FEATURE_ICON;
const static char* ATTR_LABEL = "label";
const static char* ATTR_STYLE_SHEET = "styleSheet";
const static char* ATTR_DEFAULT = "default";
const static char* ATTR_INTERNAL = "internal";
const static char* ATTR_OBLIGATORY = "obligatory";
const static char* ATTR_CONCEALMENT = "concealment";
const static char* ATTR_USE_RESET = "use_reset";
const static char* ATTR_GREED = "greed";
const static char* ATTR_MODIFIED_IN_EDIT = "modified_in_edit";
const static char* ATTR_MAIN_ARG = "main_argument";
const static char* ATTR_GEOMETRICAL_SELECTION = "geometrical_selection";


// WDG_INFO properties
const static char* INFO_WDG_TEXT = FEATURE_TEXT;
const static char* INFO_WDG_TOOLTIP = FEATURE_TOOLTIP;
// WDG_DOUBLEVALUE properties:
const static char* DOUBLE_WDG_MIN = "min";
const static char* DOUBLE_WDG_MAX = "max";
const static char* DOUBLE_WDG_STEP = "step";
const static char* DOUBLE_WDG_DEFAULT_COMPUTED = "computed";
const static char* DOUBLE_WDG_ACCEPT_EXPRESSIONS = "accept_expressions";
const static char* DOUBLE_WDG_ENABLE_VALUE = "enable_value";

const static char* DOUBLE_WDG_ENABLE_VALUE_BY_PREFERENCES = "enable_by_preferences";

// WDG_TOOLBOX/WDG_SWITCH properties
const static char* CONTAINER_PAGE_NAME = "title";
const static char* CONTAINER_PAGE_ICON = "icon";

/*
 * Hardcoded xml entities of plugins.xml
 */

const static char* PLUGIN_FILE = "plugins.xml";
const static char* NODE_PLUGIN = "plugin";
const static char* NODE_PLUGINS = "plugins";

const static char* PLUGINS_MODULE = "module";
const static char* PLUGIN_CONFIG = "configuration";
const static char* PLUGIN_LIBRARY = "library";
const static char* PLUGIN_SCRIPT = "script";
const static char* PLUGIN_DEPENDENCY = "dependency";
const static char* PLUGIN_USES = "uses";

/*
 * Hardcoded xml entities of dataModel.xml
 */
const static char* DATAMODEL_FILE = "dataModel.xml";
const static char* NODE_FOLDER = "folder";
const static char* FOLDER_NAME = "name";
const static char* GROUP_TYPE = "group_type";
const static char* ROOT_DOCUMENT = "root_document";
const static char* SUB_DOCUMENT = "sub_document";
const static char* NODE_ICON = "icon";
const static char* SHOW_EMPTY = "show_empty";
const static char* LINK_ITEM = "from_result";
const static char* FOLDER_FEATURES = "folder_features";

#endif /* CONFIG_KEYWORDS_H_ */
