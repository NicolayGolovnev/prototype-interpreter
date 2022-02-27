//
// Created by kolya on 10/21/2021.
//

#include <iostream>
#include "../defs.h"
#include "../Scaner.h"
#include "../Diagram.h"

int testSyntax(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    Scaner* scaner;
    if (argc <= 1)
        scaner = new Scaner(const_cast<char*>("TestsDiagram/myTest.txt"));
    else
        scaner = new Scaner(argv[1]);
    Diagram* diagram = new Diagram(scaner);
    diagram->program();

    int type; TypeLex lex;
    type = scaner->scan(lex);
    if (type == TypeEnd)
        printf("Syntax errors not found!\n");
    else
        scaner->printError(const_cast<char*>("Trash text in program ending"), const_cast<char*>(""), -1, -1);

    return 0;
}
