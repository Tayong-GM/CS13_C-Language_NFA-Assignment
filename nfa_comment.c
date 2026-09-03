#include <stdio.h>
#include <string.h>

#define NUM_STATES 6
enum { Q0 = 0, Q1, Q2, Q3, Q4, QF };

static const char *state_names[NUM_STATES] = {
    "Q0", "Q1", "Q2", "Q3", "Q4", "QF"
};

typedef unsigned int StateSet;  

#define BIT(s) (1u << (s))

/* delta(state, c) returns the SET of next states (0 means "no move") */
static StateSet delta(int state, char c) {
    int is_star  = (c == '*');
    int is_slash = (c == '/');
    /* anything that's neither '*' nor '/' is the placeholder 'a' */

    switch (state) {
        case Q0:
            return is_slash ? BIT(Q1) : 0;

        case Q1:
            return is_star ? BIT(Q2) : 0;

        case Q2:
            if (is_star)  return BIT(Q3);
            /* is_slash or is_other: stays in the comment body */
            return BIT(Q2);

        case Q3:
            if (is_slash) return BIT(QF);          /* closes the comment */
            if (is_star)  return BIT(Q3) | BIT(Q4); /* <-- real NFA branch */
            return BIT(Q2);                          /* is_other */

        case Q4:
            if (is_slash) return BIT(QF);
            if (is_star)  return BIT(Q4);
            return BIT(Q2);

        case QF:
        default:
            return 0;   /* nothing accepts further input after closing */
    }
}

static StateSet move_set(StateSet current, char c) {
    StateSet next = 0;
    for (int s = 0; s < NUM_STATES; s++) {
        if (current & BIT(s)) {
            next |= delta(s, c);
        }
    }
    return next;
}

static void print_set(StateSet set) {
    printf("{");
    int first = 1;
    for (int s = 0; s < NUM_STATES; s++) {
        if (set & BIT(s)) {
            printf("%s%s", first ? "" : ",", state_names[s]);
            first = 0;
        }
    }
    if (first) printf("-");   /* empty set = dead */
    printf("}");
}

/* Runs the NFA on a string, returns 1 if accepted, 0 if rejected.
   If verbose != 0, prints the active state-set after every symbol. */
int run_nfa(const char *w, int verbose) {
    StateSet current = BIT(Q0);
    size_t n = strlen(w);

    if (verbose) { printf("  "); print_set(current); }

    for (size_t i = 0; i < n; i++) {
        current = move_set(current, w[i]);
        if (verbose) { printf(" --'%c'--> ", w[i]); print_set(current); }
        if (current == 0) break;   /* every path died */
    }
    if (verbose) printf("\n");

    return (current & BIT(QF)) != 0;
}

int main(int argc, char *argv[]) {

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            int accepted = run_nfa(argv[i], 1);
            printf("\"%s\" -> %s\n\n", argv[i], accepted ? "ACCEPTED" : "REJECTED");
        }
        return 0;
    }

    /* Interactive mode: keep asking for strings until "exit" or blank line */
    char line[1024];

    printf("NFA for C-style comments  (alphabet: a, *, /)\n");
    printf("Type a string to test, or \"exit\" to quit.\n\n");

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) break; /* EOF (Ctrl+Z) */

        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }

        if (len == 0) break;
        if (strcmp(line, "exit") == 0) break;

        int accepted = run_nfa(line, 1);
        printf("\"%s\" -> %s\n\n", line, accepted ? "ACCEPTED" : "REJECTED");
    }

    printf("Bye!\n");
    return 0;
}
