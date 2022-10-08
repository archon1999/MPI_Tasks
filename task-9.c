#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <mpi.h>


typedef struct {
    int val;
    int index;
} elem;


int main(int argc, char *argv[]){
    int errCode;

    if ((errCode = MPI_Init(&argc, &argv)) != 0){
        return errCode;
    }

    int rank, size;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int n = rand() % 20 + 1;
    elem a[n];
    int sendcounts[size], displs[size];

    if( rank == 0 ){
        srand( time( 0 ) );
        printf( "Elements: " );
        for( int i = 0; i < n; i++ ) {
            if( i ) printf( ", " );
            a[i].val = rand() % 100;
            a[i].index = i;
            printf( "%d", a[i].val );
        }
        printf( "\n" );
    }

    for( int i = 0; i < size; i++ ){
        displs[i] = 0;
        sendcounts[i] = n / size;
        if( n % size >= i + 1 ){
            sendcounts[i]++;
        }
        if( i ) displs[i] = displs[i - 1] + sendcounts[i - 1];
    }

    elem global_min, local_min;
    local_min.val = INT_MAX;
    int recvcount = n / size;
    if( n % size >= rank + 1 ){
        recvcount++;
    }

    elem *buf = malloc( sizeof( elem ) * recvcount );
    MPI_Scatterv( a, sendcounts, displs, MPI_2INT, buf, recvcount, MPI_2INT, 0, MPI_COMM_WORLD );
    for( int i = 0; i < recvcount; i++ ) {
        if( buf[i].val < local_min.val ){
            local_min = buf[i];
        }
    }
    MPI_Reduce( &local_min, &global_min, 1, MPI_2INT, MPI_MINLOC, 0, MPI_COMM_WORLD );

    if( rank == 0 ){
        printf( "Min Value: %d\nIndex: %d\n", global_min.val, global_min.index );
    }

    free( buf );

    MPI_Finalize();
    return 0;
}
