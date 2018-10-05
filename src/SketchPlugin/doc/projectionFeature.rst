
Projection
==========

| The Projection operation creates a projection of 3D edge or vertex onto the sketch plane.
| The result of projection is a line.

To create a Projection in the active Sketch:

#. select in the Main Menu *Sketch - > Projection* item  or
#. click **Projection** button in Sketch toolbar:

.. image:: images/projection.png
   :align: center

.. centered::
   **Projection**  button

Property panel:

.. image:: images/Projection_panel.png
  :align: center

.. centered::
   Projection

Input fields:

- **Object** is the 3D object (edge or vertex) to project onto the sketch plane.
- **Include into the sketch result** option defines whether to include the projection line into the sketch result.

**TUI Command**:

- *Sketch_1.addProjection(EdgeOrVertex, IncludeIntoResult)*

  **Arguments**: edge or vertex and include into the result flag

- *Sketch_1.addProjection(EdgeOrVertexName, IncludeIntoResult)*

  **Arguments**: edge or vertex name and include into the result flag

Result
""""""

Created Projection appears in the view.

.. image:: images/Projection_res.png
	   :align: center

.. centered::
   Created projection (purple line)

**See Also** a sample TUI Script of :ref:`tui_create_projection` operation.