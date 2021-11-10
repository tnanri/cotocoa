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
- Takeshi Nanri (Kyushu University, Japan)
- Yuto Katoh (Tohoku University, Japan)
- Keiichiro Fukazawa (Kyoto University, Japan)
- Yohei Miyake (Kobe University, Japan)
- Kazuya Nakazawa (Kobe University, Japan)
- Jingde Zhow (Kyoto University, Japan)
- Youhei Sunada (Kobe University, Japan)
