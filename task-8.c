#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

const int ROOT = 0;

int main(int argc, char *argv[]){
    int errCode;

    if ((errCode = MPI_Init(&argc, &argv)) != 0){
        return errCode;
    }

    int rank, size;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int n = 10;
    int x[n], y[n], sendcounts[size], displs[size];
    int sum_sq_x;

    if( rank == ROOT ){
        printf( "[x, y]: " );
        for( int i = 0; i < n; i++ ) {
            x[i] = rand() % 10;
            y[i] = rand() % 10;
            if( i ) printf( ", ");
            printf( "(%d, %d)", x[i], y[i] );
        }
        printf( "\n" );

        for( int i = 0; i < size; i++ ){
            displs[i] = 0;
            sendcounts[i] = n / size;
            if( n % size >= i + 1 ){
                sendcounts[i]++;
            }
            if( i ) displs[i] = displs[i - 1] + sendcounts[i - 1];
        }
    }

    int local_sum = 0, global_sum = 0;
    int recvcount = n / size;
    if( n % size >= rank + 1 ){
        recvcount++;
    }
    int *x_buf = malloc( sizeof( int ) * recvcount );
    int *y_buf = malloc( sizeof( int ) * recvcount );
 
    MPI_Scatterv( x, sendcounts, displs, MPI_INT, x_buf, recvcount, MPI_INT, ROOT, MPI_COMM_WORLD );
    MPI_Scatterv( y, sendcounts, displs, MPI_INT, y_buf, recvcount, MPI_INT, ROOT, MPI_COMM_WORLD );

    for( int i = 0; i < recvcount; i++ ) {
        local_sum += x_buf[i] * y_buf[i];
    }

    MPI_Reduce( &local_sum, &global_sum, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD );

    if( rank == 0 ){
        printf( "|x*y| = %d\n", global_sum );
    }

    free( x_buf );
    free( y_buf );

    MPI_Finalize();
    return 0;
}
