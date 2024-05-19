#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pile.h"

#define MAX_TOKENS 100
#define MAX_PRODUCTIONS 3

typedef struct {
    char type[3];
    char value[32];
} Token;

typedef struct {
    char nonTerminal[3];
    char terminal[3];
    char production[MAX_PRODUCTIONS][3];
    int productionLength;
} TableAnalyse;

int isDigit(char c) {
    return c >= '0' && c <= '9';
}

void addToken(Token *tokens, int *tokenCount, const char *type, const char *value) {
    strcpy(tokens[*tokenCount].type, type);
    strcpy(tokens[*tokenCount].value, value);
    (*tokenCount)++;
}

void affichageJeton(Token *tokens, int tokenCount) {
    for (int i = 0; i < tokenCount; i++) {
        printf("Le jeton numero : %d: est de Type : %s, sa Valeur est : %s\n", i, tokens[i].type, tokens[i].value);
    }
}

void lex(const char *input, Token *tokens, int *tokenCount) {
    int i = 0;
    while (input[i] != '\0') {
        if (isspace(input[i])) {
            i++;
            continue;
        }

        if (isDigit(input[i])) {
            int j = i;
            while (isDigit(input[j])) j++;
            char number[32];
            strncpy(number, input + i, j - i);
            number[j - i] = '\0';
            addToken(tokens, tokenCount, "n", number);
            i = j;
        } else if (input[i] == '+') {
            addToken(tokens, tokenCount, "+", "+");
            i++;
        } else if (input[i] == '-') {
            addToken(tokens, tokenCount, "-", "-");
            i++;
        } else if (input[i] == '*') {
            addToken(tokens, tokenCount, "*", "*");
            i++;
        } else if (input[i] == '(') {
            addToken(tokens, tokenCount, "(", "(");
            i++;
        } else if (input[i] == ')') {
            addToken(tokens, tokenCount, ")", ")");
            i++;
        } else {
            printf("caracter inattendu : %c\n", input[i]);
            exit(1);
        }
    }
    addToken(tokens, tokenCount, "$", "$");  // End-of-input marker
}

TableAnalyse tableAnalyse[] = {
    {"E", "(", {"T", "E'", ""}, 2},
    {"E", "n", {"T", "E'", ""}, 2},
    {"E'", "+", {"+", "T", "E'"}, 3},
    {"E'", "-", {"-", "T", "E'"}, 3},
    {"E'", ")", {"e", "", ""}, 1},
    {"E'", "$", {"e", "", ""}, 1},
    {"T", "(", {"F", "T'", ""}, 2},
    {"T", "n", {"F", "T'", ""}, 2},
    {"T'", "+", {"e", "", ""}, 1},
    {"T'", "-", {"e", "", ""}, 1},
    {"T'", "*", {"*", "F", "T'"}, 3},
    {"T'", ")", {"e", "", ""}, 1},
    {"T'", "$", {"e", "", ""}, 1},
    {"F", "(", {"(", "E", ")"}, 3},
    {"F", "n", {"n", "", ""}, 1}
};

TableAnalyse *findEntry(const char *nonTerminal, const char *terminal) {
    for (int i = 0; i < sizeof(tableAnalyse) / sizeof(tableAnalyse[0]); i++) {
        if (strcmp(tableAnalyse[i].nonTerminal, nonTerminal) == 0 &&
            strcmp(tableAnalyse[i].terminal, terminal) == 0) {
            return &tableAnalyse[i];
        }
    }
    return NULL;
}

int parse(Token *tokens, int tokenCount) {
    Pile pile;
    initPile(&pile);
    empiler(&pile, "$");
    empiler(&pile, "E");

    int tokenIndex = 0;
    while (!estVide(&pile)) {
        const char *top = sommetData(&pile);
        Token currentToken = tokens[tokenIndex];

        printf("la sommet de la pile contient : %s, le token actuelle est : %s\n", top, currentToken.type);

        if (strcmp(top, "e") == 0) {
            depiler(&pile);
            continue;
        }

        // Ajout de la condition pour vérifier si la pile et le token courant sont tous deux $
        if (strcmp(top, "$") == 0 && strcmp(currentToken.type, "$") == 0) {
            return 1;
        }

        if (strcmp(top, currentToken.type) == 0) {
            depiler(&pile);
            tokenIndex++;
        } else {
            TableAnalyse *entry = findEntry(top, currentToken.type);
            if (entry != NULL) {
                printf("Production trouve pour (%s, %s): ", top, currentToken.type);
                for (int i = 0; i < entry->productionLength; i++) {
                    printf("%s ", entry->production[i]);
                }
                printf("\n");

                depiler(&pile);
                for (int i = entry->productionLength - 1; i >= 0; i--) {
                    if (strcmp(entry->production[i], "") != 0 && strcmp(entry->production[i], "e") != 0) {
                        empiler(&pile, entry->production[i]);
                    }
                }
            } else {
                printf("Erreur: Token inattendu %s\n", currentToken.value);
                return 0;
            }
        }
    }

    return 0;
}

int main() {
    printf(
        "\n"
        "              tttt                                    tttt\n"
        "           ttt:::t                                 ttt:::t\n"
        "           t:::::t                                 t:::::t\n"
        "           t:::::t                                 t:::::t\n"
        "     ttttttt:::::ttttttt      aaaaaaaaaaaaa  ttttttt:::::ttttttt      aaaaaaaaaaaaa        xxxxxxx      xxxxxxx\n"
        "     t:::::::::::::::::t      a::::::::::::a t:::::::::::::::::t      a::::::::::::a        x:::::x    x:::::x\n"
        "     t:::::::::::::::::t      aaaaaaaaa:::::at:::::::::::::::::t      aaaaaaaaa:::::a        x:::::x  x:::::x\n"
        "     tttttt:::::::tttttt               a::::atttttt:::::::tttttt               a::::a         x:::::xx:::::x\n"
        "           t:::::t              aaaaaaa:::::a      t:::::t              aaaaaaa:::::a          x::::::::::x\n"
        "           t:::::t            aa::::::::::::a      t:::::t            aa::::::::::::a           x::::::::x\n"
        "           t:::::t           a::::aaaa::::::a      t:::::t           a::::aaaa::::::a           x::::::::x\n"
        "           t:::::t    tttttta::::a    a:::::a      t:::::t    tttttta::::a    a:::::a          x::::::::::x\n"
        "           t::::::tttt:::::ta::::a    a:::::a      t::::::tttt:::::ta::::a    a:::::a         x:::::xx:::::x\n"
        "           tt::::::::::::::ta:::::aaaa::::::a      tt::::::::::::::ta:::::aaaa::::::a        x:::::x  x:::::x\n"
        "             tt:::::::::::tt a::::::::::aa:::a       tt:::::::::::tt a::::::::::aa:::a      x:::::x    x:::::x\n"
        "               ttttttttttt    aaaaaaaaaa  aaaa         ttttttttttt    aaaaaaaaaa  aaaa     xxxxxxx      xxxxxxx\n"
        "\n"
    );
    const char *input = "3+5*(10-4)";
    Token tokens[MAX_TOKENS];
    int tokenCount = 0;

    lex(input, tokens, &tokenCount);
    affichageJeton(tokens, tokenCount);

    if (parse(tokens, tokenCount)) {
        printf("Analyse reussie, mot accepte\n");
    } else {
        printf("Analyse echouee, mot pas accepte\n");
    }

    return 0;
}
