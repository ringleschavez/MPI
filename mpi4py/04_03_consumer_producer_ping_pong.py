from mpi4py import MPI
import array
import random


TAG_WORKER_STARTED = 1
TAG_MESSAGE_TRANSFER = 2

MAX_MSG_LENGTH = 20

mpi_comm = MPI.COMM_WORLD
mpi_status = MPI.Status()
mpi_rank = mpi_comm.Get_rank()
mpi_size = mpi_comm.Get_size()
mpi_proc_name = MPI.Get_processor_name()


# producer
if mpi_rank == 0:
    is_this_the_end = False

    n_of_workers = mpi_size - 1  # getting out rank 0
    workers_counter = 0

    int_to_be_sent = 0

    while True:
        is_worker_available = array.array('i', [0])
        # wait for some consumer/worker
        mpi_comm.Recv([is_worker_available, MPI.INT],
                      source=MPI.ANY_SOURCE,
                      tag=TAG_WORKER_STARTED,
                      status=mpi_status)

        print('0>source={},tag={}'.format(mpi_status.source,
                                          mpi_status.Get_tag()))

        if mpi_status.Get_tag() == TAG_WORKER_STARTED:
            workers_counter += 1
            int_to_be_sent = random.randrange(int_to_be_sent, (int_to_be_sent+workers_counter)*workers_counter)
            byte_array_to_be_sent = array.array('B', [0]*MAX_MSG_LENGTH)  # initializing 
            msg = array.array('B')
            msg.frombytes('rank=0,sent={}'.format(int_to_be_sent).encode())
            byte_array_to_be_sent[:len(msg)] = msg
            mpi_comm.Send([byte_array_to_be_sent, MAX_MSG_LENGTH, MPI.CHAR],
                          dest=mpi_status.source,  # sending to the recent worker
                          tag=TAG_MESSAGE_TRANSFER)

            print('0>worker={} should has gotten={}'.format(mpi_status.source,
                                                            int_to_be_sent))

        if workers_counter == n_of_workers:
            is_this_the_end = True
        elif workers_counter > n_of_workers:
            print('ERROR: More workers than expected')
            is_this_the_end = True
        else:
            is_this_the_end = False


        if is_this_the_end:
            break  # ending up producer

# consumer(s)
if mpi_rank > 0:
    print('{},{}'.format(mpi_rank, mpi_proc_name))
    flag_worker_available = array.array('i', [1])

    # informing rank==0 worker is ready
    mpi_comm.Send([flag_worker_available, MPI.INT],
                  dest=0,  # sending to the rank = 0
                  tag=TAG_WORKER_STARTED)

    byte_array_gotten = array.array('B', [0]*MAX_MSG_LENGTH)  # initializing 
    # waiting for the message from rank=0
    mpi_comm.Recv([byte_array_gotten, MAX_MSG_LENGTH, MPI.CHAR],
                  source=0,  # rank==0
                  tag=TAG_MESSAGE_TRANSFER,
                  status=mpi_status)

    print('rank={},processor={},got={}'.format(mpi_rank, 
                                               mpi_proc_name, 
                                               byte_array_gotten))

    print('rank={},got={}'.format(mpi_rank, byte_array_gotten.tobytes()))
    print('rank={},got={}'.format(mpi_rank, byte_array_gotten.tobytes().decode()))
