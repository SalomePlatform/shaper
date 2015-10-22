# Package exceptions

class ModelError(Exception):
    """Base class for exceptions in this package."""
    pass

class WrongNumberOfArguments(ModelError):
    """Exception raised when a wrong number of arguments is given."""
    pass

class FeatureInputInvalid(ModelError):
    """Exception raised if a feature input is invalid."""
    pass

    #Attributes:
        #expr -- input expression in which the error occurred
        #msg  -- explanation of the error
    #"""

    #def __init__(self, expr, msg):
        #self.expr = expr
        #self.msg = msg
