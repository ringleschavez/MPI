#!/usr/bin/env python
#
# based on: https://nyu-cds.github.io/python-mpi/05-collectives/
#           http://mvapich.cse.ohio-state.edu/benchmarks/
#           https://github.com/mpi4py/mpi4py/blob/master/demo/osu_bcast.py
# 	https://pythonprogramming.net/mpi-broadcast-tutorial-mpi4py/roadcast-tutorial-mpi4py/
#

"""
source:
https://mpi4py.readthedocs.io/en/stable/overview.html#collective-communications
In MPI for Python, the
    MPI.Comm.Bcast(), MPI.Comm.Scatter(), MPI.Comm.Gather(),
    MPI.Comm.Allgather(), and MPI.Comm.Alltoall() MPI.Comm.Alltoallw()
    methods provide support for collective communications of memory buffers.
"""
from mpi4py import MPI
import sys
import array

# from multiprocessing import shared_memory

BUFFER_SIZE = 80


"""
def set_rank_processor_name(mpi_rank=None, mpi_proc_name=None):
    a = mpi_proc_name
"""

if __name__ == '__main__':
    mpi_comm_world = MPI.COMM_WORLD
    mpi_size = mpi_comm_world.Get_size()
    mpi_rank = mpi_comm_world.Get_rank()
    mpi_status = MPI.Status()
    mpi_proc_name = MPI.Get_processor_name()

    sys.stdout.write('mpi_rank={},mpi_size={},processor={}\n'.format(
                mpi_rank, mpi_size, mpi_proc_name))

    mem_buffer = array.array('B', [0]) * BUFFER_SIZE  # allocating memory

    if mpi_rank == 0:
        # from: https://pythonprogramming.net/mpi-broadcast-tutorial-mpi4py/
        # byte assigment
        # mem_buffer = b'the.memory.buffer.from.rank.0'
        #
        # encoding string assigment
        # __ric__ mem_buffer = 'the.encoded.memory.buffer.from.rank.0'.encode('utf-8')
        mem_buffer = array.array('B')
        mem_buffer.fromstring('the.encoded.memory.buffer.from.rank.0')
        #
        # assigning a STRING produces the following:
        # TypeError: a bytes-like object is required, not 'str'
        # mem_buffer = 'the.memory.buffer.from.rank.0'

        size = len(mem_buffer)
        if size < BUFFER_SIZE:
            mem_buffer[size + 1:] = array.array('B', [0]) * (BUFFER_SIZE - size)
    #else:
        # already initialized mem_buffer = None
        #size = BUFFER_SIZE

    mpi_msg = [mem_buffer, BUFFER_SIZE, MPI.BYTE]
    print('before,rank={},size={},processor={},mpi_msg={}'.format(
                mpi_rank, mpi_size, mpi_proc_name, mpi_msg))
    ric_tmp = mpi_comm_world.Bcast(mpi_msg, root=0)
    # print('>after,rank={},size={},processor={},mpi_msg={}'.format(
    #           mpi_rank, mpi_size, mpi_proc_name, mpi_msg))
    if mpi_rank > 0:
        #
        print('>after,rank={},size={},processor={},MPI.Status.Get_count()={},\
                MPI.Status.Get_elements={}'
                .format(
                    mpi_rank, mpi_size, mpi_proc_name,
                    mpi_status.Get_count(MPI.BYTE),
                    mpi_status.Get_elements(MPI.BYTE)))

        # raw msg
        print('>after,rank={},size={},processor={},mpi_msg={}'.format(
                    mpi_rank, mpi_size, mpi_proc_name, mpi_msg))
        # the array
        mem_buffer_array = mpi_msg[0]
        print('>after,rank={},size={},processor={},mem_buffer_array={},len={}'.format(
                    mpi_rank, mpi_size, mpi_proc_name, mem_buffer_array,
                    len(mem_buffer_array)))

        # the string
        bstring_mem_buffer = mem_buffer_array.tostring()
        print('>after,rank={},size={},processor={},bstring_mem_buffer={}'.format(
                    mpi_rank, mpi_size, mpi_proc_name, bstring_mem_buffer))

        # decoded string
        decoded_mem_buffer = bstring_mem_buffer.decode()
        print('>after,rank={},size={},processor={},decoded_mem_buffer={}'.format(
                    mpi_rank, mpi_size, mpi_proc_name, decoded_mem_buffer))
