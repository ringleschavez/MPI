from mpi4py import MPI

comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()

if rank == 0:
    data = [(x+1)**x for x in range(size)]
    print('rank={},scattering={}'.format(rank, data))
else:
    data = None

data = comm.scatter(data, root=0)
print('rank={},got={}'.format(rank, data))
data += 1
print('rank={},produce={}'.format(rank, data))

newData = comm.gather(data, root=0)

if rank == 0:
    print('rank={},gathered={}'.format(rank, newData))

