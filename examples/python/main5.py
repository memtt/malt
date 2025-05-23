import numpy as np

def load_1GB_of_data():
    return np.ones((2 ** 30), dtype=np.uint8)

def process_data():
    data = load_1GB_of_data()
    return modify2(modify1(data))

def modify1(data):
    return data * 2

def modify2(data):
    return data + 10

process_data()
