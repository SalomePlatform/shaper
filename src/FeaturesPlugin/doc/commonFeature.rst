Common
======

Common feature implements a Boolean operation for extraction of a common part from a set of selected shapes.

To perform a Common operation in the active part:

#. select in the Main Menu *Features - > Common* item  or
#. click **Common** button in the toolbar

.. image:: images/bool_common.png
   :align: center

.. centered::
   **Common**  button 

There are 2 options for creation of a Common:

.. image:: images/bool_common_simple.png
   :align: left
**Simple** common part between all selected objects.

.. image:: images/bool_common_advanced.png
   :align: left
**Advanced** common part for each object between this object and all tools.


Simple
------

.. image:: images/boolean_common_simple_property_panel.png
   :align: center

.. centered::
   **Simple**

- **Objects** contains a list of objects selected in the Object Browser or in the Viewer.
  If a subshape that belongs to a compsolid/compound was selected, other shapes of this compsolid/compound will be ignored.
- **See preview** button shows a result of the operation.

**TUI Command**:  *model.addCommon(Part_doc, objects)*

**Arguments**:   Part + list of objects.

Result
""""""

The Result of the operation will be a shape which is a common for all selected shapes:

.. image:: images/CreatedCommon.png
	   :align: center

.. centered::
   **Common created**

**See Also** a sample TUI Script of :ref:`tui_create_common` operation.

Advanced
--------

.. image:: images/boolean_common_advanced_property_panel.png
   :align: center

.. centered::
   **Advanced**

- **Objects** contains a list of objects selected in the Object Browser or in the Viewer.
  If a subshape that belongs to a compsolid/compound was selected, other shapes of this compsolid/compound will be cut from
  other objects (to avoid self-intersection) and added to the result.
- **Tools** contains a list of objects selected in the Object Browser or in the Viewer, which will be fused with tool objects.
  If a subshape that belongs to a compsolid/compound was selected, other shapes of this compsolid/compound  will be ignored.
- **See preview** button shows a result of the operation.

**TUI Command**:  *model.addCommon(Part_doc, objects, tools)*

**Arguments**:   Part + list of objects + list of tools.

Result
""""""

For each selected object, the common part between this object and all tools will be found:

.. image:: images/boolean_common_result.png
	   :align: center

.. centered::
   **Created common**

**See Also** a sample TUI Script of :ref:`tui_create_common` operation.
