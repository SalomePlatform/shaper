.. |shape_group.icon|    image:: images/shape_group.png

Group
=====

A group is a set of geometrical entities created by selection of sub-shapes of the same type.
To create a Group in the active part:

#. select in the Main Menu *Features - > Group* item  or
#. click |shape_group.icon| **Group** button in the toolbar:

The following property panel appears. 

.. image:: images/group_property_panel.png
  :align: center

.. centered::
  Create a group

Input fields:

- **Name** defines the name of the group, by default, it is **Group_n**.
- **Type** buttons select geometrical entities to be included in group:
  
    .. image:: images/group_property_panel_vertice.png
         :align: left
    Vertices;
  
    .. image:: images/group_property_panel_edge.png
         :align: left
    Edges;

    .. image:: images/group_property_panel_face.png
         :align: left
    Faces;

    .. image:: images/group_property_panel_solid.png
         :align: left	       
    Solids;
 
- The list of selected entities of the given type.  Multiple selection can be done manually in OCC 3D Viewer by mouse click with Shift button pressed or by rectangle selection. To delete entities from the list, select them and call pop-up menu *Delete* item.

- **Selection by filters** button opens "selection by filters" property panel and allows to select entities by set of selected filters.

- **Show only** button allows to hide all other objects and shapes and show only selected ones.

- Control **Add elements that share the same topology** allows to add automatically all elements laying on the same topology that was selected and remove these elements if they have disappeared due to the parametrical update. They will be represented as one line in the list of selection and managed in the viewer as one object if this flag is enabled. If flag becomes disabled when such elements are already in the list, they will become divided in one line per one shape.

**Selection by filters**

If user press this button then following property panel will be opened.

.. image:: images/selection_by_filters.png
  :align: center

.. centered::
  Selection by filters property panel

**TUI Command**:

.. py:function:: model.addGroup(Part_1_doc, [model.selection("EDGE", "Cone_1_1/Face_1&Cone_1_1/Face_2"), model.selection("EDGE", "Cone_1_1/Face_1")])

    :param part: The current part object
    :param list: A list of selected entities with type
    :return: Created object.

Result
""""""

Created Group appears in the view.

.. image:: images/group_res.png
	   :align: center

.. centered::
   Created group

**See Also** a sample TUI Script of :ref:`tui_create_group` operation.
