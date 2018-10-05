
Union
=====

Union feature implements a Boolean operation for creation of a union of selected objects.

To create a Union in the active part:

#. select in the Main Menu *Features - > Union* item  or
#. click **Union** button in the toolbar

.. image:: images/union_btn.png
   :align: center

.. centered::
   **Union**  button 

The following property panel will be opened:

.. image:: images/Union.png
  :align: center

.. centered::
   **Union operation**

Here it is necessary to select some objects. Only faces with shared edges or solids with shared faces are allowed for selection.

**Apply** button creates a union shape.
  
**Cancel** button cancels the operation.

**TUI Command**:  *model.addUnion(Part_doc, objects)*

**Arguments**:   Part + list of objects.

Result
""""""

The Result of the operation will be a shape which is a union of selected objects:

.. image:: images/CreatedUnion.png
	   :align: center

.. centered::
   **Union created**

**See Also** a sample TUI Script of :ref:`tui_create_union` operation.
