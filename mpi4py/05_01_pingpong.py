# source: https://github.com/mikaem/mpi-examples/blob/master/pingpong.py

from mpi4py import MPI
comm = MPI.COMM_WORLD
assert comm.Get_size() == 2

rank = comm.Get_rank()
count = 0
while count < 5:    
    if rank == count%2:
        count += 1
        target_rank=(rank+1)%2
        comm.send(count, dest=target_rank)
        print('rank={},count={},sent_it_to_rank={}'.format(rank, count, target_rank))

    elif rank == (count+1)%2:
        count = comm.recv(source=(rank+1)%2)
    comm.barrier()

if rank == 1: print("rank=1, done,gone!")
