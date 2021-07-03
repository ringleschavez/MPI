#
# source: https://realpython.com/python-mmap/
#
import os
import mmap
from enum import IntFlag
from mpi4py import MPI

SHARED_DATA_PATH = './'
SHARED_DATA_FILENAME = 'shared_data.mmap'

SHARED_DATA_SIZE = 8

MMAP_FILENAME_KEY = 'mmap_filename'

mmap_path_filename = None


class ProcessCommand(IntFlag):
    IDLE = 65
    START = 66
    STOP = 67
    DO_THE_NEEDFUL = 68


mpi_cw_size = MPI.COMM_WORLD.Get_size()
mpi_cw_rank = MPI.COMM_WORLD.Get_rank()
mpi_proc_name = MPI.Get_processor_name()

mpi_comm = MPI.COMM_WORLD

print('{}(rank={}/{})'.format(mpi_proc_name, mpi_cw_rank, mpi_cw_size))

if mpi_cw_rank == 0:
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
    with open(mmap_path_filename, 'r+', encoding='utf8') as file_object:
        with mmap.mmap(file_object.fileno(),
                       length=0,
                       access=mmap.ACCESS_WRITE,
                       offset=0
                       ) as mmap_object:

            for i_rank in range(mpi_cw_size):
                mmap_object[i_rank] = ProcessCommand.STOP

if mpi_cw_rank > 0:
    mmap_path_filename = broadcasted_data[MMAP_FILENAME_KEY]
    with open(mmap_path_filename, 'r+', encoding='utf8') as file_object:
        with mmap.mmap(file_object.fileno(),
                       length=0,
                       access=mmap.ACCESS_READ,
                       offset=0
                       ) as mmap_object:

            while True:
                current_command = mmap_object[mpi_cw_rank]
                if current_command == ProcessCommand.STOP:
                    print('{}(rank={}/{}) stopped'.format(mpi_proc_name, mpi_cw_rank, mpi_cw_size))
                    break

                #  relinquishing CPU turn
                os.sched_yield()

broadcasted_data = mpi_comm.bcast(data_to_be_broadcasted, root=0)
print('{}(rank={}/{}) ended'.format(mpi_proc_name, mpi_cw_rank, mpi_cw_size))
