
#include "nfa.h"
#include "stack.h"

typedef struct state lstate;
struct state {
    char* matches;
    lstate** transitions;
    size_t len;
};

typedef struct nfa lnfa;
struct nfa {
    lstate* q0;
    lstate* qf;
    int reached_qf;
};

lnfa* nfa_create(const char* pattern, size_t plen) {

    stacked_t stack = stack_create();


    for (size_t i = 0; i < plen; i++) {
        const char p = pattern[i];

        switch (p) {
            case '*': {

            };
            case '|': {
                if (i+1 >= plen) {
                    dbg_printf("Issues!\n");
                    return NULL;
                }

                lnfa* unon = malloc(sizeof(lnfa));
                unon->len = 0;

                lnfa* a = (lnfa*)stack_pop(stack);

                unon->transition = malloc(2*sizeof(lnfa*));
                unon->transition[unon->len] = a;

                if (pattern[i+1] == '(') {

                } else {

                }

                break;
            };
            case '(': {
            };
            default: {
                lnfa* symbol = malloc(sizeof(lnfa));
                symbol->len = 0;
                symbol->match_item = malloc(sizeof(char));
                symbol->match_item[symbol->len] = p;
                symbol->transition = malloc(sizeof(lnfa*));
                symbol->transition[symbol->len] = NULL;
                symbol->len++;

                stack_push((elem_t)symbol);
            };
        }
    }

    if (stack_getsize(stack) != 1) {
        dbg_printf("Issues!\n");
        return NULL;
    }

    elem_t final = stack_pop(stack);
    stack_destroy(stack);

    return (lnfa*)final;

}
