.. _buildFace:

Face
====

Face feature creates one or several faces using shapes already existing in other objects.

To create a face in the active part:

#. select in the Main Menu *Build - > Face* item  or
#. click **Face** button in the toolbar

.. image:: images/feature_face.png
  :align: center

.. centered::
  **Face** button

The following property panel will be opened:
   
.. image:: images/Face.png
  :align: center

.. centered::
  Create a face
  
Select one or several faces in viewer.

**Apply** button creates faces.

**Cancel** button cancels the operation. 

**TUI Command**:  *model.addFace(Part_doc, Shapes)*

**Arguments**:   Part document + list of shapes.

Result
""""""

The result of the operation will be a set of faces created from the selected shapes:

.. image:: images/CreateFace.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_face` operation.
