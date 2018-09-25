
Union
=====

The feature Union implements a boolean operation for creation of a uniono of selected objects.

To a create boolean opration Union in the active part:

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

Here it is necessary to select some objects.

**Apply** button creates the union shape.
  
**Cancel** button cancels operation.

**TUI Command**:  *model.addUnion(Part_doc, objects)*

**Arguments**:   Part + list of objects.

Result
""""""

The Result of the operation will be a shape which is a union of selected objects:

.. image:: images/CreatedUnion.png
	   :align: center

.. centered::
   **Union created**

**See Also** a sample TUI Script of a :ref:`tui_create_union` operation.
