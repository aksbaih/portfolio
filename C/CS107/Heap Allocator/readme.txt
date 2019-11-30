File: readme.txt
Author: Akram Sbaih
----------------------

implicit
--------
I tried being as simple and straightforward with this design as possible so that I
get used to what a heap allocater is and what are the practical challenges it presents.
That was useful because I, unexpectedly, ended up using more than half my time on this
assignment debugging this design. It significantly helped me improve my skills with using
gdb (especially that I intentionally abstained from using printf and dumping the entire
heap).
When I started, I decided to store the pointer of the next block in the header instead of
the size of the current block. However, I changed my mind because it's more expensive to
subtract pointers to find sizes than it is to add sizes in order to find pointers. It's
also more consistent with the design described in lecture.

explicit
--------
I spent an hour designing the layout of this allocator, and then had to decide whether to
implement it or implement the design in lecture, and ended up making the wrong decision of
implementing it. Athough it has significant better utilization and instruction count, it took
three days of full time work (36 hours) to finish and I don't think the grade of this assignment
is worth all the effort.
This design features best-fit allocation, in-place reallocation, and left-right full coalescing.
It employs a binary heap tree (priority queue) data structure built into the headers (virtual heap)
to achieve best-fit in O(log2 N) and linear structure (physical heap) for O(1) coalescing and
in-place reallocation. I chose to use as much abstraction as possible by making many helper
functions (hoping that gcc optimizes them out, which it did), but abstraction is still not fully
achieved because C is not an object-oriented language. Therefore, I am sorry for any confusion
posed by many functions calling each other.
Utilization is over 80% for all tests except gcc (71%). Instruction count 447-679ins/req.

Tell us about your quarter in CS107!
-----------------------------------
I learned a whole lot! I just wish there was more connection in the assignments betweem me
and the instructor (as in CS106 IGs).
Thanks a lot!



