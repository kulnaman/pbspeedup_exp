/* -*- mode: C; tab-width: 2; indent-tabs-mode: nil; fill-column: 79; coding: iso-latin-1-unix -*- */
/*
  hpcc.c
*/

#include <hpcc.h>
#include <ctype.h>
#include <string.h>

void parse_arguments(int argc, char *argv[], HPCC_Params *params) {
  if (argc == 1) /* run hpcc as the default way */
    return;
  else {
    /* disable all benchmarks */
    params->RunMPIRandomAccess = 0;
    params->RunStarRandomAccess = 0;
    params->RunSingleRandomAccess = 0;
    params->RunMPIRandomAccess_LCG = 0;
    params->RunStarRandomAccess_LCG = 0;
    params->RunSingleRandomAccess_LCG = 0;
    params->RunPTRANS = 0;
    params->RunStarDGEMM = 0;
    params->RunSingleDGEMM = 0;
    params->RunStarStream = 0;
    params->RunSingleStream = 0;
    params->RunMPIFFT = 0;
    params->RunStarFFT = 0;
    params->RunSingleFFT = 0;
    params->RunLatencyBandwidth = 0;
    params->RunHPL = 0;
    /* turn on the benchmarks shown in arguments */
    for (int i=1; i<argc; i++) {
      if (strcmp(argv[i], "MPIRandomAccess") == 0)
        params->RunMPIRandomAccess = 1;
      else if (strcmp(argv[i], "StarRandomAccess") == 0) 
        params->RunStarRandomAccess = 1;
      else if (strcmp(argv[i], "SingleRandomAccess") == 0)
        params->RunSingleRandomAccess = 1;
      else if (strcmp(argv[i], "MPIRandomAccess_LCG") == 0)
        params->RunMPIRandomAccess_LCG = 1;
      else if (strcmp(argv[i], "StarRandomAccess_LCG") == 0)
        params->RunStarRandomAccess_LCG = 1;
      else if (strcmp(argv[i], "SingleRandomAccess_LCG") == 0)
        params->RunSingleRandomAccess_LCG = 1;
      else if (strcmp(argv[i], "PTRANS") == 0)
        params->RunPTRANS = 1;
      else if (strcmp(argv[i], "StarDGEMM") == 0)
        params->RunStarDGEMM = 1;
      else if (strcmp(argv[i], "SingleDGEMM") == 0)
        params->RunSingleDGEMM = 1;
      else if (strcmp(argv[i], "StarStream") == 0)
        params->RunStarStream = 1;
      else if (strcmp(argv[i], "SingleStream") == 0)
        params->RunSingleStream = 1;
      else if (strcmp(argv[i], "MPIFFT") == 0)
        params->RunMPIFFT = 1;
      else if (strcmp(argv[i], "StarFFT") == 0)
        params->RunStarFFT = 1;
      else if (strcmp(argv[i], "SingleFFT") == 0)
        params->RunSingleFFT = 1;
      else if (strcmp(argv[i], "LatencyBandwidth") == 0)
        params->RunLatencyBandwidth = 1;
      else if (strcmp(argv[i], "HPL") == 0) 
        params->RunHPL = 1;
      else /* do nothing */
        ;
    }
  }
}

int
main(int argc, char *argv[]) {
  int myRank, commSize;
  char *outFname;
  FILE *outputFile;
  HPCC_Params params;
  time_t currentTime, startTime;
  void *extdata;

  MPI_Init( &argc, &argv );

  if (HPCC_external_init( argc, argv, &extdata ))
    goto hpcc_end;

  /* HPCC_Init() set params.RunMPIRandomAccess=1 */
  if (HPCC_Init( &params ))
    goto hpcc_end;

  parse_arguments(argc, argv, &params);

  MPI_Comm_size( MPI_COMM_WORLD, &commSize );
  MPI_Comm_rank( MPI_COMM_WORLD, &myRank );

  outFname = params.outFname;

  time( &startTime ); 
  /* -------------------------------------------------- */
  /*                 MPI RandomAccess                   */
  /* -------------------------------------------------- */
  if (params.RunMPIRandomAccess) { 
    MPI_Barrier( MPI_COMM_WORLD ); 
      
    BEGIN_IO( myRank, outFname, outputFile); 
    fprintf( outputFile, "Begin of MPIRandomAccess section.\n" ); 
    END_IO( myRank, outputFile );

    if (params.RunMPIRandomAccess) HPCC_MPIRandomAccess( &params ); 
    time( &currentTime ); 
    
    BEGIN_IO(myRank, outFname, outputFile); 
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime)); 
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of MPIRandomAccess section.\n" ); 
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                  StarRandomAccess                  */
  /* -------------------------------------------------- */

  if (params.RunStarRandomAccess) {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of StarRandomAccess section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunStarRandomAccess) HPCC_StarRandomAccess( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of StarRandomAccess section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                 SingleRandomAccess                 */
  /* -------------------------------------------------- */

  if (params.RunSingleRandomAccess){
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of SingleRandomAccess section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunSingleRandomAccess) HPCC_SingleRandomAccess( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of SingleRandomAccess section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                 MPI RandomAccess LCG               */
  /* -------------------------------------------------- */

  if (params.RunMPIRandomAccess_LCG) {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of MPIRandomAccess_LCG section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunMPIRandomAccess_LCG) HPCC_MPIRandomAccess_LCG( &params );

    time( &currentTime );
    BEGIN_IO(myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of MPIRandomAccess_LCG section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                  StarRandomAccess LCG              */
  /* -------------------------------------------------- */

  if (params.RunStarRandomAccess_LCG) {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of StarRandomAccess_LCG section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunStarRandomAccess_LCG) HPCC_StarRandomAccess_LCG( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of StarRandomAccess_LCG section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                 SingleRandomAccess LCG             */
  /* -------------------------------------------------- */

  if (params.RunSingleRandomAccess_LCG){
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of SingleRandomAccess_LCG section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunSingleRandomAccess_LCG) HPCC_SingleRandomAccess_LCG( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of SingleRandomAccess_LCG section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                       PTRANS                       */
  /* -------------------------------------------------- */

  if (params.RunPTRANS) {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of PTRANS section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunPTRANS) PTRANS( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of PTRANS section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                    StarDGEMM                       */
  /* -------------------------------------------------- */

  if (params.RunStarDGEMM) {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of StarDGEMM section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunStarDGEMM) HPCC_StarDGEMM( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of StarDGEMM section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                    SingleDGEMM                     */
  /* -------------------------------------------------- */

  if (params.RunSingleDGEMM) {
    MPI_Barrier( MPI_COMM_WORLD );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of SingleDGEMM section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunSingleDGEMM) HPCC_SingleDGEMM( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of SingleDGEMM section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                    StarSTREAM                      */
  /* -------------------------------------------------- */

  if (params.RunStarStream){
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of StarSTREAM section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunStarStream) HPCC_StarStream( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of StarSTREAM section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                    SingleSTREAM                    */
  /* -------------------------------------------------- */

  if (params.RunSingleStream) {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of SingleSTREAM section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunSingleStream) HPCC_SingleStream( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of SingleSTREAM section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                       MPIFFT                       */
  /* -------------------------------------------------- */

  if (params.RunMPIFFT)  {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of MPIFFT section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunMPIFFT) HPCC_MPIFFT( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of MPIFFT section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                      StarFFT                       */
  /* -------------------------------------------------- */

  if (params.RunStarFFT) {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of StarFFT section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunStarFFT) HPCC_StarFFT( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of StarFFT section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                      SingleFFT                     */
  /* -------------------------------------------------- */

  if (params.RunSingleFFT) {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of SingleFFT section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunSingleFFT) HPCC_SingleFFT( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of SingleFFT section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                  Latency/Bandwidth                 */
  /* -------------------------------------------------- */

  if (params.RunLatencyBandwidth) {
    MPI_Barrier( MPI_COMM_WORLD );

    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of LatencyBandwidth section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunLatencyBandwidth) main_bench_lat_bw( &params );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of LatencyBandwidth section.\n" );
    END_IO( myRank, outputFile );
  }

  /* -------------------------------------------------- */
  /*                        HPL                         */
  /* -------------------------------------------------- */

  if (params.RunHPL) {
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile, "Begin of HPL section.\n" );
    END_IO( myRank, outputFile );

    if (params.RunHPL) HPL_main( argc, argv, &params.HPLrdata, &params.Failure );

    time( &currentTime );
    BEGIN_IO( myRank, outFname, outputFile);
    fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
    fprintf( outputFile,"Wall time = %ld seconds\n",(long)currentTime - (long)startTime);
    fprintf( outputFile, "End of HPL section.\n" );
    END_IO( myRank, outputFile );
  }

  hpcc_end:

  HPCC_Finalize( &params );

  HPCC_external_finalize( argc, argv, extdata );

  MPI_Finalize();
  return 0;
}
