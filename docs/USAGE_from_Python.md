# USAGE from Python
Through the Python interface, the Python program can act as a Worker in the CoToCoA framework.
When using CoToCoA framework, refer to [USAGE.md](USAGE.md "USAGE.md") for the setting of Requester and Coupler.

The usage of Python interface in the Python program is as follows:
1. Import CTCAWorker class from ctca4py.py file
    ```
    from ctca4py import CTCAWorker
    ```
2. Create CTCAWorker Instance
    ```
    ctca = CTCAWorker()
    ```
3. Call the initialize function of CoToCoA
    ```
    ctca.CTCAW_init(progid, procspercomm)
    ```
4. Make a main loop that repeats the following operations:
    - Deque request from the queue.
        ```
        fromrank, intparam, data = ctca.CTCAW_pollreq(intparamsnum, datanum, CTCAWorker.DAT_REAL8)
        ```
    - If the requester has quitted CoToCoA, quit from the main loop.
        ```
        if ctca.CTCAW_isfin()==1:
            break
        ```
    - Initialize the work.
    - Read data from the requester (if necessary).
    - Compute with the request.
    - Write-back data to the requester (if necessary).
    - Notify completion of the request.
        ```
        ctca.CTCAW_complete()
        ```