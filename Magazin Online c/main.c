#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_COS 100
#define MAX_MAGAZIN 100
#define MAX_CHAR 100
#define MAX_TVAS 100
#define DEFAULT_COLOR 15

int numar_elemente_in_cos = 0;
int numar_elemente_in_magazin = 0;
int numar_elemente_in_tvas = 0;

struct produs {
    char denumire[MAX_CHAR];
    char categorie[MAX_CHAR];
    int cantitate;
    double pret;
} cos[MAX_COS], magazin[MAX_MAGAZIN];

struct TVA {
    char categorie[MAX_CHAR];
    int procentaj;
} tvas[MAX_TVAS];

/// FUNCTII HELPER
int maxim(int a, int b) {
    return a > b ? a : b;
}

int minim(int a, int b) {
    return a < b ? a : b;
}

void set_color(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

int get_console_width() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

void indent_for_mid(int spaces) {
    int console_width = get_console_width();
    int starting_point = (console_width - spaces) / 2;
    printf("%*c", starting_point, ' ');
}

void display_header(char* header) {
    char* pre = "-=== ";
    char* post = " ===-";
    indent_for_mid(strlen(pre) + strlen(header) + strlen(post));
    printf("%s", pre);
    set_color(FOREGROUND_RED);
    printf("%s", header);
    set_color(DEFAULT_COLOR);
    printf("%s\n", post);
}

double aplicare_tva(double pret_initial, int procentaj_tva) {
    return pret_initial * (1 + procentaj_tva / 100.0);
}
/// END FUNCTII HELPER

/// TVA
void citire_tva() {
    FILE* tva_in = fopen("tva.txt", "r");
    if (tva_in == NULL) {
        printf("S-a produs o eroare, nu se pot accesa fisierele necesare!\nNu se poate citi (tva.txt)!");
        exit(1);
    }

    char categorie[MAX_CHAR], clear_buffer[MAX_CHAR];
    int procentaj;
    while (fscanf(tva_in, "%[^\n]", categorie) != EOF) {
        fscanf(tva_in, "%d", &procentaj);
        fgets(clear_buffer, MAX_CHAR, tva_in);

        strcpy(tvas[numar_elemente_in_tvas].categorie, categorie);
        tvas[numar_elemente_in_tvas].procentaj = procentaj;
        numar_elemente_in_tvas++;
    }

    fclose(tva_in);
}
/// END TVA

/// MAGAZIN
void scriere_stoc() {
    FILE* stoc = fopen("stoc.txt", "w");
    if (stoc == NULL) {
        printf("S-a produs o eroare, nu se pot accesa fisierele necesare!\nNu se poate scrie (stoc.txt)!");
        exit(1);
    }

    int i;
    for (i = 0; i < numar_elemente_in_magazin; i++) {
        fprintf(stoc, "%s\n%s\n%d %.2lf\n",
                magazin[i].denumire,
                magazin[i].categorie,
                magazin[i].cantitate,
                magazin[i].pret);
    }

    fclose(stoc);
}

void citire_stoc() {
    FILE* stoc = fopen("stoc.txt", "r");
    if (stoc == NULL) {
        printf("S-a produs o eroare, nu se pot accesa fisierele necesare!\nNu se poate citi (stoc.txt)!");
        exit(1);
    }

    char denumire[MAX_CHAR], categorie[MAX_CHAR], clear_buffer[MAX_CHAR];
    int cantitate; double pret;
    while (fscanf(stoc, "%[^\n]", denumire) != EOF) {
        fgets(clear_buffer, MAX_CHAR, stoc);
        fscanf(stoc, "%[^\n]", categorie);
        fscanf(stoc, "%d%lf", &cantitate, &pret);
        fgets(clear_buffer, MAX_CHAR, stoc);

        strcpy(magazin[numar_elemente_in_magazin].denumire, denumire);
        strcpy(magazin[numar_elemente_in_magazin].categorie, categorie);
        magazin[numar_elemente_in_magazin].cantitate = cantitate;
        magazin[numar_elemente_in_magazin].pret = pret;
        numar_elemente_in_magazin++;
    }

    fclose(stoc);
}

void afisare_produs_curent(int i, int optiune_curenta) {
    if (i == optiune_curenta) {
        set_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf(">> ");
        set_color(DEFAULT_COLOR);
    }

    set_color(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf("%15s ", magazin[i].denumire);
    set_color(DEFAULT_COLOR);

    printf("%10s ", magazin[i].categorie);

    set_color(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("%4d ", magazin[i].cantitate);
    set_color(DEFAULT_COLOR);
    printf(" produse ");

    int procentaj_tva_curent = 0;
    for (int t = 0; t < numar_elemente_in_tvas; t++) {
        if (strcmp(tvas[t].categorie, magazin[i].categorie) == 0) {
            procentaj_tva_curent = tvas[t].procentaj;
        }
    }

    set_color(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("%6.2lf", aplicare_tva(magazin[i].pret, procentaj_tva_curent));
    set_color(DEFAULT_COLOR);
    printf(" RON");

    if (i == optiune_curenta) {
        set_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf(" <<\n");
        set_color(DEFAULT_COLOR);
    } else {
        printf("\n");
    }
}

void vizualizare_magazin() {
    int input, optiune_curenta = 0;
    char filtru_categorie[MAX_CHAR], filtru_nume[MAX_CHAR];
    strcpy(filtru_nume, "");
    strcpy(filtru_categorie, "");

    while (1) {
        system("cls");
        display_header("VIZUALIZARE STOC MAGAZIN");
        int i;
        for (i = 0; i < numar_elemente_in_magazin; i++) {
            if (strlen(filtru_nume) != 0 && strstr(magazin[i].denumire, filtru_nume)) {
                afisare_produs_curent(i, optiune_curenta);
            }
            if (strlen(filtru_categorie) != 0 && strstr(magazin[i].categorie, filtru_categorie)) {
                afisare_produs_curent(i, optiune_curenta);
            }
            if (!strlen(filtru_nume) && !strlen(filtru_categorie)) {
                afisare_produs_curent(i, optiune_curenta);
            }
        }

        if (strlen(filtru_nume)) {
            printf("Filtru curent: *nume - %s*\n", filtru_nume);
        } else if (strlen(filtru_categorie)) {
            printf("Filtru curent: *categorie - %s*\n", filtru_categorie);
        }

        printf("\nApasa *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("w");
        set_color(DEFAULT_COLOR);
        printf(" - sus*, *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("s");
        set_color(DEFAULT_COLOR);
        printf(" - jos*, *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("enter");
        set_color(DEFAULT_COLOR);
        printf(" - selectare produs*");
        printf("\n      *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("n");
        set_color(DEFAULT_COLOR);
        printf(" - cautare dupa nume*, *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("c");
        set_color(DEFAULT_COLOR);
        printf(" - cautare dupa categorie*");
        printf("\n      *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("e");
        set_color(DEFAULT_COLOR);
        printf(" - eliminare filtre nume si categorie*");
        printf("\n      *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("q");
        set_color(DEFAULT_COLOR);
        printf(" - revenire la meniu principal*\n");

        input = getch();
        if (input == 'w') {
            optiune_curenta = maxim(optiune_curenta - 1, 0);
            if (strlen(filtru_nume)) {
                while (optiune_curenta != 0 && !strstr(magazin[optiune_curenta].denumire, filtru_nume)) {
                    optiune_curenta = maxim(optiune_curenta - 1, 0);
                }
            } else if (strlen(filtru_categorie)) {
                while (optiune_curenta != 0 && !strstr(magazin[optiune_curenta].categorie, filtru_categorie)) {
                    optiune_curenta = maxim(optiune_curenta - 1, 0);
                }
            }
        } else if (input == 's') {
            optiune_curenta = minim(optiune_curenta + 1, numar_elemente_in_magazin - 1);
            if (strlen(filtru_nume)) {
                while (optiune_curenta != numar_elemente_in_magazin - 1 && !strstr(magazin[optiune_curenta].denumire, filtru_nume)) {
                    optiune_curenta = minim(optiune_curenta + 1, numar_elemente_in_magazin - 1);
                }
            } else if (strlen(filtru_categorie)) {
                while (optiune_curenta != numar_elemente_in_magazin - 1 && !strstr(magazin[optiune_curenta].categorie, filtru_categorie)) {
                    optiune_curenta = minim(optiune_curenta + 1, numar_elemente_in_magazin - 1);
                }
            }
        } else if (input == 'c') {
            printf("Introduceti filtrul dorit pentru categorie: ");
            scanf("%s", filtru_categorie);
            strcpy(filtru_nume, "");
        } else if (input == 'n') {
            printf("Introduceti filtrul dorit pentru nume: ");
            scanf("%s", filtru_nume);
            strcpy(filtru_categorie, "");
        } else if (input == 'e') {
            strcpy(filtru_nume, "");
            strcpy(filtru_categorie, "");
        } else if (input == 'q') {
          break;
        } else if (input == 13) { /// daca se apasa pe ENTER
            if (magazin[optiune_curenta].cantitate == 0) {
                printf("Produsul nu mai este pe stoc!\n");
                system("pause");
                continue;
            }

            printf("Introduceti cantitatea dorita pentru produsul ");
            set_color(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("%s", magazin[optiune_curenta].denumire);
            set_color(DEFAULT_COLOR);
            printf(": ");

            set_color(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            int cantitate;
            scanf("%d", &cantitate);
            set_color(DEFAULT_COLOR);
            while (cantitate <= 0 || cantitate > magazin[optiune_curenta].cantitate) {
                printf("Cantitatea introdusa e invalida, introduceti un numar intre 1 si %d: ", magazin[optiune_curenta].cantitate);
                scanf("%d", &cantitate);
            }

            strcpy(cos[numar_elemente_in_cos].denumire, magazin[optiune_curenta].denumire);
            strcpy(cos[numar_elemente_in_cos].categorie, magazin[optiune_curenta].categorie);
            cos[numar_elemente_in_cos].cantitate = cantitate;
            magazin[optiune_curenta].cantitate -= cantitate;
            cos[numar_elemente_in_cos].pret = magazin[optiune_curenta].pret;
            numar_elemente_in_cos++;
            scriere_stoc();
            scriere_cos();
        }
    }
}
/// END MAGAZIN

/// COS
void scriere_cos() {
    FILE* cos_in = fopen("cos.txt", "w");
    if (cos_in == NULL) {
        printf("S-a produs o eroare, nu se pot accesa fisierele necesare!\nNu se poate scrie (cos.txt)!");
        exit(1);
    }

    int i;
    for (i = 0; i < numar_elemente_in_cos; i++) {
        fprintf(cos_in, "%s\n%s\n%d %.2lf\n",
                cos[i].denumire,
                cos[i].categorie,
                cos[i].cantitate,
                cos[i].pret);
    }

    fclose(cos_in);
}

void citire_cos() {
    FILE* cos_in = fopen("cos.txt", "r");
    if (cos_in == NULL) {
        printf("S-a produs o eroare, nu se pot accesa fisierele necesare!\nNu se poate citi (cos.txt)!");
        exit(1);
    }

    char denumire[MAX_CHAR], categorie[MAX_CHAR], clear_buffer[MAX_CHAR];
    int cantitate; double pret;
    while (fscanf(cos_in, "%[^\n]", denumire) != EOF) {
        fgets(clear_buffer, MAX_CHAR, cos_in);
        fscanf(cos_in, "%[^\n]", categorie);
        fscanf(cos_in, "%d%lf", &cantitate, &pret);
        fgets(clear_buffer, MAX_CHAR, cos_in);

        strcpy(cos[numar_elemente_in_cos].denumire, denumire);
        strcpy(cos[numar_elemente_in_cos].categorie, categorie);
        cos[numar_elemente_in_cos].cantitate = cantitate;
        cos[numar_elemente_in_cos].pret = pret;
        numar_elemente_in_cos++;
    }

    fclose(cos_in);
}

void adaugare_produs_inapoi_in_magazin(int pozitie_cos) {
    int i;
    for (i = 0; i < numar_elemente_in_magazin; i++) {
        if (strcmp(magazin[i].denumire, cos[pozitie_cos].denumire) == 0) {
            magazin[i].cantitate += cos[pozitie_cos].cantitate;
            break;
        }
    }
    scriere_stoc();
}

void actualizare_cantitate(int pozitie_cos) {
    int i, pozitie_magazin;
    for (i = 0; i < numar_elemente_in_magazin; i++) {
        if (strcmp(magazin[i].denumire, cos[pozitie_cos].denumire) == 0) {
            pozitie_magazin = i;
            break;
        }
    }

    int cantitate_maxima = cos[pozitie_cos].cantitate + magazin[pozitie_magazin].cantitate;

    printf("Introduceti cantitatea dorita pentru produsul ");
    set_color(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf("%s", cos[pozitie_cos].denumire);
    set_color(DEFAULT_COLOR);
    printf(": ");

    set_color(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    int cantitate;
    scanf("%d", &cantitate);
    set_color(DEFAULT_COLOR);
    while (cantitate <= 0 || cantitate > cantitate_maxima) {
        printf("Cantitatea introdusa e invalida, introduceti un numar intre 1 si %d: ", cantitate_maxima);
        scanf("%d", &cantitate);
    }

    cos[pozitie_cos].cantitate = cantitate;
    magazin[pozitie_magazin].cantitate = cantitate_maxima - cantitate;
    scriere_cos();
    scriere_stoc();
}

void sterge_produs_din_cos(int pozitie_cos) {
    int i;
    for (i = pozitie_cos + 1; i < numar_elemente_in_cos; i++) {
        strcpy(cos[i - 1].denumire, cos[i].denumire);
        strcpy(cos[i - 1].categorie, cos[i].categorie);
        cos[i - 1].pret = cos[i].pret;
        cos[i - 1].cantitate = cos[i].cantitate;
    }
    numar_elemente_in_cos--;
    scriere_cos();
}

void vizualizare_cos() {
    int input, optiune_curenta = 0;

    while (1) {
        system("cls");
        display_header("VIZUALIZARE COS");
        int i;
        for (i = 0; i < numar_elemente_in_cos; i++) {
            if (i == optiune_curenta) {
                set_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                printf(">> ");
                set_color(DEFAULT_COLOR);
            }

            set_color(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("%15s ", cos[i].denumire);
            set_color(DEFAULT_COLOR);

            printf("%10s ", cos[i].categorie);

            set_color(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            printf("%4d ", cos[i].cantitate);
            set_color(DEFAULT_COLOR);
            printf(" produse ");

            int procentaj_tva_curent = 0;
            for (int t = 0; t < numar_elemente_in_tvas; t++) {
                if (strcmp(tvas[t].categorie, magazin[i].categorie) == 0) {
                    procentaj_tva_curent = tvas[t].procentaj;
                }
            }

            set_color(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            printf("%6.2lf", aplicare_tva(cos[i].pret, procentaj_tva_curent));
            set_color(DEFAULT_COLOR);
            printf(" RON");

            if (i == optiune_curenta) {
                set_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                printf(" <<\n");
                set_color(DEFAULT_COLOR);
            } else {
                printf("\n");
            }
        }

        /// AFISARE STATISTICI COS
        double total = 0;
        for (i = 0; i < numar_elemente_in_cos; i++) {
            int procentaj_tva_curent = 0;
            for (int t = 0; t < numar_elemente_in_tvas; t++) {
                if (strcmp(tvas[t].categorie, magazin[i].categorie) == 0) {
                    procentaj_tva_curent = tvas[t].procentaj;
                }
            }
            total += cos[i].cantitate * aplicare_tva(cos[i].pret, procentaj_tva_curent);
        }
        printf("\nTotal cos: ");
        set_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf("%0.2lf RON\n", total);
        set_color(DEFAULT_COLOR);
        /// END AFISARE STATISTICI COS

        printf("\nApasa *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("w");
        set_color(DEFAULT_COLOR);
        printf(" - sus*, *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("s");
        set_color(DEFAULT_COLOR);
        printf(" - jos*, *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("e");
        set_color(DEFAULT_COLOR);
        printf(" - eliminare produs*");
        printf("\n      *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("a");
        set_color(DEFAULT_COLOR);
        printf(" - actualizare cantitate*");
        printf("\n      *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("q");
        set_color(DEFAULT_COLOR);
        printf(" - revenire la meniu principal*\n");

        input = getch();
        if (input == 'w') {
           optiune_curenta = maxim(optiune_curenta - 1, 0);
        } else if (input == 's') {
            optiune_curenta = minim(optiune_curenta + 1, numar_elemente_in_cos - 1);
        } else if (input == 'e') {
            if (numar_elemente_in_cos != 0) {
                adaugare_produs_inapoi_in_magazin(optiune_curenta);
                sterge_produs_din_cos(optiune_curenta);
            }
        } else if (input == 'a') {
            if (numar_elemente_in_cos != 0) {
                actualizare_cantitate(optiune_curenta);
            }
        } else if (input == 'q') {
          break;
        }
    }
}
/// END COS

/// MENIU PRINCIPAL
char* optiuni_menu_principal[3] = {
    "Vizualizare Magazin",
    "Vizualizare Cos",
    "Iesire"
};

void menu_principal() {
    int input, optiune_curenta = 0;

    while (1) {
        system("cls");
        display_header("MENIU PRINCIPAL");
        int i;
        for (i = 0; i < 3; i++) {
            if (i == optiune_curenta) {
                set_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                printf(">>");
                set_color(DEFAULT_COLOR);
                printf(" %s ", optiuni_menu_principal[i]);
                set_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                printf("<<\n");
                set_color(DEFAULT_COLOR);
            } else {
                printf("%s\n", optiuni_menu_principal[i]);
            }
        }
        printf("\nApasa *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("w");
        set_color(DEFAULT_COLOR);
        printf(" - sus*, *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("s");
        set_color(DEFAULT_COLOR);
        printf(" - jos*, *");
        set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("enter");
        set_color(DEFAULT_COLOR);
        printf(" - submeniu*\n");

        input = getch();
        if (input == 'w') {
           optiune_curenta = maxim(optiune_curenta - 1, 0);
        } else if (input == 's') {
            optiune_curenta = minim(optiune_curenta + 1, 2);
        } else if (input == 13) { /// daca se apasa pe ENTER
            switch (optiune_curenta) {
                case 0: vizualizare_magazin(); break;
                case 1: vizualizare_cos(); break;
                case 2: printf("Multumim pentru utilizare!"); exit(0);
            }
        }
    }
}
/// END MENIU PRINCIPAL

int main() {
    set_color(DEFAULT_COLOR);
    citire_stoc();
    citire_cos();
    citire_tva();
    menu_principal();
    return 0;
}
