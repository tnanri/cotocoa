CoToCoA (Code To Code Adapter)
=====

CoToCoA is a framework to enable coupled computations, such as multi-scale simulations. Instead of using intermediate files, it enables direct communication among multiple programs.
Compared with similar frameworks, CoToCoA has the following advantages:
- One-directional flow
- Minimul modification to existing codes
- Asynchronous execution
- One-sided data transfer

In this framework, the requester submits requests of computations to the coupler. For each request, the coupler determines which worker program to ask for the computation, then enqueues it with the program ID. The worker with that ID retrieves the request from the queue and executes it.

Currently, CoToCoA supports Fortran and C. As the fundamental communication layer, it relies on the MPI library.

Developers of CoToCoA:
- Takeshi Nanri (Kyushu Univ., Japan)
- Yuto Katoh (Tohoku Univ., Japan)
- Keiichiro Fukazawa (Kyoto Univ., Japan)
- Yohei Miyake (Kobe Univ., Japan)
- Kazuya Nakazawa (Kobe Univ., Japan)
- Jingde Zhow (Kyoto Univ., Japan)
- Youhei Sunada (Kobe Univ., Japan)
- Haichao Zhao (Kyoto Univ., Japan)
