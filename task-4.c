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

    int k = rand() % 10 + 1;
    if( rank == 0 ){
        int n = ( size - 1 ) * k;
        int x[n], y[n], z[n];
        printf( "x, y:\n" );
        for( int i = 0; i < n; i++ ) {
            x[i] = rand() % 10;
            y[i] = rand() % 10;
            printf( "(%d, %d)\n", x[i], y[i] );
        }
        for( int i = 1; i < size; i++ ){
            MPI_Send( x + ( i - 1 ) * k, k, MPI_INT, i, 0, MPI_COMM_WORLD );
            MPI_Send( y + ( i - 1 ) * k, k, MPI_INT, i, 0, MPI_COMM_WORLD );
        }
        for( int i = 1; i < size; i++ ){
            MPI_Status status;
            MPI_Recv( z + ( i - 1 ) * k, k, MPI_INT, i, 0, MPI_COMM_WORLD, &status );
        }
        printf( "z: " );
        for( int i = 0; i < n; i++ ){
            if( i ) printf( ", " );
            printf( "%d", z[i] );
        }
        puts("");
    } else {
        int x[k];
        int y[k];
        int z[k];
        MPI_Status status;
        MPI_Recv( x, k, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
        MPI_Recv( y, k, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
        for( int i = 0; i < k; i++ ) z[i] = x[i] + y[i];
        MPI_Send( z, k, MPI_INT, 0, 0, MPI_COMM_WORLD );
    }

    MPI_Finalize();
    return 0;
}
