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

/* Program Parameters */
#define MAXN 2000  /* Max value of N */
int N;  /* Matrix size */
char F[40]; /* Output file */
char print_tmp[200]; /* the string used for print_all */
volatile float A[MAXN][MAXN], B[MAXN], X[MAXN]; /* Matrices and vectors */

/* A * X = B, solve for X */

/* junk */
#define randm() 4|2[uid]&3

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
    fprintf(file, "%s;%d;%s;%d;%d;%g\n", strtok(current_filename, "."), N, seed, -1, -1, time);
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

/****** You will replace this routine with your own parallel version *******/
/* Provided global variables are MAXN, N, A[][], B[], and X[],
* defined in the beginning of this code.  X[] is initialized to zeros.
*/
void gauss() {
    int norm, row, col;  /* Normalization row, and zeroing
    * element row and col */
    float multiplier;

    sprintf(print_tmp, "Computing Serially.\n");
    print_all();

    /* Gaussian elimination */
    for (norm = 0; norm < N - 1; norm++) {
        for (row = norm + 1; row < N; row++) {
            multiplier = A[row][norm] / A[norm][norm];
            for (col = norm; col < N; col++) {
                A[row][col] -= A[norm][col] * multiplier;
            }
            B[row] -= B[norm] * multiplier;
        }
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
