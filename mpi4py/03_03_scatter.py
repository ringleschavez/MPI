# source: https://pythonprogramming.net/scatter-gather-mpi-mpi4py-tutorial/
from mpi4py import MPI

comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()

if rank == 0:
    data = [(x+1)**x for x in range(size)]
    print('rank={} scattering={}'.format(rank, data))
else:
    data = None  # <--- IMPORTANT TO SET "None" into the variable

data = comm.scatter(data, root=0)
print('rank={},got={}'.format(rank, data))
