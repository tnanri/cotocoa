Overview of CoToCoA
======

CoToCoA is a framework to enable coupled computations, such as multi-scale simulations. Instead of using intermediate files, it enables direct communication among multiple programs.
Compared with similar frameworks, CoToCoA has the following advantages:
- One-directional flow
- Minimul modification to existing codes
- Asynchronous execution
- One-sided data transfer

CoToCoA assumes that the flows of coupled computations are one-directional, originated from one type of program and targeted to another type of program. Those two types of programs are called the "requester" and the "worker", respectively. The requester sends requests which will be executed by one of the workers. CoToCoA allows one requester program with multiple worker programs. The requester sends requests that will be handled by one of the workers.

To ease the burden of constructing programs to enable coupled computations, CoToCoA is designed to minimize the modifications to the existing programs to connect each other. Therefore, it uses another program called the "coupler" to be placed between the requester and the workers. 
Fundamentaly, the coupler is responsible for:
- Accepting requests sent from the requester.
- Choosing an appropriate worker program to handle the request.
- Enqueue the request to the worker with the ID of the program.

Basically, request flows of CoToCoA are asynchronous. The requester does not have to wait for the completion of each request. If the requester must wait for the completion of the specific request, the requester can retrieve a handle from the send-request routine, so that it can use it in the wait routine later.

CoToCoA allows one-sided communication by allowing the requester to expose some of its memory area.
Such area can be directly read or written by the coupler or the worker.
In addition to that, a request of CoToCoA includes an array of integer parameters that are used to pass minimal information to control the execution in the worker program. 
Also, CoToCoA allows one array to be attached to each of the requests. 
Therefore, for example, the following methods can be used to transfer data between the requester and the workers:
- The requester attaches one array to the request which is forwarded to the worker via the coupler.
- The coupler reads some data from the exposed area of the requester which is used to create an array that will be attached to the request to the worker.
- The worker reads some data from the exposed area of the requester.
- The worker writes some data to the exposed area of the requester as feedbacks.

CoToCoA runs three types of programs, the requester, the coupler and the workers, by using MPMD (Multiple Programs Multiple Data) facility of MPI.
The processes are divided into subcommunicators according to their roles.
The number of processes of each subcommunicators are controlled by the MPMD options of mpirun, and the number of processes for each of the worker programs specified by the worker initialization routine of CoToCoA.

