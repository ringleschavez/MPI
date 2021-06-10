#
# source: https://stackoverflow.com/questions/49968647/mpi4py-comm-bcast-does-not-work
#
# from __future__ import division, print_function
from mpi4py import MPI
import numpy as np

comm = MPI.COMM_WORLD
nproc = comm.Get_size()
rank = comm.Get_rank()


scal = None
mat = np.empty([3,3], dtype='d')

arr = np.empty(5, dtype='d')
result = np.empty(5, dtype='d')


if rank==0:
    scal = 55.0
    mat[:] = np.array([[1,2,3],[4,5,6],[7,8,9]])

    arr = np.ones(5)
    result = 2*arr

# Bcast -> memory buffer, uses MPI data types
print("before,Rank: ", rank, ". Array is:", result)
comm.Bcast([ result , MPI.DOUBLE], root=0)  # check, the memory is populated by reference
print(" after,Rank: ", rank, ". Array is:", result)

# bcast (lower case) -> python object
print("before,Rank: ", rank, ". Scalar is:", scal)
scal = comm.bcast(scal, root=0)  # check, the object is populated from the return value
print(" after,Rank: ", rank, ". Scalar is:", scal)

# Bcast -> memory buffer, uses MPI data types
print("before,Rank: ", rank, ". Matrix is:", mat)
comm.Bcast([ mat , MPI.DOUBLE], root=0)
print(" after,Rank: ", rank, ". Matrix is:", mat)

