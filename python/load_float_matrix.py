import numpy as np
import struct


def load_float_matrix(fname):
    """
    Load a numpy matrix in a binary format:
    <uint32:numRows><uint32:numCols><float32:data[]>
    where data is stored in row-major order
    """
    with open(fname, 'rb') as fd:
        num_rows = struct.unpack('=I', fd.read(4))[0]
        num_cols = struct.unpack('=I', fd.read(4))[0]
        numpy_matrix = np.zeros((num_rows, num_cols), dtype=np.float32)
        for i in range(numpy_matrix.shape[0]):
            for j in range(numpy_matrix.shape[1]):
                numpy_matrix[i, j] = struct.unpack('=f', fd.read(4))[0]
    return numpy_matrix
