# Source: http://pythonprogramming.net/sending-receiving-messages-nodes-dynamically/
"""
Now we're going to talk about dynamically sending and receiving messages. 
Maybe you are running in a heavily distributed network, 
with available node counts that change often.

Maybe you just have a large number of nodes, 
and you don't want to hand-code something for each tiny node. 
You'll need to create algorithms to choose nodes that can scale with your network.

Here's an example algorithm that will always send to the next node up, 
and wrap around to the beginning when we reach the largest node number.
"""

from mpi4py import MPI
#import numpy

mpi_comm = MPI.COMM_WORLD
mpi_rank=mpi_comm.rank
mpi_size=mpi_comm.size
mpi_proc_name=MPI.Get_processor_name()

shared=(mpi_rank+1)*5

"""
The variants MPI.Comm.send(), MPI.Comm.recv() and MPI.Comm.sendrecv() can communicate general Python objects.
"""
target_rank=(mpi_rank+1)%mpi_size
source_rank=(mpi_rank-1)%mpi_size
# mpi_comm.send(shared,dest=(mpi_rank+1)%mpi_size)
# data=mpi_comm.recv(source=(mpi_rank-1)%mpi_size)
mpi_comm.send(shared,dest=target_rank)
data=mpi_comm.recv(source=source_rank)

print('rank={},processor={},got={},from={}'.format(mpi_rank, mpi_proc_name, data, source_rank))


