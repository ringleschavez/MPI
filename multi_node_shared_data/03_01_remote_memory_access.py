#
# sources:
#   https://buildmedia.readthedocs.org/media/pdf/mpi4py/latest/mpi4py.pdf
#   https://fs.hlrs.de/projects/par/mooc/one-sided_mooc.pdf
#

from enum import IntFlag  #
import os
# from multiprocessing import shared_memory
# from multiprocessing import resource_tracker
import time

from mpi4py import MPI
import numpy as np


class ProcessCommand(IntFlag):
    DO_THE_NEEDFUL = 65
    ENDED = 70
    IDLE = 75
    START = 80
    STOP = 85


mpi_cw_size = MPI.COMM_WORLD.Get_size()
mpi_cw_rank = MPI.COMM_WORLD.Get_rank()
mpi_proc_name = MPI.Get_processor_name()

mpi_comm = MPI.COMM_WORLD

print('{}(rank={}/{})'.format(mpi_proc_name, mpi_cw_rank, mpi_cw_size))

np_buffer = np.zeros(mpi_cw_size, dtype=np.uint8)

if mpi_cw_rank == 0:
    shared_win = MPI.Win.Create(np_buffer, comm=mpi_comm)

    # setting ranks > 0 on IDLE state
    shared_win.Fence()
    for i_rank in range(1, mpi_cw_size):
        np_buffer[i_rank] = ProcessCommand.IDLE
    shared_win.Fence()

    # poison pill to ranks > 0
    shared_win.Fence()
    for i_rank in range(1, mpi_cw_size):
        np_buffer[i_rank] = ProcessCommand.STOP
    shared_win.Fence()

    while True:
        ended_counter = 1
        mpi_comm.Barrier()
        shared_win.Lock(rank=0)
        shared_win.Get(np_buffer, target_rank=0)
        shared_win.Unlock(rank=0)
        for i_rank in range(1, mpi_cw_size):
            if np_buffer[i_rank] == ProcessCommand.ENDED:
                ended_counter += 1

            os.sched_yield()  # used in for i_rank range

        if ended_counter == mpi_cw_size:
            break

        os.sched_yield()  # used in infinity loop


else:
    shared_win = MPI.Win.Create(None, comm=mpi_comm)
    # receiving_buffer = np.zeros(mpi_cw_size, dtype=np.uint8)
    local_buffer = np.zeros(1, dtype=np.uint8)  # 1 = only the slot that belongs to it

if mpi_cw_rank > 0:
    '''
        source: https://mpi.deino.net/mpi_functions/MPI_Win_fence.html
    MPI_MODE_NOSTORE
        the local window was not updated by local stores 
        (or local get or receive calls) since last synchronization. 
    MPI_MODE_NOPUT
        the local window will not be updated by put or accumulate calls after the fence call, 
        until the ensuing (fence) synchronization. 
    MPI_MODE_NOPRECEDE
        the fence does not complete any sequence of locally issued RMA calls. 
        If this assertion is given by any process in the window group, 
        then it must be given by all processes in the group. 
    MPI_MODE_NOSUCCEED
        the fence does not start any sequence of locally issued RMA calls. 
        If the assertion is given by any process in the window group,
        then it must be given by all processes in the group.
    '''

    '''
        (offset, length, datatype)
            offset = rank (referring to the slot which belongs to the rank)
            length = 1 (taking only one slot)
            datatype = Char
    '''
    target = (mpi_cw_rank, 1, MPI.UNSIGNED_CHAR)  # getting only the commands what belongs to the rank

    while True:
        shared_win.Fence(assertion=MPI.MODE_NOSTORE | MPI.MODE_NOPUT)
        shared_win.Get([local_buffer, MPI.UNSIGNED_CHAR], target_rank=0, target=target)
        shared_win.Fence(assertion=MPI.MODE_NOSTORE | MPI.MODE_NOPUT)

        if local_buffer[0] == ProcessCommand.IDLE:
            os.sched_yield()
            continue
        elif local_buffer[0] == ProcessCommand.STOP:
            print('{}(rank={}/{}) stopped'.format(mpi_proc_name, mpi_cw_rank, mpi_cw_size))
            break

        os.sched_yield()

    local_buffer[0] = ProcessCommand.ENDED
    shared_win.Lock(rank=0, lock_type=MPI.LOCK_EXCLUSIVE, assertion=0)
    shared_win.Put([local_buffer, MPI.UNSIGNED_CHAR], target_rank=0, target=target)
    shared_win.Unlock(rank=0)
    mpi_comm.Barrier()
    print('{}(rank={}/{}) got buffer={}'.format(mpi_proc_name,
                                                mpi_cw_rank,
                                                mpi_cw_size,
                                                local_buffer))

if mpi_cw_rank == 0:
    print('{}({}/{} got buffer={}'.format(mpi_proc_name, mpi_cw_rank, mpi_cw_rank, np_buffer))

shared_win.Free()
