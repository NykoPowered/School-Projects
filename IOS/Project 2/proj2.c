/*
 * Autor:   Nikolas Masica, xmasic00
 * Projekt 2 IOS
 * River Crossing Problem.
 */
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>



//zdielana pamat
typedef struct{
    sem_t *memberType;
    sem_t *memberExits;
    sem_t *Write;
    sem_t *boatCruise;
    sem_t *CruiseDone;
    sem_t *semaphore;
    sem_t *waits;
    sem_t *hackBoards;
    sem_t *serfBoards;
    int NH;
    int NS;
    int hackExits;
    int serfExits;
    int processes;
    int A;
    int HACK;
    int SERF;
    int boatCrew;
    int PierProcesses;
}TMemory;

//ulozenie zadanych parametrov
typedef struct{
	int P;
	int H;
	int S;
	int R;
	int W;
	int C;
	
}TArguments;


//parsovanie arguemntov
int parseArguments(int argc, char **argv, TArguments *args){
    if (argc != 7){
        return 1;
    }
    char *check = NULL;
    short test = 0;

    args->P = strtol(argv[1], &check, 10);
    (*check == '\0')? test = test : ++test;
    args->H = strtol(argv[2], &check, 10);
    (*check == '\0')? test = test : ++test;
    args->S = strtol(argv[3], &check, 10);
    (*check == '\0')? test = test : ++test;
    args->R = strtol(argv[4], &check, 10);
    (*check == '\0')? test = test : ++test;
    args->W = strtol(argv[5], &check, 10);
    (*check == '\0')? test = test : ++test;
    args->C = strtol(argv[6], &check, 10);
    (*check == '\0')? test = test : ++test;

    if(test != 0){
        return 1;
    }

    if((args->P % 2 != 0) || (args->H > 2000) || (args->S > 2000) || (args->R > 2000) || (args->W > 2000) || (args->C < 5) || (args->P < 2) || (args->H < 0) || (args->S < 0) || (args->R < 0) || (args->W < 0)){
        return 1;
    }



    return 0;

}

//inicializacia semaforov a kontrola
int initSemaphores(TMemory *structs){
    if((structs->semaphore = sem_open("/xmasic00-semaphore", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED){
        fprintf(stderr, "Nepodarilo sa inicializovat semafor!\n");
        return 1;
    }
    if((structs->waits = sem_open("/xmasic00-waits", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED){
        fprintf(stderr, "Nepodarilo sa inicializovat semafor!\n");
        return 1;
    }
    if((structs->hackBoards = sem_open("/xmasic00-hackBoards", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED){
        fprintf(stderr, "Nepodarilo sa inicializovat semafor!\n");
        return 1;
    }
    if((structs->serfBoards = sem_open("/xmasic00-serfBoards", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED){
        fprintf(stderr, "Nepodarilo sa inicializovat semafor!\n");
        return 1;
    }
    if((structs->memberType = sem_open("/xmasic00-memberType", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED){
        fprintf(stderr, "Nepodarilo sa inicializovat semafor!\n");
        return 1;
    }
    if((structs->memberExits = sem_open("/xmasic00-memberExits", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED){
        fprintf(stderr, "Nepodarilo sa inicializovat semafor!\n");
        return 1;
    }
    if((structs->Write = sem_open("/xmasic00-write", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED){
        fprintf(stderr, "Nepodarilo sa inicializovat semafor!\n");
        return 1;
    }
    if((structs->boatCruise = sem_open("/xmasic00-boatCruise", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED){
        fprintf(stderr, "Nepodarilo sa inicializovat semafor!\n");
        return 1;
    }
    if((structs->CruiseDone = sem_open("/xmasic00-CruiseDone", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED){
        fprintf(stderr, "Nepodarilo sa inicializovat semafor!\n");
        return 1;
    }
    structs->A = 1;

    return 0;
}

//zrusenie semaforov
void unlink_memsem(){
    sem_unlink("/xmasic00-semaphore");
    sem_unlink("/xmasic00-waits");
    sem_unlink("/xmasic00-hackBoards");
    sem_unlink("/xmasic00-serfBoards");
    sem_unlink("/xmasic00-memberType");
    sem_unlink("/xmasic00-memberExits");
    sem_unlink("/xmasic00-write");
    sem_unlink("/xmasic00-boatCruise");
    sem_unlink("/xmasic00-CruiseDone");
}




//Funkcia pre vypis do suboru
void setOutput(const char *message, FILE *output, ...){
    va_list vaList;
    va_start (vaList, output);

    vfprintf(output, message, vaList);

    fflush(NULL);  //vyprazdnenie bufferu

    va_end(vaList);
}

void hackProcess(int id, TMemory *data, TArguments args, FILE *output){
	int crewCaptain =0;
	int cruiseTime;  			 //doba plavby
	
	sem_wait(data->semaphore);
	sem_wait(data->Write);

	setOutput("%d  : HACK %d  : starts\n", output, data->A++, id);
    sem_post(data->Write);
	sem_post(data->semaphore);
	sem_wait(data->waits);
	sem_wait(data->semaphore);
	sem_wait(data->Write);
	++data->NH;
	++data->PierProcesses;
    cruiseTime = (rand() % (args.W+1));
    if(data->PierProcesses > args.C){
        setOutput("%d  : HACK %d  : leaves queue  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);
        usleep(cruiseTime * 1000);
        setOutput("%d  : HACK %d  : is back\n", output, data->A++, id);
    }
    else {
        setOutput("%d  : HACK %d  : waits  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);
    }
	if(data->PierProcesses >= 4 && ((data->NH == 4) || (data->NS >= 2 && data->NH == 2))){
		crewCaptain = 1;	                 //posledny kto vstupil na molo a utvoril skupinu sa stava kapitanom
		if(data->NH == 4)
		{	
			data->NH = data->NH - 4;
			
			for(int a = 0; a < 4; a++){
				sem_post(data->hackBoards);     //skupinu tvoria 4 hackeri, pustim na palubu
			}
			sem_post(data->Write);
			sem_post(data->semaphore);
		}    
		else{
			data->NH = data->NH - 2;
			data->NS = data->NS - 2;
			sem_post(data->semaphore);
			for(int a = 0; a < 2; a++){
				sem_post(data->hackBoards);       //skupinu tvoria 2 hackeri a 2 serfovia, pustim na palubu
				sem_post(data->serfBoards);
			}
			sem_post(data->Write);
			sem_post(data->semaphore);
		}	
	}
	else{
	    sem_post(data->Write);
	    sem_post(data->semaphore);
		sem_post(data->waits);   			//nebola vytvorena skupina, pustim dalsiu osobu
	}
	
	sem_wait(data->hackBoards);
	sem_wait(data->semaphore);
	sem_wait(data->Write);               //nalodenie
	data->boatCrew++;
	sem_post(data->Write);
	sem_post(data->semaphore);
	sem_wait(data->semaphore);

	if(data->boatCrew == 4){
		data->PierProcesses = data->PierProcesses-4;
		data->boatCrew = 0;                        // nalodia sa vsetci clenovia, otvori sa semafor
		
		for (int b = 0; b < 4; b++){
			sem_post(data->memberType);
		}
		sem_post(data->semaphore);
	}
	else{
		sem_post(data->semaphore);
	}
	
	sem_wait(data->memberType);
	sem_wait(data->semaphore);
	sem_wait(data->Write);
	data->processes++;

	if(crewCaptain == 1){							//rozdelenie hodnosti
        setOutput("%d  : HACK %d  : boards  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);
	    sem_post(data->Write);
	}
	else{
	    sem_post(data->Write);
	}

	sem_post(data->semaphore);
	sem_wait(data->semaphore);

	if(data->processes == 4){
		data->processes=0;
		
		for(int a =0; a <4; a++) {
			sem_post(data->boatCruise);
		}
		sem_post(data->semaphore);
	}
	else{
		sem_post(data->semaphore);
	}
	
	sem_wait(data->boatCruise);
	sem_wait(data->semaphore);

	if(crewCaptain == 1){
		if (args.R > 0){
		cruiseTime = (rand() % (args.R+1));    //simulovanie plavby uspanim procesu kapitana
		usleep(cruiseTime * 1000);
		}
		
		for(int c = 0; c < 4; c++){
			sem_post(data->memberExits);
		}
		sem_post(data->semaphore);
	}
	else{
		sem_post(data->semaphore);
	}
	
	sem_wait(data->memberExits);
	sem_wait(data->semaphore);
	sem_wait(data->Write);
	++data->hackExits;				//po skonceni plavby sa vsetky osoby vylodia
	++data->serfExits;
    if(crewCaptain == 1) {
        setOutput("%d  : HACK %d  : captain exits  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);
    }
    else{
        setOutput("%d  : HACK %d  : member exits  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);
    }
    sem_post(data->Write);
	sem_post(data->semaphore);
	sem_wait(data->semaphore);
	if (data->hackExits == 4){
		
		data->hackExits = 0;
		sem_post(data->semaphore);
		sem_post(data->waits);			// osoby vystupili z lode, povolim semafor a dalsie osoby mozu prist na molo
	}
	else{
	    sem_post(data->semaphore);
	}
	
	sem_wait(data->semaphore);
	if(data->serfExits == args.P*2){
		data->serfExits =0;
		for(int a =0; a< args.P*2; a++){	//pokial pocet prepravenych osob*2 je rovny poctu generovanych osob od kazde osoby, vsetky osoby su prepravene
			sem_post(data->CruiseDone);
		}
		sem_post(data->semaphore);
	}
	else{
		sem_post(data->semaphore);
	}
	
	sem_wait(data->CruiseDone);
	sem_wait(data->semaphore);
	sem_wait(data->Write);
	sem_post(data->Write);
	sem_post(data->semaphore);
	sem_wait(data->semaphore);
	data->HACK++;					//inkrementacia pocitadla ukoncenych procesov
	sem_post(data->semaphore);
	exit(0);
}

void serf(int id, TMemory *data, TArguments args, FILE *output){
	int crewCaptain =0;    //doba plavby
	int cruiseTime;
	sem_wait(data->semaphore);
	sem_wait(data->Write);

	setOutput("%d  : SERF %d  : starts\n", output, data->A++, id);

	sem_post(data->Write);
	sem_post(data->semaphore);
	sem_wait(data->waits);
	sem_wait(data->semaphore);
	sem_wait(data->Write);
	++data->NS;
	++data->PierProcesses;
    cruiseTime = (rand() % (args.C+1));
    if(data->PierProcesses > args.C){
        setOutput("%d  : SERF %d  : leaves queue  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);
        usleep(cruiseTime * 1000);
        setOutput("%d  : SERF %d  : is back\n", output, data->A++, id);
    }
    else {
        setOutput("%d  : HACK %d  : waits  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);
    }

	
	if(data->PierProcesses >= 4 && ((data->NS == 4) || (data->NH >= 2 && data->NS == 2))){
		crewCaptain = 1;						
		if(data->NS == 4){
			data->NS = data->NS - 4;
			for(int a = 0; a < 4; a++){
				sem_post(data->serfBoards);		
			}
				sem_post(data->Write);
			sem_post(data->semaphore);
		}
		else{
			data->NH = data->NH - 2;
			data->NS = data->NS - 2;
			sem_post(data->semaphore);
			for(int a = 0; a < 2; a++)
			{
				sem_post(data->serfBoards);
				sem_post(data->hackBoards);
			}
				sem_post(data->Write);
			    sem_post(data->semaphore);
		}	
	}
	else{
	    sem_post(data->Write);
	    sem_post(data->semaphore);
	    sem_post(data->waits);
	}	

	sem_wait(data->serfBoards);
	sem_wait(data->semaphore);
	sem_wait(data->Write);
	data->boatCrew++;
	sem_post(data->Write);
	sem_post(data->semaphore);
	
	sem_wait(data->semaphore);
	if(data->boatCrew == 4){
		data->PierProcesses = data->PierProcesses-4;
		data->boatCrew = 0;
		
		for(int b = 0; b < 4; b++)
		{
			sem_post(data->memberType);
		}
		sem_post(data->semaphore);
	}
	else{
		sem_post(data->semaphore);
	}
	
	sem_wait(data->memberType);
	sem_wait(data->semaphore);
	sem_wait(data->Write);
	data->processes++;
	if(crewCaptain == 1){
        setOutput("%d  : SERF %d  : boards  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);
		sem_post(data->Write);
	}
	else{
		sem_post(data->Write);
	}
	sem_post(data->semaphore);
	
	sem_wait(data->semaphore);
	if(data->processes == 4){
		data->processes=0;
		
		for(int a =0; a <4; a++)
		{
			sem_post(data->boatCruise);
		}
		sem_post(data->semaphore);
	}
	else{
		sem_post(data->semaphore);
	}
	
	sem_wait(data->boatCruise);
	sem_wait(data->semaphore);

	if(crewCaptain == 1){
		if (args.R > 0){
		cruiseTime = (rand() % (args.R+1));
		usleep(cruiseTime * 1000);
		}
		
		for(int c = 0; c < 4; c++){
			sem_post(data->memberExits);
		}
		sem_post(data->semaphore);
	}
	else{
	sem_post(data->semaphore);
	}
	
	sem_wait(data->memberExits);
	sem_wait(data->semaphore);
	sem_wait(data->Write);
	++data->hackExits;
	++data->serfExits;
    if(crewCaptain == 1){
        setOutput("%d  : SERF %d  : captain exits  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);

    }
    else{
        setOutput("%d  : SERF %d  : member exits  : %d  : %d\n", output, data->A++, id, data->NH, data->NS);

    }
    sem_post(data->Write);
	sem_post(data->semaphore);
	
	sem_wait(data->semaphore);
	if (data->hackExits == 4){
		data->hackExits = 0;
		sem_post(data->semaphore);
		sem_post(data->waits);
	}
	else{
	    sem_post(data->semaphore);
	}
	
	sem_wait(data->semaphore);
	if(data->serfExits == args.P*2){
		data->serfExits =0;
		for(int a =0; a< args.P*2; a++){
			sem_post(data->CruiseDone);
		}
		sem_post(data->semaphore);
	}
	else{
		sem_post(data->semaphore);
	}
	
	sem_wait(data->CruiseDone);
	sem_wait(data->semaphore);
	sem_wait(data->Write);
	sem_post(data->Write);
	sem_post(data->semaphore);
	sem_wait(data->semaphore);
	data->SERF++;
	sem_post(data->semaphore);
	exit(0);
}

//funkce pro vytvareni hackeru
void createHackers(TMemory *data, TArguments args, FILE *output){
	int newHack;

		for(int i = 1; i < args.P+1; i++){
		    newHack = (rand() % (args.H+1));    //nahodna hodnoty
		    if(newHack > 0){
					usleep(newHack * 1000);
		    }
		    pid_t hacker_process = fork();
		    if (hacker_process == 0){
                    hackProcess(i, data, args, output);
		    }
		}

		while(data->HACK - args.P != 0){
			wait(NULL);				//kontrolujem, ci vsetky procesy boli ukoncene
		}

	exit(0);
}

//funkce pro vytvareni serfu
void createSerfs(TMemory *data, TArguments args, FILE *output){
	int newSerf;
		for(int i = 1; i < args.P+1; i++){
		    newSerf = (rand() % (args.S+1));
		    if(newSerf > 0){ 
		        usleep(newSerf * 1000);
		    }
			pid_t serf_process = fork();
		    if (serf_process == 0){
		        serf(i, data, args, output);
		    }
		}
		while(data->SERF - args.P != 0){
			wait(NULL);
		}
	exit(0);
}

//hlavni proces
void parentProcess(TMemory *data, TArguments args, FILE *output){
	pid_t createHackerProcess = fork();
		if (createHackerProcess == 0){
            createHackers(data, args, output);	//proces vytvarenia hackerov
		}
		else if (createHackerProcess > 0){
			pid_t createSerfsProcess = fork();
			if (createSerfsProcess== 0){
                createSerfs(data, args, output);	//proces vytvarenia serfov
			}
			else if (createSerfsProcess > 0){
				wait(NULL);
			}
		  wait(NULL);
		}
}
//kontrola a nacteni parametru do struktury



int main(int argc, char *argv[]){
	TArguments args;
	if (parseArguments(argc, argv, &args) == 1){
			fprintf(stderr, "Zle argumenty!\n");
			return 1;
		}
	
	FILE *out;
	if ((out = fopen("proj2.out", "w+"))==NULL){
        fprintf(stderr, "Nepodarilo sa otvorit subor!\n");
        return 2;
	}
	
	int shm_fd;				//Otevorenie sdielanej pamate, pokial sa nepodarilo otvorit, pokusim sa uzavriet pamat
    if((shm_fd = shm_open("/xmasic00-sm", O_CREAT | O_EXCL | O_RDWR, 0644)) < 0){
		fprintf(stderr, "Nepodarilo sa otvorit zdielanu pamat!\n");
		shm_unlink("/xmasic00-sm");
		unlink_memsem(); 
		fclose(out);
		return 2;
	}	
    
	if(ftruncate(shm_fd, sizeof(TMemory)) < 0){
		fprintf(stderr, "Chyba(576)!\n");
		shm_unlink("/xmasic00-sm");
		close(shm_fd);
		fclose(out);
		return 2;
	}
	TMemory *structs;
		
	if((structs = mmap(NULL, sizeof(TMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)) == MAP_FAILED){
		fprintf(stderr, "Chyba(586)!\n");
		shm_unlink("/xmasic00-sm");
		close(shm_fd);
		fclose(out);
		return 2;
	}
	
	if(initSemaphores(structs) ==1){
		unlink_memsem();
		return 2;
	}
	
    srand(time(NULL));  //inicializacia seedov

    parentProcess(structs, args, out);
    
    fclose(out);
    munmap(structs, sizeof(TMemory));
    shm_unlink("/xmasic00-sm");
	unlink_memsem();   
    close(shm_fd);	
	
    return 0;
}
