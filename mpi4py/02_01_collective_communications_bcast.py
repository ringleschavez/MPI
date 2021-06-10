#!/usr/bin/env python
#
# based on: https://nyu-cds.github.io/python-mpi/05-collectives/
#           http://mvapich.cse.ohio-state.edu/benchmarks/
#           https://github.com/mpi4py/mpi4py/blob/master/demo/osu_bcast.py
#
"""
source: https://mpi4py.readthedocs.io/en/stable/overview.html#collective-communications
The lower-case variants 
    MPI.Comm.bcast(), MPI.Comm.scatter(), MPI.Comm.gather(), 
    MPI.Comm.allgather() and MPI.Comm.alltoall() 
    can communicate general Python objects.
"""
from mpi4py import MPI
import sys
from array import array

from multiprocessing import shared_memory


def set_rank_processor_name(mpi_rank=None, mpi_proc_name=None):
    a = mpi_proc_name

if __name__ == '__main__':
    mpi_comm_world = MPI.COMM_WORLD
    mpi_size = mpi_comm_world.Get_size()
    mpi_rank = mpi_comm_world.Get_rank()
    mpi_proc_name = MPI.Get_processor_name()

    sys.stdout.write('mpi_rank={},mpi_size={},processor={}\n'.format(mpi_rank, mpi_size, mpi_proc_name))

    if mpi_rank == 0:
        # from: https://pythonprogramming.net/mpi-broadcast-tutorial-mpi4py/
        tmp_py_dict = {'a':1,'b':2,'c':3}
    else:
        tmp_py_dict = None

    print('before,rank={},size={},processor={},tmp_py_dict={}'.format(mpi_rank, mpi_size, mpi_proc_name, tmp_py_dict))
    tmp_py_dict = mpi_comm_world.bcast(tmp_py_dict, root=0)
    print('>after,rank={},size={},processor={},tmp_py_dict={}'.format(mpi_rank, mpi_size, mpi_proc_name, tmp_py_dict))
