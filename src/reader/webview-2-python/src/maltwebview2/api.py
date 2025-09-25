############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 07/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/reader/webview-2-python/src/maltwebview2/api.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
import os
import ctypes

############################################################
class MaltProfileRequest:
    def __init__(self, file_path: str):
        self.need_load = True
        self.file_path = file_path

    def lazy_load(self) -> None:
        if self.need_load:
            self.load()
            self.need_load = False

    def load(self) -> None:
        # load & setup
        self.load_dll()
        self.define_return_types()

        # call init
        self.reader_handler: ctypes.c_void_p = self.reader.malt_reader_init(ctypes.c_char_p(self.file_path.encode()))
        print(f"HANDLE = {hex(self.reader_handler)}")

    def load_dll(self) -> None:
        # Load the shared library into ctypes
        libname = os.path.abspath("../../build-release/src/reader/libreader/libmalt-profile.so")
        self.reader = ctypes.CDLL(libname)

    def define_return_types(self) -> None:
        self.reader.malt_reader_init.restype = ctypes.c_void_p
        self.reader.malt_reader_get_summary.restype = ctypes.c_char_p
        self.reader.malt_reader_get_summary_v2.restype = ctypes.c_void_p
        self.reader.malt_reader_get_flat_profile.restype = ctypes.c_void_p

    def get_summary(self) -> str:
        self.lazy_load()
        str:ctypes.c_void_p = self.reader.malt_reader_get_summary(ctypes.c_void_p(self.reader_handler))
        if (str == 0):
            raise Exception("Error, get C++ execption and no response !")
        data = ctypes.cast(str, ctypes.c_char_p).value.decode("utf-8")
        #self.reader_handler.malt_reader_free_response(str)
        return data

    def get_summary_v2(self) -> str:
        self.lazy_load()
        str:ctypes.c_void_p = self.reader.malt_reader_get_summary_v2(ctypes.c_void_p(self.reader_handler))
        if (str == 0):
            raise Exception("Error, get C++ execption and no response !")
        data = ctypes.cast(str, ctypes.c_char_p).value.decode("utf-8")
        #self.reader.malt_reader_free_response(str)
        return data

    def get_flat_profile(self) -> str:
        self.lazy_load()
        str:ctypes.c_void_p = self.reader.malt_reader_get_flat_profile(ctypes.c_void_p(self.reader_handler))
        if (str == 0):
            raise Exception("Error, get C++ execption and no response !")
        data = ctypes.cast(str, ctypes.c_char_p).value.decode("utf-8")
        #self.reader.malt_reader_free_response(str)
        return data
