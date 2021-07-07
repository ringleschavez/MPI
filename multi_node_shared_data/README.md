### Buffer-like communication methods (upper-case):
    Send(), Recv(), Bcast(), Scatter(), Gather().

### Python objects communication methods (lower-case):
    send(), recv(), bcast(), scatter(), gather().


### MPI info sources:

https://research.computing.yale.edu/sites/default/files/files/mpi4py.pdf

https://mpi4py.readthedocs.io/en/latest/tutorial.html

https://info.gwdg.de/wiki/doku.php?id=wiki:hpc:mpi4py

https://courses.cs.ut.ee/MTAT.08.020/2019_spring/uploads/Main/MPI_and_mpi4py.pdf

## Approaches 
### i. mmap
#### info sources:
https://realpython.com/python-mmap/

https://bip.weizmann.ac.il/course/python/PyMOTW/PyMOTW/docs/mmap/index.html

https://pymotw.com/3/mmap/index.html

### ii. multiprocessing.shared_memory:
#### NOTE: Only works for ONE NODE, and requires the usage of:
`resource_tracker.unregister()` 
#### IMPORTANT: https://bugs.python.org/msg364351

### iii. MPI RMA
`MPI_Put` and `MPI_Get` are called **Remote Memory Access (RMA)** operations.

info source:

https://mpi4py.readthedocs.io/en/stable/overview.html#one-sided-communications

https://fs.hlrs.de/projects/par/mooc/one-sided_mooc.pdf

https://www.rookiehpc.com/mpi/docs/mpi_win_create.php

https://portal.tacc.utexas.edu/documents/13601/1102030/4_mpi4py.pdf/f43b984e-4043-44b3-8225-c3ce03ecb93b

https://github.com/ycrc/parallel_python

https://enccs.github.io/intermediate-mpi/one-sided-routines/

https://cmse822.github.io/assets/EijkhoutParallelProgramming.pdf


