#ifndef _PILE_H_
#define _PILE_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Noeud {
    char data[32];
    struct Noeud *next;
} Noeud;

typedef struct {
    Noeud *sommet;
} Pile;

void empiler(Pile *pile, const char *data) {
    Noeud *nvNoeud = (Noeud *)malloc(sizeof(Noeud));
    strcpy(nvNoeud->data, data);
    nvNoeud->next = pile->sommet;
    pile->sommet = nvNoeud;
}

void depiler(Pile *pile) {
    if (pile->sommet == NULL) return;
    Noeud *temp = pile->sommet;
    pile->sommet = pile->sommet->next;
    free(temp);
}

const char *sommetData(Pile *pile) {
    if (pile->sommet == NULL) return NULL;
    return pile->sommet->data;
}

int estVide(Pile *pile) {
    return pile->sommet == NULL;
}

void initPile(Pile *pile) {
    pile->sommet = NULL;
}

#endif
