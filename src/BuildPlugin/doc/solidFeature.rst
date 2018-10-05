
Solid
=====

Solid feature creates a solid using faces already existing in other objects.

To create a solid in the active part:

#. select in the Main Menu *Build - > Solid* item  or
#. click **Solid** button in the toolbar

.. image:: images/feature_solid.png
  :align: center

.. centered::
  **Solid** button

The following property panel will be opened:

.. image:: images/Solid.png
  :align: center

.. centered::
  Create a solid
  
Select one or several faces in the viewer. The selected faces have to define a closed volume.

**Apply** button creates a solid.

**Cancel** button cancels the operation. 

**TUI Command**:  *model.addSolid(Part_doc, Shapes)*

**Arguments**:   Part document + list of shapes.

Result
""""""

The result of the operation will be a solid created from the selected shapes:

.. image:: images/CreateSolid.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_solid` operation.
