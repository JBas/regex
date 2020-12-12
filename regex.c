/**
 * @file regex.c
 * @author Joshua Bas
 * @date 4 Dec 2020
 * @brief A RegEx library implementation.
 *
 *        https://en.wikipedia.org/wiki/Regular_expression
 *        NFA is converted to DFA, and DFA is applied to string one
 *        symbol at a time.
 *        Theoretical compile time: O(2^m) for a pattern of length n
 *        Theoretical run time: O(n) for a string of length m
 *
 *        https://en.wikipedia.org/wiki/Nondeterministic_finite_automaton
 *        A FSM is deterministic finite automaton (DFA) if:
 *            1) each of its transitions is uniquely determined by its source state and input symbol, and
 *            2) reading an input symbol is required for each state transition.
 *        A nondeterministic finite automaton (NFA) does not need to obey these restrictions.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ctype.h>

#include "stack.h"

void compile(const char* pattern);
int expand(const char* pattern, size_t plen, char** ret, size_t* rlen);
int expand_parens(const char* pattern, size_t plen, char** ret, size_t* rlen, size_t i, size_t* j, size_t* r);
int expand_bracket(const char* pattern, size_t plen, char** ret, size_t* rlen, size_t i, size_t* j, size_t* r);

int main() {
    const char* pattern = "a*";

    char* ret = malloc(sizeof(char)*11);
    memset(ret, '\0', 11);
    size_t rlen = 11;

    if (expand(pattern, strlen(pattern), &ret, &rlen) < 0) {return -1; }

    printf("%s\n", ret);

    free(ret);
    return 0;
}

/**
 * @brief expands regex into an expression consisting of (), |, *, and symbols
 * @param const char* pattern regex to be expanded
 * @param char *ret expanded regex (pre-allocated pointer)
 * @return int: indicates failure or success
 * @details preparing for Thompson's construction for an NFA from https://en.wikipedia.org/wiki/Thompson%27s_construction
 */
int expand(const char* pattern, size_t plen, char** ret, size_t* rlen) {
    printf("%s\n", pattern);

    size_t r = 0;

    for (size_t i = 0; i < plen; i++) {
        const char c = pattern[i];

        if (2*r > *rlen) {
            *rlen = 3*r+1;
            *ret = realloc(*ret, sizeof(char)*(*rlen));
            memset(*ret+r, '\0', *rlen-r);
        }

        if (c == '(') {
            // find index of matching parens and get string inside
            // expand substring
            size_t j;

            if (expand_parens(pattern, plen, ret, rlen, i, &j, &r) < 0) {
                return -1;
            }

            i = j;

        } else if (c == '[') {
            // find index of matching bracket and get string inside
            // convert symbols to or'd expression
            size_t j;

            if (expand_bracket(pattern, plen, ret, rlen, i, &j, &r) < 0) {
                return -1;
            }

            i = j;

        } else {
            (*ret)[r] = c;
            r++;
        }

    }

    *rlen = strlen(*ret)+1;
    *ret = realloc(*ret, sizeof(char)*(*rlen));

    (*ret)[*rlen-1] = '\0';
    return 0;
}

int expand_parens(const char* pattern, size_t plen, char** ret, size_t* rlen, size_t i, size_t* j, size_t* r) {
    if (gobble(pattern, plen, i+1, ')', '(', j) < 0) {
        return -1;
    }

    size_t n = (*j - i + 1) - 2;

    char* substring = calloc(n+1, sizeof(char));
    char* subret = calloc(n+1, sizeof(char));
    size_t subret_len = n+1;
    memset(substring, '\0', subret_len);
    memset(subret, '\0', subret_len);

    strncpy(substring, pattern+i+1, n);
    //printf("start=%s i=%zu j=%zu n=%zu, subl=%zu, sub=%s\n", pattern+i, i, *j, n, strlen(substring), substring);

    if (expand(substring, strlen(substring), &subret, &subret_len) < 0) {
        return -1;
    }

    if (subret_len-1 > *rlen-1-(*r)) {
        *rlen = 3*(subret_len-1)+1;
        *ret = realloc(*ret, sizeof(char)*(*rlen));
        memset(*ret+(*r), '\0', *rlen-(*r));
    }

    (*ret)[*r] = '(';
    (*r)++;

    strncpy(*ret+(*r), subret, strlen(subret));
    (*r) += strlen(subret);

    (*ret)[*r] = ')';
    (*r)++;

    free(substring);
    free(subret);

    return 0;
}

int expand_bracket(const char* pattern, size_t plen, char** ret, size_t* rlen, size_t i, size_t* j, size_t* r) {
    if (gobble(pattern, plen, i+1, ']', '\0', j) < 0) {
        return -1;
    }

    size_t n = (*j - i + 1) - 2;

    char* substring = calloc(n+1, sizeof(char));
    char* subex = calloc(n+1, sizeof(char));
    size_t subex_len = n+1;

    memset(substring, '\0', n+1);
    memset(subex, '\0', n+1);

    strncpy(substring, pattern+i+1, n);

    //printf("start=%s i=%zu j=%zu n=%zu, sub=%s\n", pattern+i, i, *j, n, substring);

    subex[0] = '(';
    size_t k = 1;
    for (size_t q = 0; q < n; q++) {
        char c = substring[q];

        if (2*k > subex_len) {
            subex_len = 3*k+1;
            subex = realloc(subex, sizeof(char)*subex_len);
            memset(subex+k, '\0', subex_len-k);
        }

        if ((k-1 > 0) && (k-1 < k)) {
            subex[k] = '|';
            k++;
        }

        if (isdigit(c)) {
            if ((q+1 < n) && (substring[q+1] == '-')) {
                if (q+2 >= n) { return -1; }
                else if (!isdigit(substring[q+2])) { return -1; }

                subex[k] = c;
                subex[k+1] = '-';
                subex[k+2] = substring[q+2];
                k += 3;
                q += 2;
            } else {
                subex[k] = c;
                k++;
            }
        } else {
            subex[k] = c;
            k++;
        }
    }

    subex[k] = ')';
    subex_len = strlen(subex)+1;
    subex = realloc(subex, sizeof(char)*subex_len);
    subex[subex_len-1] = '\0';

    if (2*(subex_len-1) > *rlen-1-(*r)) {
        *rlen = 3*(subex_len-1)+1;
        *ret = realloc(*ret, sizeof(char)*(*rlen));
        memset(*ret+(*r), '\0', *rlen-(*r));
    }

    strncpy(*ret+(*r), subex, strlen(subex));
    *r += strlen(subex);

    free(substring);
    free(subex);

    return 0;
}

/**
 * @brief <brief>
 * @param const char* pattern <parameter_description>
 * @return void
 * @details <details>
 */
void compile(const char* pattern) {

    for (const char* p = pattern; p != NULL && *p != '\0'; p++) {
        printf("%c", *p);

    }
    printf("%c", '\n');

}
