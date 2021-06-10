# source: https://pythonprogramming.net/tagging-messages-mpi-multiple-messages/

from mpi4py import MPI
"""
Tagging messages is basically a requirement, 
unless maybe you treat a node itself as a variable name, 
and that node only produces one element.
The idea of tagging is that you can actually put a label to the data that you're sending,
otherwise there's really no way to differentiate between one bit of "data" and another.

Tagging allows us to control data flow, and make sure correct data gets stored where we expect. 

As you can see, we use the tag parameter in both
"""
mpi_comm = MPI.COMM_WORLD
mpi_rank = mpi_comm.rank
mpi_size = mpi_comm.size
mpi_proc_name = MPI.Get_processor_name()

if mpi_rank == 0:
    dict_to_be_sent_01 = {'d1':55,'d2':42}
    mpi_comm.send(dict_to_be_sent_01, dest=1, tag=1)

    dict_to_be_sent_02 = {'d3':25,'d4':22}
    mpi_comm.send(dict_to_be_sent_02, dest=1, tag=2)

if mpi_rank > 0:
    receive = mpi_comm.recv(source=0, tag=1)
    print(receive)

    receive2 = mpi_comm.recv(source=0, tag=2)
    print(receive2)
