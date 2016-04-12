// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_Keywords.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

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
const static char* NODE_SELFILTER = "selection_filter";
const static char* NODE_XMLPARENT = "libxml_parent";

// Property panels
const static char* PROPERTY_PANEL_ID = "property_panel_id";

// Widgets
const static char* WDG_INFO = "label";
const static char* WDG_ERRORINFO = "error_label";
const static char* WDG_DOUBLEVALUE = "doublevalue";
const static char* WDG_INTEGERVALUE = "integervalue";
const static char* WDG_BOOLVALUE = "boolvalue";
const static char* WDG_STRINGVALUE = "stringvalue";
const static char* WDG_MULTISELECTOR = "multi_selector";
const static char* WDG_SHAPE_SELECTOR = "shape_selector";
const static char* WDG_CHOICE = "choice";
const static char* WDG_DOUBLEVALUE_EDITOR = "doublevalue_editor";
const static char* WDG_FILE_SELECTOR= "file_selector";
const static char* WDG_EXPR_EDITOR = "expr_editor";
const static char* WDG_PLACE_HOLDER = "placeholder";

// Containers
const static char* WDG_GROUP = "groupbox";
const static char* WDG_CHECK_GROUP = "check_groupbox";
const static char* WDG_TOOLBOX = "toolbox";
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
const static char* FEATURE_ICON = "icon";
const static char* FEATURE_TEXT = "title";
const static char* FEATURE_KEYSEQUENCE = "keysequence";
const static char* FEATURE_NESTED = "nested";
const static char* FEATURE_WHEN_NESTED = "when_nested";
const static char* FEATURE_WHEN_NESTED_ACCEPT = "accept";
const static char* FEATURE_WHEN_NESTED_ABORT = "abort";
const static char* FEATURE_DOC = WORKBENCH_DOC;
const static char* FEATURE_AUTO_PREVIEW = "auto_preview";
// NODE_VALIDATOR properties, NODE_SELFILTER properties
const static char* _PARAMETERS = "parameters";

// Widget (attribute) properties
const static char* ATTR_TOOLTIP = FEATURE_TOOLTIP;
const static char* ATTR_ICON = FEATURE_ICON;
const static char* ATTR_LABEL = "label";
const static char* ATTR_DEFAULT = "default";
const static char* ATTR_INTERNAL = "internal";
const static char* ATTR_OBLIGATORY = "obligatory";
const static char* ATTR_CONCEALMENT = "concealment";
const static char* ATTR_USE_RESET = "use_reset";
const static char* ATTR_GREED = "greed";

// WDG_INFO properties
const static char* INFO_WDG_TEXT = FEATURE_TEXT;
const static char* INFO_WDG_TOOLTIP = FEATURE_TOOLTIP;
// WDG_DOUBLEVALUE properties:
const static char* DOUBLE_WDG_MIN = "min";
const static char* DOUBLE_WDG_MAX = "max";
const static char* DOUBLE_WDG_STEP = "step";
const static char* DOUBLE_WDG_DEFAULT_COMPUTED = "computed";
const static char* DOUBLE_WDG_ACCEPT_EXPRESSIONS = "accept_expressions";
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

#endif /* CONFIG_KEYWORDS_H_ */
