# cotocoa
Code-To-Code Adapter (CoToCoA) is a framework to connect a requester program to multiple worker programs via a coupler program. In this framework, the requester submits requests of computations to the coupler. For each request, the coupler determines which program to ask for the computation, finds available worker processes, and sends the request to some of those processes. 
In transferring data from the requester to workers, there can be two cases:
  - Static case: the data to be transferred can be determined by the requester.
  - Dynamic case: the data to be transferred can be determined by the coupler or the workers.
To enable the framework to be used in both cases, four methods of transferring data from the requester to the workers are available:
  (a) Requester can pack short integer data in each request.
  (b) Requester also can attach an array with one of three types (integer/4byte real/8byte real) to the request.
  (c) Coupler can determine the data to transfer by using information in the request, reads the data from the requester, and sends it to the workers with the method (a) or (b).
  (d) Woker can determine the necessary data by using informaation in the request, and reads it from the requester.
In dynamic case, the requester needs to expose a region of its memory to the coupler and the workers, so that they can read data from the region.

Currently, CoToCoA supports Fortran and C. As the fundamental communication layer, at this point, it only supports MPI.
