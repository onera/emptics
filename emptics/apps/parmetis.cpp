#include <vector>
#include <string>
#include <iostream>
#include "stdio.h"
#include "mpi.h"

#include <metis.h>
#include <parmetis.h>

void printa(const std::string &name,
            const std::vector<int> &a) {
  printf("%s = ", name.c_str());
  for (size_t i=0; i<a.size(); ++i) {
    printf("%i ", a.data()[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  printf("Test (par)metis\n");

  MPI_Init(&argc, &argv);

  MPI_Comm comm = MPI_COMM_WORLD;
  int mpi_size, mpi_rank;
  MPI_Comm_size(comm, &mpi_size);
  MPI_Comm_rank(comm, &mpi_rank);
  if (mpi_size < 2) {
    printf("The (par)metis test should be launched with at least two processes\n");
    return 1;
  }


  /**
   * METIS test
   *
   * idx_t is metis int type, i.e. int32 or int64 depending on metis compilation flags
   */
  if (mpi_rank==0) {
    /**
     *    0         3
     *    +---------+
     *    | \       |
     *    |   \     |
     *    |     \   |
     *    |       \ |
     *    +---------+
     *    2         1
     *
     */

    {
      idx_t  n      = 4;
      idx_t  ncon   = 1;
      std::vector<idx_t> xadj   = {0    , 3    , 6  , 8  , 10};
      std::vector<idx_t> adjncy = {2,1,3, 0,2,3, 1,0, 0,1};
      std::vector<idx_t> vwgt   = {0,0,100,100};
      idx_t edgecut;
      std::vector<idx_t> part(n);

      printf("\n");
      printf("Test n°1: metis: no given weight\n");
      int status = METIS_PartGraphKway(&n, &ncon, xadj.data(), adjncy.data(), NULL, NULL, NULL, &mpi_size,
      NULL, NULL, NULL, &edgecut, part.data());

      printf("  > status = %i\n", status);
      printf("  > edgecut = %i\n", edgecut);
      printa("  > part", part);
      printf("  ref_result: part = [0,0,0,0]");
      printf("\t--> metis didn't cut any edge because we didn't give it any clue about balancing constaints\n");

      printf("\n");
      printf("Test n°2: metis: given weight\n");
      status = METIS_PartGraphKway(&n, &ncon, xadj.data(), adjncy.data(), vwgt.data(), NULL, NULL, &mpi_size,
      NULL, NULL, NULL, &edgecut, part.data());

      printf("  > status = %i\n", status);
      printf("  > edgecut = %i\n", edgecut);
      printa("  > part", part);
      printf("  ref_result: part = [1,1,1,0]");
      printf("\t--> metis tries to distribute vertices with huge weights into different partitions\n");
    }


    /**
     *   0         3
     *   +         +
     *   |         |
     *   |         |
     *   +         +
     *   2         1
     *
     */
    {
      printf("\n");
      printf("Test n°3: metis: non-connex graph\n");
      idx_t  n      = 4;
      idx_t  ncon   = 1;
      std::vector<idx_t> xadj   = {0, 1 ,2, 3, 4};
      std::vector<idx_t> adjncy = {2, 3, 0, 1};
      idx_t objval;
      std::vector<idx_t> part(n);
      printf("mpi_size = %i\n", mpi_size);

      int status = METIS_PartGraphKway(&n, &ncon, xadj.data(), adjncy.data(), NULL, NULL, NULL, &mpi_size,
        NULL, NULL, NULL, &objval, part.data());

      printf("  > status = %i\n", status);
      printf("  > objval = %i\n", objval);
      printa("  > part", part);
      printf("  ref_result: part = [1,0,1,0]");
      printf("\t--> metis can distribute easily because of non-connexity\n");
    }
  }




  /**
   * PARMETIS test
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
    idx_t  ncon    = 1;
    idx_t  wgtflag = 2; // weights on vertices only
    idx_t  numflag = 0; // numbering starts at 0
    std::vector<idx_t > xadjdist;
    std::vector<idx_t > adjncydist;
    std::vector<idx_t > vwgt;
    std::vector<real_t> tpwgt = {1.f/mpi_size, 1.f/mpi_size};
    std::vector<real_t> ubvec = {1.05f};
    std::vector<idx_t > vtxdist = {0, 2, 4};
    idx_t n = vtxdist[mpi_rank+1]-vtxdist[mpi_rank];
    idx_t options[] = {0};

    if (mpi_rank==0) {
      xadjdist   = {0    ,   3    , 6};
      adjncydist = {2,1,3,   0,2,3};
      vwgt       = {0,0};
    }
    else if (mpi_rank==1) {
      xadjdist   = {0  ,   2  , 4};
      adjncydist = {1,0,   0,1};
      vwgt       = {100,100};
    }


    if (mpi_rank==0) {
      printf("\n");
      printf("Test n°4: parmetis\n");
    }
    idx_t edgecut;
    std::vector<idx_t> partdist(n);
    idx_t status =  ParMETIS_V3_PartKway(vtxdist.data(), xadjdist.data(), adjncydist.data(), vwgt.data(), NULL,
                                        &wgtflag, &numflag, &ncon, &mpi_size, tpwgt.data(), ubvec.data(),
                                         options, &edgecut, partdist.data(), &comm);

    std::string prefix = "["+std::to_string(mpi_rank)+"]  > part";
    printf("[%d]  > status  = %i\n", mpi_rank, status);
    printf("[%d]  > edgecut = %i\n", mpi_rank, edgecut);
    printa(prefix, partdist);
    if (mpi_rank==0) {
      printf("     ref_result: part = [0,0, 0,1]");
      printf("\t--> same partitioning as test n°2 (but inverted)\n");
    }
  }
  MPI_Finalize();

  return 0;
}
