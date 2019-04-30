#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

char *NaVelke(char *str)		//funkcia (toupper): prevod na velke pismena
{
	for (char *s = str; *s; ++s)
	{
		*s = toupper((unsigned char)*s);
	}
	return str;
}

int main (int argc, char *argv[])
{
	int adresa[100] = {0};
	char mesto[100][42];
	char vstup[42];
	char *enabled_list = argv[1] ? NaVelke(argv[1]) : "";	 //ak "*enable_list = argv[1]" potom vysledok je "NaVelkde(argv[1])" inak vysledok bude ""
	char enable[26+1] = {0};
	size_t len = strlen(enabled_list);
	int premenna;
	int pocitadlo = 0;

	if (argc>2)	//ak je zadanych viac ako jeden argument tak vypise "Vela Argumentov" a ukonci program
	{
		fprintf(stderr, "Vela argumentov\n" );
		return EXIT_FAILURE;
	}

	while(scanf("%s\n",vstup)!=EOF)			//nacita mesta z textoveho suboru a priradi ich do pola
	{
		premenna= NaVelke(vstup)[len] - 65;
		if (strncmp(vstup, enabled_list,len)==0) //strncmp porovnava stringy, ak sa rovnaju tak pokracuje
		{
			++adresa[premenna];
			strcpy(mesto[premenna],vstup);  //zkopiruje "vstup" do "mesto"
		}
	}
	if (strlen(vstup)==0)		//ak je dlzka stringu 0 ta vypise prazdny subor a ukonci program
	{
		fprintf(stderr,"prazdny subor\n");
		return EXIT_FAILURE;
	}

	for (int i = 0; i <100; i++)
	{
		if (adresa[i])
		{
			enable[pocitadlo++]= 65 + i;
		}
	}
	
	if (pocitadlo==0)	//ak sa premenna "pocitadlo" rovna 0 tak nenasiel ziadnu zhodu
	{
		printf("Not found\n");
	}
	else
	{
		premenna = enable[0] - 65;
		if (pocitadlo == 1 && adresa [premenna]==1)	//ak sa premenna "pocitadlo" && adresa rovna 1 tak vypise "mesto"
			printf("Found: %s\n", mesto[premenna]);
		else
			printf("Enable: %s\n", enable); //inac vypise enable

	}

}
