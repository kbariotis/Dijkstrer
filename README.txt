ABOUT Dijkstrer:
    Dijkstrer is a C application which calculate the shortest path of a graph
    using Dijkstra's Algorithm[1]. Dijkstrer uses MPI library to implement 
    cluster functionality where every member of the cluster calculates each of a 
    node's childs. Dijkstrer will use each process to calculate each child of 
    every node. This means that when a node has fewer childs connected than the 
    number of processes, then some process( the highers ) will not be executed.
    Use number of processes equal to the maximum number of childs connected to a
    node.

    There are many ways of implementing graphs. 
    Dijkstrer uses two structures to represent nodes and acnes. Each node keeps 
    a list with references to every acne connected to him. It's acne has a node 
    member which points to the node connected. By this way we are able to 
    represent a path. By using recursion we are travelling through the shortest
    path and calculating the childs of each node.

    NODE0              
     acneslist          
      0.&node1------>NODE1  
        .             acneslist
        .              1.&node2-------
        .                  .          |
        .                  .          |
        .                             \/
      1.&node2---------------------->NODE2
                                      acneslist
                                         .
                                         .

    A non-clustered version can find at [2].


COMPILE & RUN:
    Dijkstrer requires MPICH2[3] library to be installed. Once you have 
    installed it you can compile Dijkstrer using the C compiler of your choise.

    Example of compiling under Windows using gcc:
        gcc -o dijkstrer.exe -I "C:\path\to\MPICH2\include" -L 
        "C:\path\to\MPICH2\lib" dijkstrer.c "C:\path\to\MPICH2\lib\mpi.lib

    Then, once you have your cluster app and running copy the executable to each
    of the cluster's members and run it. Dijkstrer will read the graph from a 
    file defined in the cmd or the default one which is dijkstra.dat.
    
    Files content should be in the format:
    
        n N
        startNode endNode distance
        startNode endNode distance
        .
        .
        .
        startNode endNode distance

    where n is the total number of Nodes and N is the total number of Acnes.

    The file must also be copied in every cluster's member in the same path.
    
    Example of running Dijkstrer under Windows using MPICH2's mpiexec:
        mpiexec -hosts 2 <host1> 1 <host2> 2 c:\dijkstrer.exe c:\dijkstra.dat
    
    Don't forget to copy the executables and the data file.
    
    Each of your cluster's member will create a process which will calculate a
    child in every node.
    

ABOUT THE AUTHOR:
    At the present:
        Student at Department Of Informatics at Alexander Technical & 
        Educational Institute of Thessaloniki, Hellas
        
    Contact me at : konmpar at gmail dot com   
    *Observations, proposals, greetings are always accepted.
    
REFERENCES:
    [1]: http://en.wikipedia.org/wiki/Dijkstra's_algorithm
    [2]: http://snipplr.com/view/60663/dijkstras-shortest-path-algorithm/
    [3]: http://www.mcs.anl.gov/research/projects/mpich2/
    
Dijkstrer (C) 2011 
