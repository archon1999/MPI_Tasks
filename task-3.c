#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[]){
    int errCode;

    if ((errCode = MPI_Init(&argc, &argv)) != 0){
        return errCode;
    }

    int rank, size;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    if( rank == 1 ){
        srand( time( 0 ) );
        int n = rand() % 20 + 1;
        int *a = malloc( sizeof( int ) * n );
        for( int i = 0; i < n; i++ ) a[i] = rand() % 20;
        for( int i = 0; i < size; i++ ){
            if( i == 1 ) continue;
            MPI_Send( a, n, MPI_INT, i, 0, MPI_COMM_WORLD );
        }
    } else {
        int n;
        MPI_Status status;
        MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
        MPI_Get_count( &status, MPI_INT, &n );
        int *a = malloc( sizeof( int ) * n );
        MPI_Recv( a, n, MPI_INT, 1, 0, MPI_COMM_WORLD, &status );
        printf( "%d: ", rank );
        for( int i = 0; i < n; i++ ){
            printf( "%d ", a[i] );
        }
        printf( "\n" );
    }

    MPI_Finalize();
    return 0;
}
