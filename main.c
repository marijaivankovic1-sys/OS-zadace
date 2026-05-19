#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#define STATUS_FILE "status.txt"
#define OBRADA_FILE "obrada.txt"

int broj = 1;

// signali

void sigusr1_handler(int sig) {
    printf("\n[SIGUSR1] trenutni broj: %d\n", broj);
}

void sigterm_handler(int sig) {
    FILE *f = fopen(STATUS_FILE, "w");
    if (f) {
        fprintf(f, "%d", broj);
        fclose(f);
    }

    printf("\n[SIGTERM] spremljeno stanje: %d\n", broj);
    exit(0);
}

void sigint_handler(int sig) {
    printf("\n[SIGINT] prekid bez spremanja.\n");
    exit(0);
}

// citanje statusa

void ucitaj_status() {
    FILE *f = fopen(STATUS_FILE, "r");

    if (f) {
        if (fscanf(f, "%d", &broj) != 1)
            broj = 0;
        fclose(f);
    } else {
        broj = 0;
    }
}

//oporavak

void oporavak() {
    FILE *f = fopen(OBRADA_FILE, "r");
    int zadnji = 0, tmp;

    if (!f) {
        broj = 1;
        return;
    }

    while (fscanf(f, "%d", &tmp) == 1) {
        zadnji = tmp;
    }

    fclose(f);

    if (zadnji > 0)
        broj = (int)sqrt(zadnji) + 1;
    else
        broj = 1;
}

// zapis 

void zapisi(int kvadrat) {
    FILE *f = fopen(OBRADA_FILE, "a");
    if (f) {
        fprintf(f, "%d\n", kvadrat);
        fclose(f);
    }
}


int main() {

    printf("PID: %d\n\n", getpid());

    signal(SIGUSR1, sigusr1_handler);
    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigint_handler);

    ucitaj_status();

    if (broj == 0) {
        printf("[OPORAVAK] analiza obrada.txt...\n");
        oporavak();
    }

    // status je 0, obrada je u tijeku
    
    FILE *f = fopen(STATUS_FILE, "w");
    if (f) {
        fprintf(f, "0");
        fclose(f);
    }

    while (1) {

        int x = broj * broj;

        zapisi(x);

        printf("broj: %d -> %d\n", broj, x);

        broj++;
        sleep(5);
    }
}