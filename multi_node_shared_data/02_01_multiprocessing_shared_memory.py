#
from enum import IntFlag#
import os
from multiprocessing import shared_memory
from multiprocessing import resource_tracker
import time

from mpi4py import MPI

# SHARED_DATA_PATH = './'
# SHARED_DATA_FILENAME = 'shared_data.mmap'

SHARED_DATA_SIZE = 8

MMAP_FILENAME_KEY = 'shared_memory_name'

g_shm_name = None
g_shm = None

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

if mpi_cw_rank == 0:
    '''
    mmap_path_filename = os.path.join(SHARED_DATA_PATH, SHARED_DATA_FILENAME)
    

    with open(mmap_path_filename, 'wb') as file_object:
        file_object.write(bytes(SHARED_DATA_SIZE))

    with open(mmap_path_filename, 'r+', encoding='utf8') as file_object:
        with mmap.mmap(file_object.fileno(),
                       length=0,
                       access=mmap.ACCESS_WRITE,
                       offset=0
                       ) as mmap_object:
            # just checking the content
            assert bytes(SHARED_DATA_SIZE) == mmap_object[:SHARED_DATA_SIZE]

            data_to_be_broadcasted = {MMAP_FILENAME_KEY: mmap_path_filename}

            # initializing the mmap file content
            for i_rank in range(mpi_cw_size):
                mmap_object[i_rank] = ProcessCommand.IDLE
    '''
    g_shm = shared_memory.SharedMemory(create=True, size=SHARED_DATA_SIZE)
    if g_shm:
        g_shm_name = g_shm.name
        data_to_be_broadcasted = {MMAP_FILENAME_KEY: g_shm_name}

        for i_rank in range(mpi_cw_size):
            g_shm.buf[i_rank] = ProcessCommand.IDLE
        # shm.close()
        # unlink() is not called here
    else:
        print('error creating SharedMemory')
else:
    data_to_be_broadcasted = None  # not main rank

# all processes wait for the mmap file creation
broadcasted_data = mpi_comm.bcast(data_to_be_broadcasted, root=0)

if mpi_cw_rank > 0:
    print('{}(rank={}/{}) got data={}'.format(mpi_proc_name,
                                              mpi_cw_rank,
                                              mpi_cw_size,
                                              broadcasted_data))

#  Sending commands to the processes
if mpi_cw_rank == 0:
    '''
    with open(mmap_path_filename, 'r+', encoding='utf8') as file_object:
        with mmap.mmap(file_object.fileno(),
                       length=0,
                       access=mmap.ACCESS_WRITE,
                       offset=0
                       ) as mmap_object:

            for i_rank in range(mpi_cw_size):
                mmap_object[i_rank] = ProcessCommand.STOP
    '''
    # shm = shared_memory.SharedMemory(g_shm_name)
    time.sleep(1/1000)  # allowing rank > 0  processes going to IDLE
    if g_shm:
        for i_rank in range(mpi_cw_size):
            g_shm.buf[i_rank] = ProcessCommand.STOP

        # checking how many processes have been ended
        while True:
            ended_counter = 1  # including rank=0
            for i_rank in range(mpi_cw_size):
                if g_shm.buf[i_rank] == ProcessCommand.ENDED:
                    ended_counter += 1
                    # __debugging__
                    # print('{}({}/{}): ended_counter={}'.format(mpi_proc_name,
                    #                                           mpi_cw_rank,
                    #                                           mpi_cw_size,
                    #                                           ended_counter))
                os.sched_yield()  # i_rank for

            if ended_counter == mpi_cw_size:
                break

            os.sched_yield()  # infinity loop

        # time.sleep(1)  # just in case
        # g_shm.close()
        # g_shm.unlink()

if mpi_cw_rank > 0:
    '''
    mmap_path_filename = broadcasted_data[MMAP_FILENAME_KEY]
    with open(mmap_path_filename, 'r+', encoding='utf8') as file_object:
        with mmap.mmap(file_object.fileno(),
                       length=0,
                       access=mmap.ACCESS_READ,
                       offset=0
                       ) as mmap_object:

    '''
    shm = shared_memory.SharedMemory(broadcasted_data[MMAP_FILENAME_KEY])
    if shm:
        while True:
            current_command = shm.buf[mpi_cw_rank]
            if current_command == ProcessCommand.IDLE:
                # __debugging__ print('{}(rank={}/{}) idle'.format(mpi_proc_name, mpi_cw_rank, mpi_cw_size))
                os.sched_yield()
                continue  # next loop
            elif current_command == ProcessCommand.STOP:
                print('{}(rank={}/{}) stopped'.format(mpi_proc_name, mpi_cw_rank, mpi_cw_size))
                break

            #  relinquishing CPU turn
            os.sched_yield()

        shm.buf[mpi_cw_rank] = ProcessCommand.ENDED
        print('{}(rank={}/{}) ended'.format(mpi_proc_name, mpi_cw_rank, mpi_cw_size))
        shm.close()
        #
        # msg364351 Author: Jeff Fischer (jfischer)     Date: 2020-03-16 18:48
        # https://bugs.python.org/msg364351
        #
        resource_tracker.unregister(shm._name, 'shared_memory')

        # os.sched_yield()
        # time.sleep(1)

# broadcasted_data = mpi_comm.bcast(data_to_be_broadcasted, root=0)

if mpi_cw_rank == 0:
    # time .sleep(1)
    g_shm.close()
    g_shm.unlink()

