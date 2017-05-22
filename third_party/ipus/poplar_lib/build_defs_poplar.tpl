# Macros for supporting Poplar library

def poplar_available():
    """Returns true because Poplar library was configured
    """
    return True

def poplar_lib_directory():
    """Returns the full path to the Poplar libraries directory
    """
    return "POPLAR_LIB_DIRECTORY"
