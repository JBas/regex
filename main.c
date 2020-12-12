

#include "lib/stack.h"
#include <stdio.h>

int main() {

    stacked_t stack = stack_create();
    if (stack == NULL) {
        printf("Error!\n");
        return -1;
    }

    for (char i = 0; i < 26; i++) {
        char c = 'a' + i;
        stack_push(stack, (elem_t)c);
    }

    elem_t e;
    while (!stack_isempty(stack)) {
        e = stack_pop(stack);
        printf("%c\n", (char)e);
    }
    stack_destroy(stack);

    return 0;
}
