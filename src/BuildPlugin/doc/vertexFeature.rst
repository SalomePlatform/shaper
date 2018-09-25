
Vertex
======

The Vertex feature creates one or several vertices using already existing vertices in other objects.

To create vertices in the active part:

#. select in the Main Menu *Build - > Vertex* item  or
#. click **Vertex** button in the toolbar

.. image:: images/feature_vertex.png
  :align: center

.. centered::
  **Vertex** button

The following property panel will be opened:

.. image:: images/Vertex.png
  :align: center

.. centered::
  Create vertices

Select one or several vertices in viewer.

**Apply** button creates vertices.

**Cancel** button cancels operation. 

**TUI Command**:  *model.addVertex(Part_doc, Shapes)*

**Arguments**:   Part document + list of shapes.

Result
""""""

The result of the operation will be set of vertices created from selected shapes:

.. image:: images/CreateVertex.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of a :ref:`tui_create_vertex` operation.
