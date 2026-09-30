from salome.shaper import model
import ModelAPI
import traceback
import math

class SplitCylinder(model.Feature):

    def __init__(self):
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        return "SplitCylinder"

    @staticmethod
    def SOLID_ID():
        return "solid_body"

    @staticmethod
    def FACE_ID():
        return "face"

    @staticmethod
    def SELECTION_METHOD_ID():
        return "SelectionMethod"

    @staticmethod
    def SIZE_METHOD_ID():
        return "SizeMethod"

    @staticmethod
    def SIZE_INPUT_ID():
        return "size_input"

    @staticmethod
    def SIZE_PERCENTAGE_ID():
        return "size_percentage"

    def getKind(self):
        return SplitCylinder.ID()

    def initAttributes(self):
        self.data().addAttribute(self.SOLID_ID(), ModelAPI.ModelAPI_AttributeSelection_typeId())
        self.data().addAttribute(self.FACE_ID(), ModelAPI.ModelAPI_AttributeSelection_typeId())
        self.data().addAttribute(self.SELECTION_METHOD_ID(), ModelAPI.ModelAPI_AttributeString_typeId())
        self.data().addAttribute(self.SIZE_METHOD_ID(), ModelAPI.ModelAPI_AttributeString_typeId())
        self.data().addAttribute(self.SIZE_INPUT_ID(), ModelAPI.ModelAPI_AttributeDouble_typeId())
        self.data().addAttribute(self.SIZE_PERCENTAGE_ID(), ModelAPI.ModelAPI_AttributeDouble_typeId())

    def execute(self):
        try:
            self._execute_safe()
        except Exception:
            print("\n" + "="*50)
            print("ERROR IN SPLIT CYLINDER MACRO:")
            traceback.print_exc()
            print("="*50 + "\n")
            return

    def _execute_safe(self):
        doc = self.document()

        # =========================================================
        # 1. PREVIEW STATE INITIALIZATION
        # =========================================================
        # Nothing is deleted here. We only initialize the list on the
        # first run. Whether to rebuild everything or just update the
        # existing values is decided in step 5.
        if not hasattr(self, "_preview_features"):
            self._preview_features = []

        # =========================================================
        # 2. CHECK CURRENT SELECTIONS
        # =========================================================
        method_attr = self.string(self.SELECTION_METHOD_ID())
        selection_method = method_attr.value() if (method_attr and method_attr.isInitialized()) else "Cylinder"
        face_only_mode = (selection_method == "Face")

        face_attr = self.selection(self.FACE_ID())
        if face_attr is None or not face_attr.isInitialized(): return
        face_name = face_attr.namingName()
        if not face_name: return

        if face_only_mode:
            # No solid needed in this mode.
            solid_attr = None
            solid_name = None
        else:
            solid_attr = self.selection(self.SOLID_ID())
            if solid_attr is None or not solid_attr.isInitialized(): return
            solid_name = solid_attr.namingName()
            if not solid_name: return

        # =========================================================
        # 3. CACHE SYSTEM (avoids read failures on update)
        # =========================================================
        # If the selection (or the mode) changed, or this is the first
        # run, extract the geometric data and cache it.
        selection_changed = (
            not hasattr(self, "_cached_selections")
            or self._cached_selections != (face_only_mode, solid_name, face_name)
        )

        if selection_changed:

            face_shape = face_attr.value()

            R = 10.0
            H = 50.0

            if face_only_mode:
                # No solid to explore. Get the radius directly from the
                # circular edge bounding the selected face.
                try:
                    from GeomAPI import GeomAPI_Edge, GeomAPI_ShapeExplorer, GeomAPI_Shape
                    exp = GeomAPI_ShapeExplorer(face_shape, GeomAPI_Shape.EDGE)
                    while exp.more():
                        try:
                            edge = GeomAPI_Edge(exp.current())
                            circ = edge.circle()
                            R = circ.radius()
                            break
                        except:
                            pass
                        exp.next()
                except Exception:
                    traceback.print_exc()
                # H is not used in face-only mode (no extrusion), keep default.
            else:
                solid_shape = solid_attr.value()
                try:
                    from GeomAPI import GeomAPI_Face, GeomAPI_ShapeExplorer, GeomAPI_Shape
                    exp = GeomAPI_ShapeExplorer(solid_shape, GeomAPI_Shape.FACE)
                    while exp.more():
                        try:
                            f = GeomAPI_Face(exp.current())
                            cyl = f.getCylinder()
                            R = cyl.radius()
                            H = cyl.height()
                            break
                        except:
                            pass
                        exp.next()
                except Exception:
                    pass

            cx, cy, cz = 0.0, 0.0, 0.0
            try:
                mid = face_shape.middlePoint()
                try: cx, cy, cz = mid.X(), mid.Y(), mid.Z()
                except: cx, cy, cz = mid.x(), mid.y(), mid.z()
            except: pass

            # Cache the extracted data
            self._cached_R = R
            self._cached_H = H
            self._cached_center = (cx, cy, cz)
            self._cached_selections = (face_only_mode, solid_name, face_name)

        # Use the cached data
        R = self._cached_R
        H = self._cached_H
        cx, cy, cz = self._cached_center

        # =========================================================
        # 4. SIZE CALCULATION (real-time update)
        # =========================================================
        size_method_attr = self.string(self.SIZE_METHOD_ID())
        size_method = size_method_attr.value() if (size_method_attr and size_method_attr.isInitialized()) else "Value"
        is_percentage = (size_method == "Percentage")

        if is_percentage:
            input_attr = self.real(self.SIZE_PERCENTAGE_ID())
        else:
            input_attr = self.real(self.SIZE_INPUT_ID())
        if input_attr is None or not input_attr.isInitialized(): return
        user_input = input_attr.value()
        if user_input <= 0: return

        if is_percentage:
            # R is now always exact (e.g. 5). Diameter = 10. 20% will be 2.
            math_side = (2.0 * R) * (user_input / 100.0)
        else:
            math_side = user_input

        # SAFETY LIMIT
        max_side = (R * math.sqrt(2.0)) - 0.01
        if math_side > max_side:
            math_side = max_side

        h = math_side / 2.0

        L_ext = R - (h * 1.414213562)
        if L_ext < 0.1: L_ext = 0.1

        # =========================================================
        # 5. GEOMETRY: REBUILD (if selection changed) or
        #    UPDATE IN PLACE (if only the size changed)
        # =========================================================
        needs_rebuild = selection_changed or not self._preview_features

        if needs_rebuild:
            # --- If there was a preview for a different selection, delete it ---
            if self._preview_features:
                for feat in reversed(self._preview_features):
                    try:
                        doc.removeFeature(feat)
                    except Exception:
                        traceback.print_exc()
                self._preview_features = []

            # --- Full construction ---
            center_3d = model.addPoint(doc, cx, cy, cz)
            center_3d.execute(True)

            sketch = model.addSketch(doc, model.selection("FACE", face_name))
            proj_c = sketch.addProjection(model.selection(center_3d.defaultResult()), False)
            proj_c.execute(True)
            c_feat = proj_c.createdFeature()

            center_2d = None
            if c_feat is not None:
                c_feat.execute(True)
                center_2d = c_feat.result()

            p0 = sketch.addPoint(0, 0)
            p0.setAuxiliary(True)
            p0.execute(True)

            if center_2d is not None:
                try: sketch.setCoincident(p0.coordinates(), center_2d, True)
                except: pass

            sq1 = sketch.addLine(-h, h, -h, -h)
            sq2 = sketch.addLine(-h, -h, h, -h)
            sq3 = sketch.addLine(h, -h, h, h)
            sq4 = sketch.addLine(h, h, -h, h)

            c1 = sketch.addLine(-h, h, h, -h); c1.setAuxiliary(True)
            c2 = sketch.addLine(-h, -h, h, h); c2.setAuxiliary(True)

            d1 = sketch.addLine(h, h, h+1, h+1)
            d2 = sketch.addLine(-h, h, -h-1, h+1)
            d3 = sketch.addLine(-h, -h, -h-1, -h-1)
            d4 = sketch.addLine(h, -h, h+1, -h-1)

            for line in [sq1, sq2, sq3, sq4, c1, c2, d1, d2, d3, d4]: line.execute(True)

            sketch.setCoincident(sq4.endPoint(), sq1.startPoint(), True)
            sketch.setCoincident(sq1.endPoint(), sq2.startPoint(), True)
            sketch.setCoincident(sq2.endPoint(), sq3.startPoint(), True)
            sketch.setCoincident(sq3.endPoint(), sq4.startPoint(), True)
            sketch.setHorizontal(sq4.result(), True)
            sketch.setPerpendicular(sq1.result(), sq2.result(), True)
            sketch.setPerpendicular(sq2.result(), sq3.result(), True)
            sketch.setPerpendicular(sq3.result(), sq4.result(), True)

            len_sq1 = sketch.setLength(sq1.result(), math_side, True)
            len_sq2 = sketch.setLength(sq2.result(), math_side, True)

            sketch.setCoincident(sq1.startPoint(), c1.startPoint(), True)
            sketch.setCoincident(sq3.startPoint(), c1.endPoint(), True)
            sketch.setCoincident(sq2.startPoint(), c2.startPoint(), True)
            sketch.setCoincident(sq4.startPoint(), c2.endPoint(), True)
            sketch.setCoincident(p0.coordinates(), c1.result(), True)
            sketch.setCoincident(p0.coordinates(), c2.result(), True)

            sketch.setCoincident(sq4.startPoint(), d1.startPoint(), True)
            sketch.setCoincident(sq1.startPoint(), d2.startPoint(), True)
            sketch.setCoincident(sq2.startPoint(), d3.startPoint(), True)
            sketch.setCoincident(sq3.startPoint(), d4.startPoint(), True)

            sketch.setAngle(d1.result(), sq4.result(), 135.0, type="Direct", is_active=True)
            sketch.setAngle(d2.result(), sq1.result(), 135.0, type="Direct", is_active=True)
            sketch.setAngle(d3.result(), sq2.result(), 135.0, type="Direct", is_active=True)
            sketch.setAngle(d4.result(), sq3.result(), 135.0, type="Direct", is_active=True)

            len_d1 = sketch.setLength(d1.result(), L_ext, True)
            len_d2 = sketch.setLength(d2.result(), L_ext, True)
            len_d3 = sketch.setLength(d3.result(), L_ext, True)
            len_d4 = sketch.setLength(d4.result(), L_ext, True)

            sketch.execute(True)

            extrusion = None
            edge_obj = None
            if face_only_mode:
                # --- Face-only mode: split the face directly, no extrusion ---
                # Split rejects a whole sketch/face selection as a tool
                # ("result construction not allowed for selection"). It
                # needs individual EDGE selections, so first materialize
                # the sketch curves as an Edge feature, then use its
                # resulting edges as the Split tool objects.
                edge_obj = model.addEdge(
                    doc,
                    [model.selection("COMPOUND", sketch.name())],
                    False
                )
                edge_obj.execute(True)

                split = model.addSplit(
                    doc,
                    [model.selection("FACE", face_name)],
                    list(edge_obj.results()),
                    keepSubResults=True
                )
                split.execute(True)
            else:
                # --- Cylinder mode: extrude the sketch and cut the solid ---
                extrusion = model.addExtrusion(
                    doc,
                    [sketch.result()],
                    model.selection(),
                    0.0,
                    H + 0.1,
                    "Edges"
                )
                extrusion.execute(True)

                split = model.addSplit(
                    doc,
                    [model.selection("SOLID", solid_name)],
                    [extrusion.result()],
                    keepSubResults=False
                )
                split.execute(True)

            # --- Clean up extra edges left by the split (only makes ---
            # --- sense for the solid/cylinder cut, not for a flat face) ---
            remove_extra = None
            if not face_only_mode:
                # split.result() is already a ModelHighAPI_Selection, so it
                # must be passed directly (wrapping it again in
                # model.selection(...) fails, since that expects a raw
                # Result/Shape, not an already-built Selection).
                remove_extra = model.addRemoveExtraEdges(
                    doc,
                    split.result(),
                    True
                )
                remove_extra.execute(True)

            # --- Store references so we can update or delete them later ---
            self._preview_features = [center_3d.feature(), sketch.feature()]
            if extrusion is not None:
                self._preview_features.append(extrusion.feature())
            if edge_obj is not None:
                self._preview_features.append(edge_obj.feature())
            self._preview_features.append(split.feature())
            if remove_extra is not None:
                self._preview_features.append(remove_extra.feature())

            self._sq_lines = (sq1, sq2)
            self._d_lines = (d1, d2, d3, d4)
            self._length_constraints = (len_sq1, len_sq2, len_d1, len_d2, len_d3, len_d4)
            self._sketch_obj = sketch
            self._extrusion_obj = extrusion
            self._edge_obj = edge_obj
            self._split_obj = split
            self._remove_extra_obj = remove_extra
            self._face_only_mode = face_only_mode

            # --- Group all sub-features into a single collapsible tree node ---
            self._group_into_folder(doc)

        else:
            # --- Only the size changed: update the VALUE of the
            #     existing length constraints, without creating new ones ---
            values = (math_side, math_side, L_ext, L_ext, L_ext, L_ext)
            for constraint, value in zip(self._length_constraints, values):
                try:
                    constraint.feature().real("ConstraintValue").setValue(value)
                except Exception:
                    print("[ERROR updating constraint]")
                    traceback.print_exc()

            self._sketch_obj.execute(True)
            if self._extrusion_obj is not None:
                self._extrusion_obj.execute(True)
            if self._edge_obj is not None:
                self._edge_obj.execute(True)
            self._split_obj.execute(True)
            if self._remove_extra_obj is not None:
                self._remove_extra_obj.execute(True)

    def _group_into_folder(self, doc):
        """
        Groups the internal sub-features (point, sketch, extrusion,
        split, remove extra edges) into a single collapsible folder in
        the object browser tree, so the user only sees one
        "SplitCylinder" entry instead of five separate operations.

        moveToFolder expects a ModelAPI.FeatureList (a SWIG wrapper
        around std::list<FeaturePtr>), not a plain Python list, so we
        build one explicitly before calling it.
        """
        try:
            feat_list = ModelAPI.FeatureList()
            try:
                # Some SWIG std::list wrappers accept a Python iterable
                # directly in the constructor.
                feat_list = ModelAPI.FeatureList(self._preview_features)
            except Exception:
                # Fallback: build it by appending one by one.
                for f in self._preview_features:
                    feat_list.append(f)

            folder = doc.addFolder(self._preview_features[0])
            doc.moveToFolder(feat_list, folder)
            folder.data().setName("SplitCylinder")
        except Exception:
            print("[WARN] Could not group sub-features into a folder:")
            traceback.print_exc()

    def isMacro(self):
        return True