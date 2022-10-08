#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>


int main( int argc, char *argv[] ){
    int errCode;

    if ((errCode = MPI_Init(&argc, &argv)) != 0){
        return errCode;
    }

    int rank, size;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    if( size != 2 ){
		if( rank == 0 ){
			printf("This program requires exactly 2 MPI ranks\n");
		}
		MPI_Finalize();
		exit(0);
	}

    MPI_Status status;
    MPI_Request recv_request, send_request;

    if( rank == 0 ){
        printf( "Perfomance ping pong\n" );
    }

	for( int i = 15; i <= 26; i++ ){
		int n = 1 << i;
		int *a = malloc( sizeof( int ) * n );

		for( int i = 0; i < n; i++ ){
			a[i] = 0;
		}
	
		int tag1 = 1;
		int tag2 = 2;

		double start_time, stop_time, time;
		start_time = MPI_Wtime();
	
        if( rank == 0 ){
            MPI_Send( a, n, MPI_INT, 1, tag1, MPI_COMM_WORLD );
            MPI_Recv( a, n, MPI_INT, 1, tag2, MPI_COMM_WORLD, &status );
        } else if( rank == 1 ){
            MPI_Recv( a, n, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status );
            MPI_Send( a, n, MPI_INT, 0, tag2, MPI_COMM_WORLD );
        }

		stop_time = MPI_Wtime();
		time = stop_time - start_time;

		int bytes = 4*n;
		if( rank == 0 ) {
            printf("Size (B): %10d, Time (s): %15.9f\n", bytes, time );
        }
        free( a );
	}

    if( rank == 0 ){
        printf( "Perfomance ping ping\n" );
    }

	for( int i = 15; i <= 26; i++ ){
		int n = 1 << i;
		int *a = malloc( sizeof( int ) * n );

		for( int i = 0; i < n; i++ ){
			a[i] = 0;
		}
	
		int tag1 = 1;
		int tag2 = 2;
	
		double start_time, stop_time, time;
		start_time = MPI_Wtime();
	
        if( rank == 0 ){
            MPI_Isend( a, n, MPI_INT, 1, tag2, MPI_COMM_WORLD, &send_request );
            MPI_Irecv( a, n, MPI_INT, 1, tag1, MPI_COMM_WORLD, &recv_request );
            MPI_Wait( &recv_request, &status );
        } else if( rank == 1 ){
            MPI_Isend( a, n, MPI_INT, 0, tag1, MPI_COMM_WORLD, &send_request );
            MPI_Irecv( a, n, MPI_INT, 0, tag2, MPI_COMM_WORLD, &recv_request );
            MPI_Wait( &recv_request, &status );
        }

		stop_time = MPI_Wtime();
		time = stop_time - start_time;

		int bytes = 4*n;
		if( rank == 0 ) {
            printf("Size (B): %10d, Time (s): %15.9f\n", bytes, time );
        }

        free( a );
	}

    MPI_Finalize();
    return 0;
}
