
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,◄
**      DLDisposeList ... zrušení všech prvků seznamu,◄ ☼
**      DLInsertFirst ... vložení prvku na začátek seznamu,◄ ☼
**      DLInsertLast .... vložení prvku na konec seznamu,◄
**      DLFirst ......... nastavení aktivity na první prvek,◄
**      DLLast .......... nastavení aktivity na poslední prvek,◄
**      DLCopyFirst ..... vrací hodnotu prvního prvku,◄
**      DLCopyLast ...... vrací hodnotu posledního prvku,◄
**      DLDeleteFirst ... zruší první prvek seznamu,◄
**      DLDeleteLast .... zruší poslední prvek seznamu,◄
**      DLPostDelete .... ruší prvek za aktivním prvkem,◄
**      DLPreDelete ..... ruší prvek před aktivním prvkem,◄
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,◄
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,◄
**      DLCopy .......... vrací hodnotu aktivního prvku,◄
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,◄
**      DLSucc .......... posune aktivitu na další prvek seznamu,◄
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,◄
**      DLActive ........ zjišťuje aktivitu seznamu.◄
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int errflg;
int solved;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
    L->Act = L->First;      //prvy prvok bude aktivny

    while(L->Act != NULL) {
        L->First = L->Act->rptr;    //nasledujuci prvok bude aktivny
        free(L->Act);       //uvolni aktivny prvok
        L->Act = L->First;      //novy prvy prvok bude aktivny
    }

    L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr tmpr = malloc(sizeof(struct tDLElem));

    if(tmpr == NULL){   //ak zlyha alokacia vola Error
        DLError();
        return;
    }

    tmpr->data = val;
    tmpr->lptr = NULL;
    tmpr->rptr = L->First;

    if(L->First == NULL){
        L->Last = tmpr;      //ak zoznam je prazdny novy prvy prvok bude aj poslednym
    }else{
        L->First->lptr = tmpr;
    }

    L->First = tmpr;
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr tmpr = malloc(sizeof(struct tDLElem));

    if(tmpr == NULL){   //ak zlyha alokacia vola Error
        DLError();
        return;
    }
    tmpr->data = val;
    tmpr->rptr = NULL;
    tmpr->lptr = L->Last;

    if(L->First == NULL){
        L->First = tmpr;     //ak zoznam je prazdny novy posledny prvok bude aj prvym
    }else{ L->Last->rptr = tmpr;
    }

    L->Last = tmpr;
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First;

}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->Last;

}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if(L->First == NULL){
        DLError();
        return;
    }

    *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if(L->First == NULL){
        DLError();
        return;
    }

    *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if(L->First == NULL){       //ak je zoznam prazdny tak funkcia nerobi nic
        return;
    }

    tDLElemPtr tmpr = L->First;      //do pomocnej premennej ulozi adresu prveho prvku

    if(L->Act == L->First){
        L->Act = NULL;      //ak je prvy prvok aktivny tak aktivita sa strati
    }

    if(L->First == L->Last){    //ak je prvy prvok zaroven aj poslednym
        L->First = NULL;
        L->Last = NULL;
    }else{
        L->First = L->First->rptr;  //prvy prvok bude nasledujuci prvok
        L->First->lptr = NULL;
    }

    free(tmpr);      //povodny prvok zanika a uvolnuje sa pamät
}

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if(L->First == NULL){
        return;
    }

    tDLElemPtr tmpr = L->Last;  //do pomocnej premennej ulozi posledny prvok

    if(L->Act == L->Last){
        L->Act = NULL;
    }

    if(L->First == L->Last){
        L->First = NULL;
        L->Last = NULL;
    }else{
        L->Last = L->Last->lptr;        //poslednym prvok sa stava predchadzajuci prvok
        L->Last->rptr = NULL;
    }

    free(tmpr);      //povodny prvok zanika a ulovni sa pamät
}


void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    if((L->Act == NULL) || (L->Act == L->Last)){
        return;
    }

    tDLElemPtr tmpr = L->Act->rptr;      //do pomocnej premennej sa vlozi prvok za aktivnym prvkom

    L->Act->rptr = tmpr->rptr;

    if(tmpr == L->Last){
        L->Last = L->Act;
    }else{
        tmpr->rptr->lptr = L->Act;
    }

    free(tmpr);
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
    if ((L->Act == NULL) || (L->Act == L->First)){
        return;
    }

    tDLElemPtr tmpr = L->Act->lptr;  //do pomocnej premennej volzi prvok pred aktivnym prvokm

    L->Act->lptr = tmpr->lptr;       //ako predchadzajuci prvok sa nastavi prvok pred tmp

    if (tmpr == L->First) {
        L->First = L->Act;
    }else{
        tmpr->lptr->rptr = L->Act;
    }

    free(tmpr);
}



void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if(L->Act == NULL){
        return;
    }

    tDLElemPtr tmpr = malloc(sizeof(struct tDLElem));

    if(tmpr == NULL){
        DLError();
        return;
    }
    tmpr->lptr = L->Act;
    tmpr->data = val;
    tmpr->rptr = L->Act->rptr;
    L->Act->rptr = tmpr;

    if (L->Act == L->Last){
        L->Last = tmpr;
    }else{
        tmpr->rptr->lptr = tmpr;
    }
}


void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if(L->Act == NULL){
        return;
    }

    tDLElemPtr tmpr = malloc(sizeof(struct tDLElem));

    if(tmpr == NULL){
        DLError();
    }

    tmpr->rptr = L->Act;
    tmpr->data = val;
    tmpr->lptr = L->Act->lptr;
    L->Act->lptr = tmpr;

    if(L->Act == L->First){
        L->First = tmpr;
    }else{
        tmpr->lptr->rptr = tmpr;
    }
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
    if(L->Act == NULL){
        DLError();
        return;
    }

    *val = L->Act->data;
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    if (L->Act != NULL){
        L->Act->data = val;
    }
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
    if(L->Act == NULL){
        return;
    }

    if(L->Act == L->Last){
        L->Act = NULL;
    }else{
        L->Act = L->Act->rptr;
    }
}

void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
    if (L->Act == NULL) {
        return;
    }

    if (L->Act == L->First) {
        L->Act = NULL;
    } else {
        L->Act = L->Act->lptr;
    }
}
int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
   return L->Act != NULL;       //vracia 1 ak je zoznam aktivny ak L->Act == NULL tak vrati 0
}

/* Konec c206.c*/
