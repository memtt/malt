import os
import json
import ctypes
import zmq

class MaltProfileRequest:
    def __init__(self, socket_path: str = "ipc:///tmp/socket"):
        context = zmq.Context()

        #  Socket to talk to server
        print("Connecting to hello world server…")
        self.socket = context.socket(zmq.REQ)
        self.socket.connect(socket_path)

    def make_request(self, operation: str, **kwargs):
        request = kwargs.copy()
        request["operation"] = operation

        print("Sending request %s …" % request)
        self.socket.send(json.dumps(request).encode())

        #  Get the reply.
        message = self.socket.recv()
        print("Received reply %s" % (request))
        return message

    def get_summary(self) -> str:
        return self.make_request("getSummary")

    def get_summary_v2(self) -> str:
        return self.make_request("getSummaryV2")

    def get_flat_profile(self) -> str:
        return self.make_request("getFlatFunctionProfile", own=True, total=True)

    def getFilterdStacksOnFileLine(self, file: str, line: int) -> str:
        return self.make_request("getFilterdStacksOnFileLine", file=file, line=line)

    def getFilterdStacksOnSymbol(self, func: str) -> str:
        return self.make_request("getFilterdStacksOnSymbol", func = func)

    def get_call_stack_next_level(self, parentStackId: int, parentStackLevel: int, filter: dict = {'function': '', 'file': '', 'line': -1}) -> str:
        return self.make_request("getCallStackNextLevel", parentStackId = parentStackId, parentStackLevel = parentStackLevel, filter = filter)

    def getFileLinesFlatProfile(self, file: str, total: bool) -> str:
        return self.make_request("getFileLinesFlatProfile", file = file, total = total)

    def getTimedValues(self) -> str:
        return self.make_request("getTimedValues")

    def getStacksMem(self) -> str:
        return self.make_request("getStacksMem")

    def getStackInfoOnFunction(self, thread_id: int) -> str:
        return self.make_request("getStackInfoOnFunction", thread_id = thread_id)

    def wait_ready(self) -> str:
        return self.make_request("waitReady")

    def stop(self) -> str:
        return self.make_request("stop")