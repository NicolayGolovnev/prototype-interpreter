//
// Created by kolya on 12/1/2021.
//

#include <iostream>
#include "defs.h"
#include "Scaner.h"
#include "Diagram.h"
#include "Semantic.h"


int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    Scaner* scaner;
    if (argc <= 1)
        scaner = new Scaner(const_cast<char*>("test3.txt"));
    else
        scaner = new Scaner(argv[1]);
    Diagram* diagram = new Diagram(scaner);
    Node node;
    memcpy(node.id, &("root"), 5);
    node.dataType = TYPE_NONE;
    node.objectType = TYPE_UNDEFINED;

    Tree* root = new Tree(nullptr, nullptr, nullptr, &node);
    root->setCur(root);
    diagram->root = root;
    root->setScaner(scaner);
    diagram->program();

    int type; TypeLex lex;
    type = scaner->scan(lex);
    if (type == TypeEnd)
        printf("\nSyntax errors not found!\n");
    else
        scaner->printError(const_cast<char*>("Trash text in program ending"), const_cast<char*>(""), -1, -1);
    root->print();

    return 0;
}
