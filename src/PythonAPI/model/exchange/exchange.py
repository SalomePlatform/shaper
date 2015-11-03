"""Import Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addImport(part, *args):
    """Add an Import feature to the Part and return Import.

    Pass all args to Import __init__ function.
    """
    assert(args)
    feature = part.addFeature("Import")
    return Import(feature, *args)


class Import(Interface):
    """Interface class for Import feature.

    Import(feature) -> feature interface without initialization
    Import(feature, file_path) ->
        feature interface initialized from arguments:
        - file_path -- path to the imported file
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Import")

        self._file_path = self._feature.data().string("file_path")

        assert(self._file_path)

        if not args:
            return

        assert(len(args) == 1)
        self.setFilePath(args[0])

        self.execute()
        pass

    def setFilePath(self, file_path):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._file_path, file_path)
        pass


def exportToFile(part, *args):
    """Add an Export feature to the Part and return Export.

    Pass all args to Export __init__ function.
    """
    assert(args)
    feature = part.addFeature("Export")
    return Export(feature, *args)


class Export(Interface):
    """Interface class for Export feature.

    Export(feature) -> feature interface without initialization
    Export(feature, file_path, file_format, selection_list) ->
        feature interface initialized from arguments:
        - file_path -- path to the exported file
        - file_format -- format of to the exported file
        - selection_list -- objects to export
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Export")

        self._file_path = self._feature.data().string("file_path")
        self._file_format = self._feature.data().string("file_format")
        self._objects = self._feature.data().selectionList("selection_list")

        assert(self._file_path)
        assert(self._file_format)
        assert(self._objects)

        if not args:
            return

        assert(len(args) == 3)
        self.setFilePath(args[0])
        self.setFileFormat(args[1])
        self.setObjects(args[2])

        self.execute()
        pass

    def setFilePath(self, file_path):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._file_path, file_path)
        pass

    def setFileFormat(self, file_format):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._file_format, file_format)
        pass

    def setObjects(self, objects):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._objects, object)
        pass
