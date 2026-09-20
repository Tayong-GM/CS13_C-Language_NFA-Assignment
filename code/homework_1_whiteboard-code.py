
# Minimization algorithm 
# ---------------------------------------------------------------------
def minimize_dfa(states, alphabet, transitions, start, finals, verbose=True):
    finals = set(finals)
    non_finals = set(states) - finals
    partition = [g for g in (finals, non_finals) if g]

    def group_index(state, part):
        for i, g in enumerate(part):
            if state in g:
                return i
        raise ValueError(f"State {state} not found in any group")

    if verbose:
        print(f"0-equivalence: {[sorted(g) for g in partition]}")

    k = 0
    while True:
        k += 1
        new_partition = []
        for group in partition:
            buckets = {}
            for s in group:
                signature = tuple(
                    group_index(transitions[s][a], partition) for a in alphabet
                )
                buckets.setdefault(signature, set()).add(s)
            new_partition.extend(buckets.values())

        if verbose:
            print(f"{k}-equivalence: {[sorted(g) for g in new_partition]}")

        if len(new_partition) == len(partition):
            partition = new_partition
            break
        partition = new_partition

    def name(group):
        return "".join(sorted(group))

    new_states = [name(g) for g in partition]
    new_start = name(next(g for g in partition if start in g))
    new_finals = [name(g) for g in partition if g & finals]
    new_transitions = {}
    for g in partition:
        rep = next(iter(g))
        new_transitions[name(g)] = {
            a: name(next(g2 for g2 in partition if transitions[rep][a] in g2))
            for a in alphabet
        }

    return new_states, alphabet, new_transitions, new_start, new_finals


# print transition table
# ---------------------------------------------------------------------
def print_table(title, states, alphabet, transitions, start, finals):
    print(f"\n{title}")
    header = "        | " + " | ".join(alphabet)
    print(header)
    print("-" * len(header))
    for s in states:
        row = " | ".join(transitions[s][a] for a in alphabet)
        prefix = ("->" if s == start else "  ") + ("*" if s in finals else " ")
        print(f"{prefix}{s:5} | {row}")



#Text-based state diagram 
# ---------------------------------------------------------------------
def print_diagram(title, states, alphabet, transitions, start, finals):
    print(f"\n{title}")
    print(f"  States : {', '.join(states)}")
    print(f"  Start  : {start}")
    print(f"  Final  : {', '.join(sorted(finals)) if finals else '(none)'}")
    print("  Transitions:")
    for s in states:
        marker = ("->" if s == start else "  ") + ("*" if s in finals else " ")
        for a in alphabet:
            dest = transitions[s][a]
            loop = "  (self-loop)" if dest == s else ""
            print(f"    {marker} {s:5} --{a}--> {dest:5}{loop}")


# The DFA 
# ---------------------------------------------------------------------
states = ['A', 'B', 'C', 'D', 'E']
alphabet = ['0', '1']
transitions = {
    'A': {'0': 'B', '1': 'C'},
    'B': {'0': 'B', '1': 'D'},
    'C': {'0': 'B', '1': 'C'},
    'D': {'0': 'B', '1': 'E'},
    'E': {'0': 'B', '1': 'C'},
}
start = 'A'
finals = {'E'}

if __name__ == "__main__":
    print("=" * 60)
    print("Homework_1 (whiteboard-code): A,B,C,D,E ; start A ; final E")
    print("=" * 60)

    print("Equivalence refinement:")
    new_states, alphabet2, new_transitions, new_start, new_finals = minimize_dfa(
        states, alphabet, transitions, start, finals
    )

    print_diagram("Minimized DFA diagram", new_states, alphabet2, new_transitions, new_start, new_finals)
    print_table("Minimized transition table", new_states, alphabet2, new_transitions, new_start, new_finals)
    print(f"\n({len(states)} states -> {len(new_states)} states)")
