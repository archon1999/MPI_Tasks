#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <math.h>


int main(int argc, char *argv[]){
    int errCode;

    if ((errCode = MPI_Init(&argc, &argv)) != 0){
        return errCode;
    }

    int rank, size;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int n = 100;
    int x[n], sendcounts[size], displs[size];
    int sum_sq_x;

    if( rank == 0 ){
        printf( "x: " );
        for( int i = 0; i < n; i++ ) {
            x[i] = i;
            if( i ) printf( ", ");
            printf( "%d", x[i] );
        }
        printf( "\n" );
    }

    int local_sum = 0, global_sum = 0;
    int recvcount = n / size;
    if( n % size >= rank + 1 ){
        recvcount++;
    }
    int *buf = malloc( sizeof( int ) * recvcount );
    MPI_Scatterv( x, sendcounts, displs, MPI_INT, buf, recvcount, MPI_INT, 0, MPI_COMM_WORLD );
    for( int i = 0; i < recvcount; i++ ) {
        local_sum += buf[i] * buf[i];
    }

    MPI_Reduce( &local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );

    if( rank == 0 ){
        double norm = global_sum;
        printf( "%.2f\n", norm );
    }

    free( buf );

    MPI_Finalize();
    return 0;
}
