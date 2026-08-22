#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
MAIN="$ROOT/src/mainwindow.cpp"
TERMUI="$ROOT/src/terminalui.cpp"

# importBbsList is overloaded. Connecting QAction::triggered directly to the
# overloaded member leaves Qt6 unable to deduce Func2. Keep an explicit lambda
# selecting the zero-argument GUI importer.
grep -Fq 'connect(m_importBbsAction, &QAction::triggered, this, [this] { importBbsList(); });' "$MAIN"
if grep -Fq 'connect(m_importBbsAction, &QAction::triggered, this, &MainWindow::importBbsList);' "$MAIN"; then
    echo 'FAIL: ambiguous Qt6 connection to overloaded MainWindow::importBbsList reintroduced' >&2
    exit 1
fi

# C++ needs two source backslashes to deliver \] to QRegularExpression.
grep -Fq '[:>?\\]]' "$TERMUI"

echo 'WaffleHouse-Client 3.3r1 Qt6 overloaded-slot/regex compile guard: PASS'
