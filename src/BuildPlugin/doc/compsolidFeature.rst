
CompSolid
=========

Compsolid feature creates a compsolid using shapes already existing in other objects.

To create a compsolid in the active part:

#. select in the Main Menu *Build - > CompSolid* item  or
#. click **CompSolid** button in the toolbar

.. image:: images/feature_compsolid.png
  :align: center

.. centered::
  **CompSolid** button

The following property panel will be opened:

.. image:: images/Compsolid.png
  :align: center

.. centered::
  Create a compsolid

Select one or several shapes in the viewer.

**Apply** button creates a compsolid.

**Cancel** button cancels the operation. 

**TUI Command**:  *model.addCompSolid(Part_doc, Shapes)*

**Arguments**:   Part document + list of shapes.

Result
""""""

The result of the operation will be a compsolid created from the selected shapes:

.. image:: images/CreateCompsolid.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_compsolid` operation.
