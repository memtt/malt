# ctypes_test.py
import ctypes
import pathlib

def main():
    # Load the shared library into ctypes
    libname = pathlib.Path().absolute() / "libcmult.so"
    c_lib = ctypes.CDLL(libname)

    x, y = 6, 2.3
    # ctypes_test.py
    answer = c_lib.cmult(x, ctypes.c_float(y))
    print(f"    In Python: int: {x} float {y:.1f} return val {answer:.1f}")


main()
