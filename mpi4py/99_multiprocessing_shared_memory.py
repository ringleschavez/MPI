#!/usr/bin/env python
#
# based on: https://nyu-cds.github.io/python-mpi/05-collectives/
#           http://mvapich.cse.ohio-state.edu/benchmarks/
#           https://github.com/mpi4py/mpi4py/blob/master/demo/osu_bcast.py
#
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
    # mpi_comm_world.Barrier()

    sys.stdout.write('mpi_rank={},mpi_size={},processor={}\n'.format(mpi_rank, mpi_size, mpi_proc_name))

    if mpi_rank == 0:
        shm = shared_memory.SharedMemory(create=True,
                                         size=(80 * mpi_size))
        shm_dict = {'name': shm.name}
        buf = array('B', [0]) * 80
        msg = [buf, 80, MPI.BYTE]

        # from: https://pythonprogramming.net/mpi-broadcast-tutorial-mpi4py/
        abc_data = {'a':1,'b':2,'c':3}
    else:
        shm = None
        shm_dict = None
        abc_data = None

    # mpi_comm_world.Barrier()
        """
    print('rank={},size={},processor={},shm.name={}'.format(mpi_rank, mpi_size, mpi_proc_name, shm_dict))
    mpi_comm_world.bcast(shm_dict, root=0)
    print('rank={},size={},processor={},shm.name={}'.format(mpi_rank, mpi_size, mpi_proc_name, shm_dict))
    """
    print('rank={},size={},processor={},abc_data={}'.format(mpi_rank, mpi_size, mpi_proc_name, abc_data))
    abc_data = mpi_comm_world.bcast(abc_data, root=0)
    print('rank={},size={},processor={},abc_data={}'.format(mpi_rank, mpi_size, mpi_proc_name, abc_data))

    if mpi_rank == 0:
        print(bytes(shm.buf[:80 * mpi_size]))
        shm.close()
        shm.unlink()
