
Remove Sub-Shapes
=================

Remove Sub-Shapes allows to remove sub shapes from wires, shells, compsolids, compounds.

To perform Remove Sub-Shapes in the active part:

#. select in the Main Menu *Features - > Remove Sub-Shapes* item  or
#. click **Remove Sub-Shapes** button in the toolbar

.. image:: images/remove_subshapes_btn.png
   :align: center

.. centered::
   **Remove Sub-Shapes** button

The following property panel will be opened:

.. image:: images/RemoveSubShapes.png
  :align: center

.. centered::
  Remove Sub-Shapes

- **Shape** shape from which sub-shapes should be removed. Only wires, shells, compsolids, compounds allowed for selection.

There are 2 options for removing sub-shapes:

.. image:: images/remove_subshapes_32x32.png
   :align: left
**By removing** allows to select which sub-shapes will be removed.

- **Sub-Shapes to remove** list of sub-shapes which should be removed.

**TUI Command**:
 | *Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, object)*
 | *Remove_SubShapes_1.setSubShapesToRemove(subShapes)*

**Arguments**: Part + object; List of sub-shapes to remove.


.. image:: images/keep_subshapes_32x32.png
   :align: left
**By keeping** allows to select which sub-shapes will be kept.

- **Sub-Shapes to keep** list of sub-shapes which should be kept.

**TUI Command**:
 | *Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, object)*
 | *Remove_SubShapes_1.setSubShapesToKeep(subShapes)*

**Arguments**: Part + object; List of sub-shapes to keep.


Result
""""""

The Result of the operation will be a shape without removed sub-shapes:

.. image:: images/remove_subshapes_result.png
	   :align: center

.. centered::
   **Remove Sub-Shapes created**

**See Also** a sample TUI Script of a :ref:`tui_create_remove_subshapes` operation.
