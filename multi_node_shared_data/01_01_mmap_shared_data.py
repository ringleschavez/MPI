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

class Command(IntFlag):
    IDLE = 0 
    START = 1
    STOP = 2
    DO_THE_NEEDFUL = 3


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

            data_to_be_broadcasted = {'mmap_filename': mmap_path_filename}

            # initializating the mmap file content
            for i in range(mpi_cw_size):
                
else:
    data_to_be_broadcasted = None  # not main rank

    
# all processes wait for the mmap file creation
broadcasted_data = mpi_comm.bcast(data_to_be_broadcasted, root=0)

if mpi_cw_rank > 0:
    print('{}(rank={}/{}) got data={}'.format(mpi_proc_name,
                                              mpi_cw_rank,
                                              mpi_cw_size,
                                              broadcasted_data))


if 
