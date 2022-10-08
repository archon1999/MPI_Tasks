#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MAT_SIZE 5

int main(int argc, char *argv[]){
    int errCode;

    if ((errCode = MPI_Init(&argc, &argv)) != 0){
        return errCode;
    }

    int rank, size;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    if( rank == 0 ){
        srand( time( 0 ) );
        int a[MAT_SIZE][MAT_SIZE];
        int x[MAT_SIZE];
        for( int i = 0; i < MAT_SIZE; i++ ){
            for( int j = 0; j < MAT_SIZE; j++ ){
                a[i][j] = rand() % 10;
                printf( "%d ", a[i][j] );
            }
            printf( "\n" );
        }

        int workers_num = size - 1;
        int avgrow = MAT_SIZE / workers_num;
        int cur_row = 0;
        for( int dest = 1; dest <= workers_num; dest++ ){
            int rows = avgrow;
            if( workers_num % MAT_SIZE <= dest ){
                rows++;
            }
            int data[2] = { cur_row, rows };
            MPI_Send( data, 2, MPI_INT, dest, 0, MPI_COMM_WORLD );
            MPI_Send( &a[cur_row][0], rows*MAT_SIZE, MPI_INT, dest, 0, MPI_COMM_WORLD );
            cur_row += rows;
        }

        cur_row = 0;
        for( int source = 1; source <= workers_num; source++ ){
            int rows = avgrow;
            if( workers_num % MAT_SIZE <= source ){
                rows++;
            }
            MPI_Status status;
            MPI_Recv( x + cur_row, rows, MPI_INT, source, 0, MPI_COMM_WORLD, &status );
            cur_row += rows;
        }

        printf( "x: " );
        for( int i = 0; i < MAT_SIZE; i++ ){
            if( i ) printf( ", " );
            printf( "%d", x[i] );
        }
        printf( "\n" );
    } else {
        int data[2];
        MPI_Status status;
        MPI_Recv( data, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
        int cur_row = data[0];
        int count = data[1];
        int b[count][MAT_SIZE];
        MPI_Recv( &b[0][0], count*MAT_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
        int buf[count];
        for( int row = cur_row; row < cur_row + count; row++ ){
            buf[row - cur_row] = b[row - cur_row][row];
        }
        MPI_Send( buf, count, MPI_INT, 0, 0, MPI_COMM_WORLD );
    }

    MPI_Finalize();
    return 0;
}
