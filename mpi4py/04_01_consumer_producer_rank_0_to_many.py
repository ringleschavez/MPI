# Source: 
#	https://pythonprogramming.net/basic-mpi4py-script-getting-node-rank/
#   https://pythonprogramming.net/sending-receiving-data-messages-mpi4py/   

from mpi4py import MPI
import random

mpi_comm = MPI.COMM_WORLD
mpi_rank = mpi_comm.rank
mpi_size = mpi_comm.size
mpi_proc_name = MPI.Get_processor_name()

print('rank={},size={},processor={}'.format(mpi_rank, mpi_size, mpi_proc_name))

if mpi_rank == 0:
    print('0 on {}'.format(mpi_proc_name))

    lowest_value = 0
    for i_rank in range(1, mpi_size):
        data_to_be_sent = random.uniform(lowest_value, i_rank)
        lowest_value = i_rank
        mpi_comm.send(data_to_be_sent, dest=i_rank)
        print('0 sent {} to rank {}'.format(data_to_be_sent, i_rank))

if mpi_rank > 0:
    print('rank={} on {}'.format(mpi_rank, mpi_proc_name))
    recv_data  = mpi_comm.recv(source=0)
    print('rank={} got {}'.format(mpi_rank, recv_data))

