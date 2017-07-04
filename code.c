#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "err.h"
#define NR_PROC 3
#define BUFFOR_SIZE 80

double sum(double* vector, int start, int end) {
	int i;
	double sum = 0.0f;
	for(i=start; i<end; i++) {
		sum += vector[i];
	}
	return sum;
}

int main(int argc, char **argv) {
	clock_t start = clock();
	FILE* f = fopen("vector.dat", "r");
    char buffor[BUFFOR_SIZE+1];
	double* vector;
	int n;
	int i;
	fgets(buffor, BUFFOR_SIZE, f);
 	n = atoi(buffor);
	vector = malloc(sizeof(double) * n);
	printf("Vector has %d elements\n", n);
	for(i=0; i<n; i++) {
		fgets(buffor, BUFFOR_SIZE, f);
		vector[i] = atof(buffor);
	}
	fclose(f);
	printf("v = [ ");
	for(i=0; i<n; i++) {
		printf("%f ", vector[i]);
	}
	printf("]\n");
	
	int* range;
	float* result;
	int   idRange;
	int   idResult;
		

	key_t k;
	k = ftok(".", 'a');
	key_t k1;
	k1 = ftok(".", 'b');
	
 
	if ((idRange=shmget (k, 100, 0666|IPC_CREAT)) == -1)  /* Stworzenie segementu */
 	error ("blad");  
	if ((idResult=shmget (k1, 100, 0666|IPC_CREAT)) == -1)  /* Stworzenie segementu */
 	error ("blad");  

	if ((range =shmat(idRange, 0, 0)) < 0) /* Przy³¹czenie */
 		error ("blad");

	if ((result =shmat(idResult, 0, 0)) < 0) /* Przy³¹czenie */
 		error ("blad");




	printf ("\n");
	
	
	for (i=0; i<NR_PROC; i++){
	result[i] = 0;
	}
	printf("\n");

	int ii;
	for(ii=0; ii<= n+1; ii = ii + n/NR_PROC){
		range[ii] = ii;
	}
	printf("\n");



	pid_t sygnaly[NR_PROC];
    pid_t pid;
  
  /* wypisuje identyfikator procesu */
  printf("Moj PID = %d\n", getpid());
  /* tworzy nowe procesy */
  for (i = 1; i <= NR_PROC; i++)
    switch (pid = fork()) {
      case -1: /* blad */
        error("Error in fork\n");
     case 0: /* proces potomny */
	

		if ((range =shmat(idRange, 0, 0)) < 0) /* Przy³¹czenie */
			error ("blad");

	    if ((result =shmat(idResult, 0, 0)) < 0) /* Przy³¹czenie */
			error ("blad");
		result[i-1] = sum(vector, range[(i-1)*(n/NR_PROC)], range[i*(n/NR_PROC)]);
		printf("Jestem procesem potomnym nr %d, Wynik = %f\n", getpid(), result[i-1]);
		sygnaly[i-1] = getpid();

       
       return 0;
     default: /* proces macierzysty */
       


	printf("\nJestem procesem macierzystym. Moj PID = %d\n", getpid());
	if ((range =shmat(idRange, 0, 0)) < 0) /* Przy³¹czenie */
 		error ("blad");


	if ((result =shmat(idResult, 0, 0)) < 0) /* Przy³¹czenie */
 		error ("blad");

      } /*switch*/
  /* czeka na zakonczenie procesow potomnych */
	kill (sygnaly[i-1], SIGUSR1);
    for (i = 1; i <= NR_PROC; i++)
      if (wait(0) == -1)
        error("Error in wait\n");

	float wynikost = 0.0;
	for (i=0; i<NR_PROC; i++){
		wynikost = wynikost + result[i];
	}
	printf ("\n Wynik dodawania wektora = %f\n", wynikost);
	clock_t end = clock();
	float seconds = (float)(end-start)/CLOCK_PER_SEC;
	printf("czas = %f\n", seconds);

	return 0;
}