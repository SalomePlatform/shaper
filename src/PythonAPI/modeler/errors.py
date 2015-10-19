# Package exceptions

class ModelerError(Exception):
    """Base class for exceptions in this package."""
    pass

class WrongNumberOfArguments(ModelerError):
    """Exception raised when a wrong number of arguments is given."""
    pass

    #Attributes:
        #expr -- input expression in which the error occurred
        #msg  -- explanation of the error
    #"""

    #def __init__(self, expr, msg):
        #self.expr = expr
        #self.msg = msg
