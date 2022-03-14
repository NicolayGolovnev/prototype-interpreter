//
// Created by kolya on 10/21/2021.
//

#ifndef ANALYZATOR_DIAGRAM_H
#define ANALYZATOR_DIAGRAM_H

#include "Scaner.h"
#include "Semantic.h"

class Diagram {
private:
    Scaner* scaner;
    TypeLex lex;
    int lookForward(int);

    // ф-ии СД
    void typeStruct();
    void typeMain();
    void typeConstData();
    void typeVarData();
    DATA_TYPE typeType();
    void typeConstList();
    void typeVarList();
    void typeOperator();
    void typeCompoundOperator();
    void typeAssign();
    void typeFor();
    void typeExpression(ExpresData* ed);
    void typeXorEl(ExpresData* ed);
    void typeAndEl(ExpresData* ed);
    void typeComparesEl(ExpresData* ed);
    void typeSummEl(ExpresData* ed);
    void typeMultEl(ExpresData* ed);
    void typeElemExpression(ExpresData* ed);

public:
    Tree* root;

    Diagram(Scaner* sc) {scaner = sc;}
    ~Diagram() {}
    void program(); // Программа
};


#endif //ANALYZATOR_DIAGRAM_H
