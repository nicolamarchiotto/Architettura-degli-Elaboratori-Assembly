#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

/* Inserite eventuali extern modules qui */

/* ************************************* */

enum { MAXLINES = 400 };
enum { LIN_LEN = 15 };
enum { LOUT_LEN = 8 };

long long current_timestamp() {
    struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	/* te.tv_nsec nanoseconds divide by 1000 to get microseconds*/
	long long nanoseconds = tp.tv_sec*1000LL + tp.tv_nsec; // caculate nanoseconds
    return nanoseconds;
}


int main(int argc, char *argv[]) {
    int i = 0;
    char bufferin[MAXLINES*LIN_LEN+1] ;
    char line[1024];
    long long tic_c, toc_c, tic_asm, toc_asm;

    char bufferout_c[MAXLINES*LOUT_LEN+1] = "" ;
    char bufferout_asm[MAXLINES*LOUT_LEN+1] = "" ;

    FILE *inputFile = fopen(argv[1], "r");

    if(argc != 3)
    {
        fprintf(stderr, "Syntax ./test <input_file> <output_file>\n");
        exit(1);
    }

    if (inputFile == 0)
    {
        fprintf(stderr, "failed to open the input file. Syntax ./test <input_file> <output_file>\n");
        exit(1);
    }

    while (i < MAXLINES && fgets(line, sizeof(line), inputFile))
    {
        i = i + 1;
        strcat( bufferin, line) ;
    }

    bufferin[MAXLINES*LIN_LEN] = '\0' ;

    fclose(inputFile);
	
	

    /* ELABORAZIONE in C */
    tic_c = current_timestamp();

    /* è possibile inserire qui il codice per l'elaborazione in C (non richiesto per l'elaborato) */
    /* questo pezzo di codice è solo una base di partenza per fare dei check sui dati */
    /*
    int c = 0;
    char tmpout[LOUT_LEN];
    strcpy( tmpout, "000-00\n");
    while ( bufferin[c] != '\0') {
      printf( "%d\n", bufferin[c]);
      strcat( bufferout_asm, tmpout);
      c = c + LIN_LEN ;
    }
    */
    
    int res_gen, res_dw, res_wm;
	int int_gen, int_dw, int_wm;
	char th1='0', th2='0';
	int vet[10];

	int ol=0;
	int l=0;
	int cont;
	int watt=0;
	int acceso=0;
	char strsup[8];

	do
	{
		res_gen=bufferin[l]-48;
		res_dw=bufferin[l+1]-48;	
		res_wm=bufferin[l+2]-48;
		l=l+4;
		
		for(cont=0; cont<10;cont++,l++)
		{
			vet[cont]=bufferin[l]-48;
		}
		l++;
		
		if((res_gen==1) && (acceso==0))
		{
			acceso=1;
			int_gen=1;
			int_dw=1;
			int_wm=1;
		}
		
		if(acceso==1)
		{
			if(res_dw==1)
				int_dw=1;
			
			if(res_wm==1)
				res_wm=1;
				
			watt=0;
			
			if(vet[0]==1)
				watt=watt+2000;
			if(vet[1]==1)
				watt=watt+300;
			if(vet[2]==1)
				watt=watt+1200;
			if(vet[3]==1)
				watt=watt+1000;
			if(vet[6]==1)
				watt=watt+240;
			if(vet[7]==1)
				watt=watt+400;
			if(vet[8]==1)
				watt=watt+200;
			if(vet[9]==1)
				watt=watt+400;
				
			//casi speciali
			if(vet[4]==1)
			{
				if(int_dw==1)
					watt=watt+2000;
			}
			
			if(vet[5]==1)
			{
				if(int_wm==1)
					watt=watt+1800;
			}
			
			
			//calcolo quanti cicli in overload
			if(watt>4500)
			{
				ol++;
			}
			else
			{
				ol=0;
			}
			
			//calcolo fascia
			if(watt<=1500)
			{
				th1='F';
				th2='1';
			}
			else
			{
				if(watt<=3000)
				{
					th1='F';
					th2='2';
				}
				else
				{
					if(watt<=4500)
					{
						th1='F';
						th2='3';
					}
					else
					{
						th1='O';
						th2='L';
					}
				}			
			}
			
			if(ol==4)
			{
				int_dw=0;
			}
			
			if(ol==5)
			{	
				int_dw=0;
				int_wm=0;
			}
			
			if(ol==6)
			{	
				int_dw=0;
				int_wm=0;
				int_gen=0;
				th1='0';
				th2='0';
				acceso=0;
				ol=0;
			}			
		}
		
		strsup[0]=int_gen+48;
		strsup[1]=int_dw+48;
		strsup[2]=int_wm+48;
		strsup[3]='-';
		strsup[4]=th1;
		strsup[5]=th2;
		strsup[6]='\n';
		
		strcat(bufferout_c, strsup);	
}
while(bufferin[l]!='\0');



    toc_c = current_timestamp();

  	long long c_time_in_nanos = toc_c - tic_c;
  	

    /* FINE ELABORAZIONE C */


    /* INIZIO ELABORAZIONE ASM */

    tic_asm = current_timestamp();

    /* Assembly inline:
    Inserite qui il vostro blocco di codice assembly inline o richiamo a funzioni assembly.
    Il blocco di codice prende come input 'bufferin' e deve restituire una variabile stringa 'bufferout_asm' che verrà poi salvata su file. */
    
__asm__(

"STATO_0:"
	"movl $0, %%eax;"	//registro per il conteggio del carico
	"movl $0, %%ebx;"	//registro per il conteggio dei cicli in overload 

"INIZIO:"
    "cmpb $49, (%[in]);"
    "jne SPENTO;"
	
"ACCENDI:"
	"movb $49, (%[out]);"	//49 = codifica 1
	"movb $49, 1(%[out]);"
	"movb $49, 2(%[out]);"
	"movb $45, 3(%[out]);"	//45 = codifica -
	
//calcolo carico

"CALCOLA_CARICO:"

	"movl $0, %%eax;"
	"cmpb $49, 4(%[in]);"
	"je SOMMA_1;"
"SUM_2:"
	"cmpb $49, 5(%[in]);"
	"je SOMMA_2;"
"SUM_3:"
	"cmpb $49, 6(%[in]);"
	"je SOMMA_3;"
"SUM_4:"
	"cmpb $49, 7(%[in]);"
	"je SOMMA_4;"
"SUM_DW:"
	"cmpb $49, 8(%[in]);"
	"je SOMMA_DW;"
"SUM_WM:"
	"cmpb $49, 9(%[in]);"
	"je SOMMA_WM;"
"SUM_7:"
	"cmpb $49, 10(%[in]);"
	"je SOMMA_7;"
"SUM_8:"
	"cmpb $49, 11(%[in]);"
	"je SOMMA_8;"
"SUM_9:"
	"cmpb $49, 12(%[in]);"
	"je SOMMA_9;"
"SUM_10:"
	"cmpb $49, 13(%[in]);"
	"je SOMMA_10;"
"jmp CALCOLA_FASCIA;"
	
"SOMMA_1:"
	"addl $2000, %%eax;"
	"jmp SUM_2;"
"SOMMA_2:"
	"addl $300, %%eax;"
	"jmp SUM_3;"
"SOMMA_3:"
	"addl $1200, %%eax;"
	"jmp SUM_4;"
"SOMMA_4:"
	"addl $1000, %%eax;"
	"jmp SUM_DW;"
	
"SOMMA_DW:"
	"cmpb $49, 1(%[out]);"
	"jne SUM_WM;"
"REAL_SUM_DW:"
	"addl $2000, %%eax;"
	"jmp SUM_WM;"
	
"SOMMA_WM:"
	"cmpb $49, 2(%[out]);"
	"jne SUM_7;"
"REAL_SUM_WM:"
	"addl $1800, %%eax;"
	"jmp SUM_7;"
	
"SOMMA_7:"
	"addl $240, %%eax;"
	"jmp SUM_8;"
"SOMMA_8:"
	"addl $400, %%eax;"
	"jmp SUM_9;"
"SOMMA_9:"
	"addl $200, %%eax;"
	"jmp SUM_10;"
"SOMMA_10:"
	"addl $400, %%eax;"
	
//CALCOLO FASCIA

"CALCOLA_FASCIA:"
	"cmpl $1500, %%eax;"
	"jle F1;"
	"cmpl $3000, %%eax;"
	"jle F2;"
	"cmpl $4500, %%eax;"
	"jle F3;"
	"movb $79, 4(%[out]);"	//codifica O
	"movb $76, 5(%[out]);"	//codifica L
	"movb $10, 6(%[out]);"	//codifica \n
	"addl $1, %%ebx;"
	"jmp CONTROLLO_CICLI_OL;"

"F1:"
	"movb $70, 4(%[out]);"	//codifica F
	"movb $49, 5(%[out]);"	//codifica 1
	"movb $10, 6(%[out]);"	//codifica \n
	"movl $0, %%ebx;"
	"jmp CONTROLLO_CICLI_OL;"
	
"F2:"
	"movb $70, 4(%[out]);"	//codifica F
	"movb $50, 5(%[out]);"	//codifica 2
	"movb $10, 6(%[out]);"	//codifica \n
	"movl $0, %%ebx;"
	"jmp CONTROLLO_CICLI_OL;"
	
"F3:"
	"movb $70, 4(%[out]);"	//codifica F
	"movb $51, 5(%[out]);"	//codifica 3
	"movb $10, 6(%[out]);"	//codifica \n
	"movl $0, %%ebx;"

//CONTROLLO CICLI IN OVERLOAD

"CONTROLLO_CICLI_OL:"
	"cmpl $4, %%ebx;"
	"je QUARTO_CICLO_OL;"
	"cmpl $5, %%ebx;"
	"je QUINTO_CICLO_OL;"
	"cmpl $6, %%ebx;"
	"je SESTO_CICLO_OL;"

	
"NEW_LINE_S:"
	"cmpb $0, 15(%[in]);"	//controlla se siamo all'ultimo input
	"je FINE;"
	"addl $7, %[out];"		//new line out
	"addl $15, %[in];"		//new line in
	//SCRIVO INT-GEN A 1, IL IL TRATTINO, E LO \N PER IL NUOVO OUTPUT
	"movb $49, (%[out]);"	//49 = codifica 1
	"movb $45, 3(%[out]);"	//45 = codifica -
	"movb $10, 6(%[out]);"	//10 = codifica \n
	
//RES DW

"CTRL_RES_DW:"
	"cmpb $49, 1(%[in]);"
	"jne SCRIVI_DW_PREC;"
	"movb $49, 1(%[out]);"
	
//RES WM
	
"CTRL_RES_WM:"
	"cmpb $49, 2(%[in]);"
	"jne SCRIVI_WM_PREC;"
	"movb $49, 2(%[out]);"
	"jmp CALCOLA_CARICO;"

//4 CICLO IN OL
"QUARTO_CICLO_OL:"
	"movb $48, 1(%[out]);"
	"jmp NEW_LINE_S;"

//5 CICLO IN OL
"QUINTO_CICLO_OL:"
	"movb $48, 1(%[out]);"
	"movb $48, 2(%[out]);"
	"jmp NEW_LINE_S;"

//SESTO CICLO OL
"SESTO_CICLO_OL:"	
	"movl $0, %%ebx;"
	"movb $48, (%[out]);"	//48 = codifica 0
	"movb $48, 1(%[out]);"
	"movb $48, 2(%[out]);"
	"movb $45, 3(%[out]);"	//45 = codifica -
	"movb $48, 4(%[out]);"
	"movb $48, 5(%[out]);"
	"movb $10, 6(%[out]);"	//10 = codifica \n
	"jmp NEW_LINE;"

"SCRIVI_DW_PREC:"
	"movb -6(%[out]), %%cl;"
	"movb %%cl, 1(%[out]);"
	"jmp CTRL_RES_WM;"
	
"SCRIVI_WM_PREC:"
	"movb -5(%[out]), %%cl;"
	"movb %%cl, 2(%[out]);"
	"jmp CALCOLA_CARICO;"
	
//SCRIVO IN OUTPUT 000-00

"SPENTO:"
	"movb $48, (%[out]);"	//48 = codifica 0
	"movb $48, 1(%[out]);"
	"movb $48, 2(%[out]);"
	"movb $45, 3(%[out]);"	//45 = codifica -
	"movb $48, 4(%[out]);"
	"movb $48, 5(%[out]);"
	"movb $10, 6(%[out]);"	//10 = codifica \n
	
"NEW_LINE:"
	"cmpb $0, 15(%[in]);"
	"je FINE;"
	"addl $7, %[out];"		//new line out
	"addl $15, %[in];"		//new line in
	"jmp INIZIO;"
	
"FINE:"
	
	
	
    
    
    
    ://output
    :[in]"r"(bufferin), [out]"r"(bufferout_asm)
    :"eax", "ebx", "ecx"
);
    

    toc_asm = current_timestamp();

  	long long asm_time_in_nanos = toc_asm - tic_asm;

    /* FINE ELABORAZIONE ASM */


    printf("C time elapsed: %lld ns\n", c_time_in_nanos);
    printf("ASM time elapsed: %lld ns\n", asm_time_in_nanos);

    /* Salvataggio dei risultati ASM */
  	FILE *outputFile;
    outputFile = fopen (argv[2], "w");
    fprintf (outputFile, "%s", bufferout_asm);
    fclose (outputFile);

    return 0;
}
