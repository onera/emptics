#include <vector>
#include <string>
#include <iostream>
#include "stdio.h"
#include "mpi.h"

#include <ptscotch.h>

void printa(const std::string             &name,
            const std::vector<SCOTCH_Num> &a) {
  printf("%s = ", name.c_str());
  for (size_t i=0; i<a.size(); ++i) {
    printf("%i ", a.data()[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  printf("Test ptscotch\n");

  MPI_Init(&argc, &argv);

  MPI_Comm comm = MPI_COMM_WORLD;
  int mpi_size, mpi_rank;
  MPI_Comm_size(comm, &mpi_size);
  MPI_Comm_rank(comm, &mpi_rank);
  if (mpi_size < 2) {
    printf("The ptscotch test should be launched with at least two processes\n");
    return 1;
  }


  /**
   * PTSCOTCH test
   */
  {
    /**
     *    0 (rk 0)   3 (rk 1)
     *    +----------+
     *    |  \       |
     *    |    \     |
     *    |      \   |
     *    |        \ |
     *    +----------+
     *    2 (rk 1)   1 (rk 0)
     *
     */


    int dn_vtx = 2;
    std::vector<SCOTCH_Num> dgraph_idx;
    std::vector<SCOTCH_Num> dgraph;
    std::vector<SCOTCH_Num> weight;
    std::vector<SCOTCH_Num> part(dn_vtx);
    if (mpi_rank==0) {
      dgraph_idx = {0    ,   3    , 6};
      dgraph     = {2,1,3,   0,2,3};
      weight     = {0,0};
    }
    else if (mpi_rank==1) {
      dgraph_idx = {0  ,   2  , 4};
      dgraph     = {1,0,   0,1};
      weight     = {100,100};
    }

    SCOTCH_Dgraph graph;
    int ierr = SCOTCH_dgraphInit(&graph, comm);

    SCOTCH_Num _baseval = 0;
    SCOTCH_Num _vertlocnbr = dn_vtx;
    SCOTCH_Num _vertlocmax = dn_vtx;

    SCOTCH_Num *_vertloctab = dgraph_idx.data();
    SCOTCH_Num *_vendloctab = dgraph_idx.data() + 1;
    SCOTCH_Num *_edgeloctab = dgraph.data();

    SCOTCH_Num *_vlblloctab = NULL;
    SCOTCH_Num _edgelocnbr = dgraph_idx.data()[dn_vtx];
    SCOTCH_Num _edgelocsiz = dgraph_idx.data()[dn_vtx];
    SCOTCH_Num *_edgegsttab = NULL;
    SCOTCH_Num *_edloloctab;

    SCOTCH_Num *_veloloctab = weight.data();
    SCOTCH_Num *_part       = part.data();


    ierr = SCOTCH_dgraphBuild(&graph,
                              _baseval,
                              _vertlocnbr,
                              _vertlocmax,
                              _vertloctab,
                              _vendloctab,
                              _veloloctab,
                              _vlblloctab,
                              _edgelocnbr,
                              _edgelocsiz,
                              _edgeloctab,
                              _edgegsttab,
                              _edloloctab);


    /* Checks graph */

    ierr = SCOTCH_dgraphCheck(&graph);

    if (ierr) {
      printf("PPART error : Error in PT-Scotch graph check\n");
      exit(1);
    }

    /* Partitioning strategy : */
    SCOTCH_Strat strat;
    SCOTCH_stratInit(&strat);

    ierr = SCOTCH_dgraphPart(&graph,
                             mpi_size,  /* Nombre de partitions demande */
                             &strat,
                             _part);    /* parts[i] donne le numero */

    std::string prefix = "["+std::to_string(mpi_rank)+"]  > part";
    printf("[%d]  > ierr = %i\n", mpi_rank, ierr);
    printa(prefix, part);

    if (mpi_rank==0) {
      printf("     ref_result: part = [0,0, 0,1]");
      printf("\t--> same partitioning as Parmetis test n°4\n");
    }
    SCOTCH_stratExit(&strat);
    SCOTCH_dgraphExit(&graph);

  }
  MPI_Finalize();

  return 0;
}
