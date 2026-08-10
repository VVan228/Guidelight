import sys
from pathlib import Path
import re
from enum import Enum
import shutil

OUTPUT_DIR = 'visualize/res'
FILENAME = 'dot{}'
FILETYPE = 'svg'

def lines_iter(foo):
    prevnl = -1
    while True:
        nextnl = foo.find('\n', prevnl + 1)
        if nextnl < 0: break
        yield foo[prevnl + 1:nextnl]
        prevnl = nextnl

class Mode(Enum):
    INIT = 0
    TS = 1
    DFA = 2

class Node:
    def __init__(self) -> None:
        self.label = ""
        self.is_fin = False


class DFA:
    def __init__(self) -> None:
        self.agent = ""
        self.nodes = []
        self.transitions = []

class TS:
    def __init__(self) -> None:
        self.transitions = []
        self.properties = []
        self.initial_states = []

m = Mode.INIT

cur_dfa = None
cur_ts = None
all = []

def clear_dfa():
    global cur_dfa
    global all
    if cur_dfa != None:
        all.append(cur_dfa)
    del cur_dfa
    cur_dfa = None

def clear_ts():
    global cur_ts
    global all
    if cur_ts != None:
        all.append(cur_ts)
    del cur_ts
    cur_ts = None

def init_ts(s):
    global cur_ts
    global all
    global m
    m = Mode.TS
    cur_ts = TS()
    for i in re.findall(r"\d+", s):
        cur_ts.initial_states.append(int(i))

def init_dfa(s):
    global cur_dfa
    global all
    global m
    m = Mode.DFA
    cur_dfa = DFA()
    cur_dfa.agent = s[0]

input = sys.stdin.read()
for s in lines_iter(input):
    # DFA business
    is_dfa = re.match(r"[A-Z] DFA", s) != None
    if is_dfa and m == Mode.INIT:
        init_dfa(s)
    elif is_dfa and m == Mode.DFA:
        clear_dfa()
        init_dfa(s)
    elif is_dfa and m == Mode.TS:
        clear_ts()
        init_dfa(s)
    is_node = re.match(r"^node: \d+\s(\d+\s)*", s) != None
    is_init_node = re.match(r"^node: init_node", s) != None
    if (is_node or is_init_node) and m == Mode.DFA:
        cur_dfa.nodes.append(Node())
        label = s[6:]
        is_fin = re.match(r".*\(fin\)$", label) != None
        if is_fin:
            cur_dfa.nodes[-1].label = label[:-5]
        else:
            cur_dfa.nodes[-1].label = label
        cur_dfa.nodes[-1].is_fin = is_fin

    is_node_transit = re.match(r"<([a-zA-Z]\d*\s)*>\s->\s\d+(\s\d+)*", s) != None
    if is_node_transit and m == Mode.DFA:
        res = s.split("->")
        cur_dfa.transitions.append((cur_dfa.nodes[-1].label, res[1][1:], res[0][1:-2]))
    # TS business
    is_ts = re.match(r"init:\s\d+(\s\d+)*", s) != None
    if is_ts and m == Mode.INIT:
        init_ts(s)
    elif is_ts and m == Mode.TS:
        clear_ts()
        init_ts(s)
    elif is_ts and m == Mode.DFA:
        clear_dfa()
        init_ts(s)
    is_ts_transit = re.match(r"^\d+\s(\d+\s)*$", s) != None
    if is_ts_transit and m == Mode.TS:
        cur_ts.transitions.append([])
        for i in re.findall(r"\d+", s):
            cur_ts.transitions[-1].append(int(i))
    is_ts_props = re.match(r"^\d+:\s([a-zA-Z]+\d*\s)*$", s) != None
    if is_ts_props and m == Mode.TS:
        res = s.split(": ")
        cur_ts.properties.append(res[1])

if cur_dfa != None:
    all.append(cur_dfa)
if cur_ts != None:
    all.append(cur_ts)


from graphviz import Digraph

dots = []

for a in all:
    if isinstance(a, DFA):
        dot = Digraph(a.agent+" DFA")
        dot.attr(rankdir='LR')
        for n in a.nodes:
            if n.is_fin:
                dot.node(n.label, n.label, shape='doublecircle')
            elif n.label == 'init_node':
                dot.node("-",shape='none', label='', width='0', height='0')
                dot.node(n.label, n.label)
                dot.edge("-", n.label)
            else:
                dot.node(n.label, n.label, shape='circle')
        for t in a.transitions:
            dot.edge(t[0], t[1], t[2])
        dots.append(dot)
    if isinstance(a, TS):
        dot = Digraph("ts")
        dot.attr(rankdir='LR')
        for i in range(len(a.transitions)):
            if i in a.initial_states:
                dot.node(str(i), str(i), shape='circle',xlabel='{'+a.properties[i]+'}')
                dot.node(str(i)+'i', shape='none', label='', width='0', height='0')
                dot.edge(str(i)+'i', str(i))
            dot.node(str(i), str(i), shape='circle',xlabel='{'+a.properties[i]+'}')
        for i in range(len(a.transitions)):
            for j in range(len(a.transitions)):
                if a.transitions[i][j] == 1:
                    dot.edge(str(i), str(j))
        dots.append(dot)


if Path(OUTPUT_DIR).is_dir():
    shutil.rmtree(OUTPUT_DIR)
for i in range(len(dots)):
    dots[i].render(FILENAME.format(i), directory=OUTPUT_DIR, format=FILETYPE)
