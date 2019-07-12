.. |feature_vertex.icon|    image:: images/feature_vertex.png

Vertex
======

Vertex feature creates one or several vertices using vertices already existing in other objects.

To create vertices in the active part:

#. select in the Main Menu *Build - > Vertex* item  or
#. click |feature_vertex.icon| **Vertex** button in the toolbar

The following property panel will be opened:

.. image:: images/Vertex.png
  :align: center

.. centered::
  Create vertices

Select one or several vertices in the viewer.

**Apply** button creates vertices.

**Cancel** button cancels operation. 

**TUI Command**:

.. py:function:: model.addVertex(Part_doc, Shapes)

    :param part: The current part object.
    :param list: A list of shapes.
    :return: Result object.

Result
""""""

The result of the operation will be a set of vertices created from the selected shapes:

.. image:: images/CreateVertex.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_vertex` operation.
