/**
 * Kostra programu pro 3. projekt IZP 2017/18
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 * https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 * @author Nikolas Masica <xmasic00>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>
#include <ctype.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->obj = malloc(cap * sizeof(struct obj_t));  //alokacia pamate pre pole
    c->capacity = cap;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);       //uvolnenie pamate
    c->size = 0;        //npriradenie size 0
    c->capacity = 0;    //priradenie capacity 0
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if (c->size == c->capacity)
    {
        resize_cluster(c, c->capacity + CLUSTER_CHUNK); //rozsirenie kapacity clustera
    }
    c->obj[c->size] = obj;    //pridanie objektu na volnu poziciu na konci clustera
    c->size++;   //zvysenie poctu prvkov v clusteri

}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    for (int i = 0; i < c2->size; i++)
    {
        append_cluster(c1, c2->obj[i]);   // //prida objekty c2 do c1
    }
    sort_cluster(c1);   // zoradi objekty v c1;
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]); // uvolnenie pamate pre cluster s indexom idx

    for (int i = idx; i < narr - 1; i++) {
        carr[i] = carr[i + 1]; //narr +1 => velkost bude o 1 vacsia //posunie cluster v poli smerom doprava
    }

    return narr - 1; //odstranil sa cluster tak narr -1
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    return sqrtf((o1->x - o2->x) * (o1->x - o2->x) + (o1->y - o2->y) * (o1->y - o2->y));
    //vypocet Euklidovej vzdialenosti medzi objektami o1,o2
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float distance = 0;
    int num_repeat = 0;
    for (int i = 0; i < c1->size; i++)              //pocitanie vzdialenosti objektov z clustera c1 a c2
    {
        for (int j = 0; j < c2->size; j++)
        {
            distance += obj_distance(&c1->obj[i], &c2->obj[j]);  //vypocet priemernej vzdialenosti
            num_repeat++;
        }
    }

    return distance / num_repeat;
}



/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float distance;
    float nearest_distance = INT_MAX;

    for (int i = 0; i < narr; ++i)              //pocitanie vzdialenosti medzi clustermi navzajom
    {
        for (int j = 0; j < narr; ++j)
        {
            if(i !=j)
            {
                distance = cluster_distance(&carr[i], &carr[j]);
                if (distance < nearest_distance)    //hladanie minimalnej vzdialenosti
                {
                    nearest_distance = distance;
                    *c1 = i;
                    *c2 = j;
                }
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}


/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *pFile;
    int loaded_clusters;
    pFile = fopen(filename, "r"); //otvorenie suboru na citanie
    fscanf(pFile, "count=%d", &loaded_clusters);
    int id;
    float x, y;
    int loaded_lines = 0;
    if(loaded_clusters <= 0 || loaded_clusters - (int)loaded_clusters != 0)  //osetrenie ci bol zadany spravny count v subore Count > 0
    {
        fprintf(stderr,"Bol zadany nespravny Count\n");
        fclose(pFile);
        return -1;
    }

    if (pFile == NULL)
    {
        fprintf(stderr,"Subor sa nepodarilo otvorit\n");
        fclose(pFile);
        return -1;
    }

    *arr = (struct cluster_t *) malloc(loaded_clusters * sizeof(struct cluster_t));  //alokacia pamate

    for (int i = 0; i < loaded_clusters; i++)
    {
        init_cluster(&(*arr)[i], 1);
        (*arr)[i].size = 1;   // nastavenie pociatocnej velkosti clustera
        loaded_lines++;

        if (loaded_lines < 0)   //ak je pocet nacitanych riadkov 0 tak je subor prazdny -> error
        {
            fprintf(stderr, "Subor je prazdny\n");
            fclose(pFile);
            return -1;
        }

        fscanf(pFile, "%d %f %f", &id, &x, &y);
            if (x > 1000 || x < 0 || y > 1000 || y < 0)  //osetrenie na povoleny limit suradnic
            {
                fprintf(stderr, "Suradnice su mimo povoleny limit\n");
                fclose(pFile);
                *arr = NULL;
                return -1;
            }

            if (id > INT_MAX)
            {
                fprintf(stderr,"Zadane nespravne ID\n");
                fclose(pFile);
                *arr = NULL;
                return -1;
            }
            if (x - (int)x != 0 || y - (int)y != 0 || id - (int)id != 0)  //osetrenie ci suradnice su celociselne
            {
                fprintf(stderr,"Boli zadane nespravne suradnice alebo ID objektu\n");
                fclose(pFile);
                *arr = NULL;
                return -1;
            }
            if ((*arr)[i].obj == NULL)   //ak sa *arr rovna NULL ->error
            {
                fprintf(stderr, "Alokacia pamate sa nepodarila\n");
                fclose(pFile);
                *arr = NULL;
                return -1;
            }

            for (int j = 0; j < loaded_lines - 1; j++)   // osetrenie na objekty s rovnakym ID
            {
                if ((*arr)[j].obj->id == id) {
                    fprintf(stderr, "V subore sa nachadzaju objekty s rovnakym ID\n");
                    fclose(pFile);
                    *arr = NULL;
                    return -1;
                }
            }

            (*arr)[i].obj->id = id;     //priradenie ID objektu
            (*arr)[i].obj->x = x;   //priradenie X-sovej suradnice
            (*arr)[i].obj->y = y;      //priradenie Y-ovej suradnice
    }
    fclose(pFile);//uzavretie suboru
    return loaded_lines;   // vracia pocet nacitanych clusterov
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[]) {
    struct cluster_t *clusters;
    int input_clusters;
    int start_clusters = load_clusters(argv[1], &clusters);  // pocet clusterov na zaciatku
    int c1, c2;

    if (argc >= 4) {
        fprintf(stderr, "Nespravny pocet argumentov\n");
        return EXIT_FAILURE;
    }

    if (atoi(argv[2]) == 0)
    {
        fprintf(stderr,"Nespravne zadany argument N\n");
        return EXIT_FAILURE;
    }

    if (argc == 2) //ak nebol zadany argument N tak mu priradi hodnotu 1
    {
        input_clusters = 1;
    } else {
        input_clusters = atoi(argv[2]); //priradenie hodnoty // argv[2] => N, pocet vyslednych clusterov
        if (isdigit(*argv[2]) == 0)     //ak N nie je integer ->error
        {
            fprintf(stderr, "Nespravne zadany argument N\n");
            return EXIT_FAILURE;
        }
    }

    if (start_clusters == -1)   //ak load_cluster vracia -1 -> ukonci program
    {
        return EXIT_FAILURE;
    }

    if (input_clusters < 0) //ak je hodnota zadaneho N mensia 0 -> error
    {
        fprintf(stderr, "Bol zadany nespravny pocet clusterov. ([N] > 0)\n");
        return EXIT_FAILURE;
    }

    if (input_clusters > start_clusters)        //ak je N vacsie ako pocet objektov v subore -> error
    {
        fprintf(stderr, "Zadany argument N je vacsi ako pocet objektov v subore alebo sa nezhoduje zadany Count a pocet objektov\n");
        return EXIT_FAILURE;
    }

    while (start_clusters > input_clusters)        //pokial start_cluster nie je N tak rob
    {
        find_neighbours(clusters, start_clusters, &c1, &c2);
        merge_clusters(&clusters[c1], &clusters[c2]);
        start_clusters = remove_cluster(clusters, start_clusters, c2);
    }

    print_clusters(clusters, start_clusters);

    for (int i = 0; i < start_clusters; ++i)        //uvolnenie pamate
    {
        clear_cluster(&clusters[i]);
    }

    free(clusters);
    return 0;
}