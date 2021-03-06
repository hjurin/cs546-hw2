/* Gaussian elimination without pivoting.
* Compile with "gcc gauss.c"
*/

/* ****** ADD YOUR CODE AT THE END OF THIS FILE. ******
* You need not submit the provided code.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>

/* Program Parameters */
#define MAXN 2000  /* Max value of N */
int N;  /* Matrix size */
char F[40]; /* Output file */
char print_tmp[200]; /* the string used for print_all */
volatile float A[MAXN][MAXN], B[MAXN], X[MAXN]; /* Matrices and vectors */
int nb_threads = 8; // the number of threads that will be created

/* A * X = B, solve for X */

/* Prototype */
void gauss();  /* The function you will provide.
* It is this routine that is timed.
* It is called only on the parent.
*/

/* returns a seed for srand based on the time */
unsigned int time_seed() {
    struct timeval t;
    struct timezone tzdummy;

    gettimeofday(&t, &tzdummy);
    return (unsigned int)(t.tv_usec);
}

// echo elapsed time in a csv file
void print_time(char * seed, float time) {
    char time_file[20] = "elapsed_times.csv";
    FILE * file = fopen(time_file, "r");
    if (file == NULL) { // if the file doesn't exist we create it with headers
        file = fopen(time_file, "a");
        fprintf(file, "program;size_matrix;seed;nb_processus;nb_threads;time\n");
    }
    file = fopen(time_file, "a");
    char current_filename[20] = __FILE__;
    fprintf(file, "%s;%d;%s;%d;%d;%g\n", strtok(current_filename, "."), N, seed, -1, nb_threads, time);
}

/* output the given string to the file and to the output stream */
void print_all() {
    printf("%s", print_tmp);

    /* builds the repertory for output files */
    struct stat sb;
    if (stat("output_files", &sb) != 0 || !S_ISDIR(sb.st_mode)) {
        mode_t mode = 0774;
        mkdir("output_files", mode);
    }

    char filepath[40];
    sprintf(filepath, "output_files/%s", F);
    FILE *f = fopen(filepath, "a");
    fprintf(f, "%s", print_tmp);
    fclose(f);
}

/* Set the program parameters from the command-line arguments */
void parameters(int argc, char **argv) {
    int seed = 0;  /* Random seed */

    /* Set the output file name from the command-line argument */
    if (argc == 4) {
        sprintf(F, "%s", argv[3]);
    }
    else { // the default file name is hh_mm_ss_source_filename_without_extension.txt
        time_t intps = time(NULL);
        struct tm *t = localtime(&intps);
        char filename[20] = __FILE__;
        sprintf(F, "%d_%d_%d_%s.txt", t->tm_hour, t->tm_min, t->tm_sec, strtok(filename, "."));
    }
    sprintf(print_tmp, "\nOutput file = %s\n", F);
    print_all();

    /* Read command-line arguments */
    srand(time_seed());  /* Randomize */

    if (argc >= 3) {
        seed = atoi(argv[2]);
        srand(seed);
        sprintf(print_tmp, "Random seed = %i\n", seed);
        print_all();
    }
    if (argc >= 2) {
        N = atoi(argv[1]);
        if (N < 1 || N > MAXN) {
            sprintf(print_tmp, "N = %i is out of range.\n", N);
            print_all();
            exit(0);
        }
    }
    else {
        sprintf(print_tmp, "Usage: %s <matrix_dimension> [random seed]\n",
        argv[0]);
        print_all();
        exit(0);
    }



    /* Print parameters */
    sprintf(print_tmp, "\nMatrix dimension N = %i.\n", N);
    print_all();
}

/* Initialize A and B (and X to 0.0s) */
void initialize_inputs() {
    int row, col;

    sprintf(print_tmp, "\nInitializing...\n");
    print_all();
    for (col = 0; col < N; col++) {
        for (row = 0; row < N; row++) {
            A[row][col] = (float)rand() / 32768.0;
        }
        B[col] = (float)rand() / 32768.0;
        X[col] = 0.0;
    }

}

/* Print input matrices */
void print_inputs() {
    int row, col;

    if (N < 10) {
        sprintf(print_tmp, "\nA =\n\t");
        print_all();
        for (row = 0; row < N; row++) {
            for (col = 0; col < N; col++) {
                sprintf(print_tmp, "%5.2f%s", A[row][col], (col < N-1) ? ", " : ";\n\t");
                print_all();
            }
        }
        sprintf(print_tmp, "\nB = [");
        print_all();
        for (col = 0; col < N; col++) {
            sprintf(print_tmp, "%5.2f%s", B[col], (col < N-1) ? "; " : "]\n");
            print_all();
        }
    }
}

void print_X() {
    int row;
    sprintf(print_tmp, "\nX = [");
    print_all();
    for (row = 0 ; row < ((N <= 100) ? N : 100) ; row++) {
        sprintf(print_tmp, "%5.2f%s", X[row], (row < N-1) ? "; " : "]\n");
        print_all();
    }
}

int main(int argc, char **argv) {
    /* Timing variables */
    struct timeval etstart, etstop;  /* Elapsed times using gettimeofday() */
    struct timezone tzdummy;
    clock_t etstart2, etstop2;  /* Elapsed times using times() */
    unsigned long long usecstart, usecstop;
    struct tms cputstart, cputstop;  /* CPU times for my processes */

    /* Process program parameters */
    parameters(argc, argv);

    /* Initialize A and B */
    initialize_inputs();

    /* Print input matrices */
    print_inputs();

    /* Start Clock */
    sprintf(print_tmp, "\nStarting clock.\n");
    print_all();
    gettimeofday(&etstart, &tzdummy);
    etstart2 = times(&cputstart);

    /* Gaussian Elimination */
    gauss();

    /* Stop Clock */
    gettimeofday(&etstop, &tzdummy);
    etstop2 = times(&cputstop);
    sprintf(print_tmp, "Stopped clock.\n");
    print_all();
    usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
    usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;

    /* Display output */
    print_X();

    /* Display timing results */
    sprintf(print_tmp, "\nElapsed time = %g ms.\n",
    (float)(usecstop - usecstart)/(float)1000);
    print_all();

    // gather timing results
    print_time((argc >= 3) ? argv[2] : "?", (float)(usecstop - usecstart)/(float)1000);

    sprintf(print_tmp, "(CPU times are accurate to the nearest %g ms)\n",
    1.0/(float)CLOCKS_PER_SEC * 1000.0);
    print_all();
    sprintf(print_tmp, "My total CPU time for parent = %g ms.\n",
    (float)( (cputstop.tms_utime + cputstop.tms_stime) -
    (cputstart.tms_utime + cputstart.tms_stime) ) /
    (float)CLOCKS_PER_SEC * 1000);
    print_all();
    sprintf(print_tmp, "My system CPU time for parent = %g ms.\n",
    (float)(cputstop.tms_stime - cputstart.tms_stime) /
    (float)CLOCKS_PER_SEC * 1000);
    print_all();
    sprintf(print_tmp, "My total CPU time for child processes = %g ms.\n",
    (float)( (cputstop.tms_cutime + cputstop.tms_cstime) -
    (cputstart.tms_cutime + cputstart.tms_cstime) ) /
    (float)CLOCKS_PER_SEC * 1000);
    print_all();
    /* Contrary to the man pages, this appears not to include the parent */
    sprintf(print_tmp, "--------------------------------------------\n");
    print_all();

    exit(0);
}

/* ------------------ Above Was Provided --------------------- */

// debugging function
void printAB(char * opt) {
    int row, col;
    printf("\nA%s =\n\t", opt);
    for (row = 0; row < N; row++) {
        for (col = 0; col < N; col++) {
            printf("%5.2f%s", A[row][col], (col < N-1) ? ", " : ";\n\t");
        }
    }
    printf("\nB%s =\n\t", opt);
    for (row = 0; row < N; row++) {
        printf("%5.2f%s", B[row], (row < N-1) ? ";\n\t " : ";\n");
    }
}

// thread args
struct pt_args {
    int rank;
};

// number of threads to wait and its mutex
volatile int working_threads;
volatile int sleeping_threads;
volatile char go[MAXN];
pthread_mutex_t to_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t threads_ended_turn = PTHREAD_COND_INITIALIZER;
pthread_cond_t threads_started_turn = PTHREAD_COND_INITIALIZER;

// The function executed by created threads in gauss()
static void * partial_gauss_elimination (void * _args) {
    int norm, row, col;  /* Normalization row, and zeroing
    * element row and col */
    float multiplier;

    /* thread args */
    struct pt_args * args = (struct pt_args *) _args;

    /* Gaussian elimination */
    for (norm = 0; norm < N - 1; norm++) {
        for (row = norm + 1; row < N; row += nb_threads) {
            if(row + args->rank >= N) {
                break;
            }
            multiplier = A[row + args->rank][norm] / A[norm][norm];
            for (col = norm; col < N; col++) {
                A[row + args->rank][col] -= A[norm][col] * multiplier;
            }
            B[row + args->rank] -= B[norm] * multiplier;
        }
        // printf("%d is done working for round %d\n",args->rank, norm);
        // fflush(stdout);

        // // signal other thread we are done and wait until all threads have finished current submatrix
        // while (sleeping_threads != 0 && norm != 0 && go[norm - 1] == 1) {
        //     printf("%d waits everybody to wake up\n", args->rank);
        //     pthread_cond_wait(&threads_started_turn, &to_wait_mutex);
        // }
        pthread_mutex_lock(&to_wait_mutex);

        working_threads--;
        // printf("I'm %d and there are still %d threads to wait.\n", args->rank, working_threads);
        if(working_threads == 0) {
            // printf("%d gives the go !\n", args->rank);
            go[norm] = 1;
            working_threads = nb_threads;
            int i;
            for (i = 0; i < nb_threads - 1; i++) {
                pthread_cond_signal(&threads_ended_turn);
            }
        }

        while (go[norm] == 0) {
            // printf("%d is in round %d and goes to sleep\n", args->rank, norm);
            // sleeping_threads++;
            pthread_cond_wait(&threads_ended_turn, &to_wait_mutex);
            // sleeping_threads--;
            // printf("%d is in round %d and awake\n", args->rank, norm);
        }
        // if (sleeping_threads == 0) {
        //     printf("round %d and everybody is awake!\n", norm);
        //     pthread_cond_signal(&threads_started_turn);
        // }
        pthread_mutex_unlock(&to_wait_mutex);
    }

    return NULL;
}

/****** You will replace this routine with your own parallel version *******/
/* Provided global variables are MAXN, N, A[][], B[], and X[],
* defined in the beginning of this code.  X[] is initialized to zeros.
*/
void gauss() {
    int norm, row, col;  /* Normalization row, and zeroing
    * element row and col */
    working_threads = nb_threads;
    sleeping_threads = 0;

    for(norm = 0; norm < N; norm++)
        go[norm] = 0;
    pthread_t threads[nb_threads]; // the ids of threads
    struct pt_args args[nb_threads]; // thread args
    int i;
    for(i = 0; i < nb_threads; ++i)
        args[i].rank = i;
    int ret = 0; // will store the return value of pthread_create calls

    sprintf(print_tmp, "Computing Serially.\n");
    print_all();

    // launch threads
    for (i = 0; i < nb_threads; i++) {
        ret = pthread_create(&threads[i], NULL, partial_gauss_elimination, (void *)&args[i]);
        if (ret) {
            fprintf(stderr, "%s", strerror (ret));
        }
    }

    // wait for threads
    for (i = 0; i < nb_threads; i++) {
        pthread_join(threads[i], NULL);
    }


    /* (Diagonal elements are not normalized to 1.  This is treated in back
    * substitution.)
    */


    /* Back substitution */
    for (row = N - 1; row >= 0; row--) {
        X[row] = B[row];
        for (col = N-1; col > row; col--) {
            X[row] -= A[row][col] * X[col];
        }
        X[row] /= A[row][row];
    }
}
