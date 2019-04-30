/******************* err_codes.h *******************\
|***************************************************|
|team: 053/1                                        |
|author: Martin Gaňo, xganom00                      |
|FIT VUT                                            |
|***************************************************|
|***************************************************/

/*
V module definujeme chybove kody zadane v zadani projektu,
v komentaroch vysvetlujeme ich vyznam a kedy ich volat.
*/

#define OK 0  //program zbehol OK

#define SCNR_ERR  1  //chyba pri lexikalnej analyze
#define PRSR_ERR  2  //chyba pri syntaktickej analyze

/*znacka - semanticke chyby*/
#define SEM_ERR_UNDEF 3 //nedefinovana funkcia alebo premenna alebo pokus o ich redefiniciu
#define SEM_ERR_TYPE 4 //chyba typovej kompatibility
#define SEM_ERR_PARAM 5 //zly pocet argumentov

#define SEM_ERR_ALL 6 //semanticke chyby okrem tychto 3 vyssie - od znacky "znacka"

#define RUN_ERR_ZERODIV 9 //delenie nulou

#define INTERN_ERR 99 //chyba v prekladaci napr nie je dost pamate a tak dalej
