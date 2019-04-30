#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

double taylor_tan(double x, unsigned int n)
{
	double citatel[13] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604}; 		//prvýych13 citatelov 
	double menovatel[13] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};	//prv�ch 13 menovatelov
	double vysledok = 0.0;
	double xx = x*x;
	double mocninax = x; 
	 
	for (int i = 0; i<n; i++)  //opakuje az po  i=13
	{
		vysledok+= mocninax * citatel[i]/ menovatel[i];  //vysledok + mocninax * citatel[i] / menovatel[i]
		mocninax *= xx;
	}
	return vysledok;
}

double cfrac_tan(double x, unsigned int n) 	//funkcia na vypocet tan pomocou zlozeneho zlomku
{
	double zlomok = 1.0;
	double citatel = x*x;				//vypocet podla prveho zlomku
	double cislo;
	double vysledok;
	for (int i = n; i>=1; --i)   
	{
		cislo = (i + 1)*2-1;	
		zlomok= citatel / (cislo - zlomok);
	}
	vysledok = x / (1.0 - zlomok);
	return vysledok;	
	
}

double mattan(double x)	//funkcia na vypocet tan z matematickej knihovny
{
	double vysledok_x;
	vysledok_x = tan(x);
	return vysledok_x;
}


int  main(int argc, char *argv[])
{
	double vysledok_d;
	double vysledok_v;
	double n = 13;
		
	if (strcmp(argv[1], "--help")==0)  //ak sa argv[1] rovna --help tak vypise pomoc pre uzivatela ako pocitat priklad
	{
		printf("--tan porovnáva presnos vypoctu tangens uhlu Alfa\n -m vypocita vzdialenost od objektu a dlzku objektu\n -c nastavuje vysku meracieho zariadenia\n \n Chyby vo vstupe: \n ERROR 1: Hodnota uhla Alfa nebol v rozmedzi 0 < alfa <= 1.4\n ERROR 2: Bol zadaný neply pocet argumentov \n ERROR 3: Bola zadana neplatna vyska \n");
		return EXIT_SUCCESS;	//vracia success a ukonci funckiu
	}
	
	else if (strcmp(argv[1], "--tan") == 0)	//ak sa argv[1] rovna --tan tak vypise porovnania vysledkov tan 
	{ 
		if (argc > 2)  //ak je pocet argumentov vacsi ako 2 vypise porovnania na zadanych iteraciach
		{
			n = atoi(argv[3]);
			for (int i = n; i < atoi(argv[4]); i++)
			{
				printf("%.10e %.10e %.10e %.10e %.10e\n", taylor_tan(atof(argv[2]),n), mattan(atof(argv[2])), mattan(atof(argv[2])) - taylor_tan(atof(argv[2]),n), cfrac_tan (atof(argv[2]),n), 
mattan(atof(argv[2])) - cfrac_tan(atof(argv[2]),n));	
			}
		}
		else if (argc == 2){
			printf("%.10e %.10e %.10e %.10e %.10e\n", taylor_tan(atof(argv[2]),n), mattan(atof(argv[2])), mattan(atof(argv[2])) - taylor_tan(atof(argv[2]),n), cfrac_tan(atof(argv[2]),n), mattan(atof(argv[2])) - 
cfrac_tan(atof(argv[2]),n));	//ak je pocet argumentov rovny 2 tak vypise porovnanie na poslednej iteracii
		}
	}
	else if (strcmp(argv[1],"-c")==0)	//ak sa argv[1] rovna -c tak zacne pocitat vzdialenost alebo dlzku
	{
		double c;
		if(atof(argv[4]) > 1.4 && atof(argv[4])<= 0) 	//ak je uhol alfa vacsi ako 1,4 a mensi ako 0 tak vyhodi error
		{
			printf("Bola zadaná neplatná hodnota uhla\n");
			return EXIT_FAILURE;
		}
		if (strcmp(argv[1],"-c")==0)	//ak je argv[1] rovne -c tak priradi novu hondotu do "c" z argv[2]
		{
		 c = atof(argv[2]);
		}
		vysledok_v = c  / cfrac_tan(atof(argv[4]),n); //vypocet vzdialenosti
		printf("%.10e\n",vysledok_v);
		if (argc > 4)
		{
			vysledok_d = c + vysledok_v * cfrac_tan(atof(argv[5]),n);
			printf("%.10e\n",vysledok_d);
		}

		return EXIT_SUCCESS;
		
	}
	else if (strcmp(argv[1],"-m")==0)		//ak sa argument rovna -m tak vypocita vzdialenost objektu
	{
		if (atof(argv[2]) > 1.4 && atof(argv[2]) <= 0)
		{
			printf("Bola zadaná neplatná hodnota uhla. Pre viac info použi --help\n");	//ak je zadana neplatna hodnota uhla vypise error
			return EXIT_FAILURE;
		} 
		
		double vyska = 1.5;
		vysledok_v = vyska / cfrac_tan(atof(argv[2]),n);		//vypocet vzdialenosti
		printf("%.10e\n", vysledok_v);
		if (argc > 2)	
		{
			double vyska = 1.5;					//vypocet dlzky
			vysledok_d = vyska + vysledok_v * cfrac_tan(atof(argv[3]),n);
			printf("%.10e\n",vysledok_d);
		}
		
		return EXIT_SUCCESS;
		 
	}

	return 0;
}
