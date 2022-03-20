//
// Created by kolya on 10/21/2021.
//

#include <iostream>
#include "Diagram.h"
#include "defs.h"

#define max(a, b) a > b ? a : b
#define isConst(type) ((type == TypeConstInt) || (type == TypeConstHex) || (type == TypeConstExp))
#define isType(type) ((type == TypeInt) || (type == TypeShort) || (type == TypeLong) || (type == TypeDouble) || (type == TypeIdent))
#define isOperator(type) ((type == TypeFor) || ((type == TypeIdent) && ((lookForward(2) == TypeAssign) || (lookForward(2) == TypeDot))) || (type == TypeEndComma) || (type == TypeLeftFB))
#define isCompare(type) ((type == TypeEqual) || (type == TypeNotEqual) || (type == TypeLess) || (type == TypeLessOrEqual) || (type == TypeMore) || (type == TypeMoreOrEqual))

DATA_TYPE dataType;
TypeLex structIdent;

int Diagram::lookForward(int q) {
    int position = scaner->getPos();
    int line = scaner->getLine();
    int newLinePos = scaner->getNewLinePos();

    int nextType = TypeErr;
    for (int i = 0; i < q; i++)
        nextType = scaner->scan(lex);
    scaner->setPos(position);
    scaner->setLine(line);
    scaner->setNewLinePos(newLinePos);

    return nextType;
}

void Diagram::program() {
    int type = lookForward(1);
    while ((type == TypeStruct) || (type == TypeConst) || isType(type)) {
        if (type == TypeStruct)
            typeStruct();
        else if (type == TypeConst)
            typeConstData();
        else if (isType(type)){
            type = lookForward(2);
            if (type == TypeMain)
                typeMain();
            else
                typeVarData();
        }

        type = lookForward(1);
    }
    if (type != TypeEnd)
        scaner->printError(const_cast<char *>("Expected keywords \'struct\', \'main\' or data descriptions"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
    //TODO удаление всего синтаксического дерева
}

DATA_TYPE Diagram::typeType() {
    int type = scaner->scan(lex);
    if (!isType(type))
        scaner->printError(const_cast<char *>("Expected one of the variable types - int, short, long, double -, or identifier"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
    memcpy(structIdent, lex, sizeof(lex));
    if (type == TypeShort)
        return TYPE_SHORT;
    else if (type == TypeLong)
        return TYPE_LONG;
    else if (type == TypeInt)
        return TYPE_INTEGER;
    else if (type == TypeDouble)
        return TYPE_DOUBLE;
    else if (type == TypeIdent)
        return TYPE_DATASTRUCT;
}

void Diagram::typeStruct() {
    int type = scaner->scan(lex);
    if (type != TypeStruct)
        scaner->printError(const_cast<char *>("Expected keyword \'struct\'"), lex, scaner->getLine(),
                scaner->getPos() - scaner->getNewLinePos());

    type = scaner->scan(lex);
    if (type != TypeIdent)
        scaner->printError(const_cast<char *>("Expected identifier of struct"), lex, scaner->getLine(),
                scaner->getPos() - scaner->getNewLinePos());
    Tree* v = root->semanticInclude(lex, OBJECT_TYPE::TYPE_UNDEFINED, DATA_TYPE::TYPE_DATASTRUCT);

    type = scaner->scan(lex);
    if (type != TypeLeftFB)
        scaner->printError(const_cast<char *>("Expected symbol {"), lex, scaner->getLine(),
                scaner->getPos() - scaner->getNewLinePos());

    type = lookForward(1);
    while (type != TypeRightFB) {
        if (type == TypeConst)
            typeConstData();
        else if (isType(type))
            typeVarData();
        else
            scaner->printError(const_cast<char *>("Expected keyword \'const\' or data descriptions"), lex, scaner->getLine(),
                    scaner->getPos() - scaner->getNewLinePos());
        type = lookForward(1);
    }

    type = scaner->scan(lex);
    if (type != TypeRightFB)
        scaner->printError(const_cast<char *>("Expected symbol }"), lex, scaner->getLine(),
                scaner->getPos() - scaner->getNewLinePos());

    type = scaner->scan(lex);
    if (type != TypeEndComma)
        scaner->printError(const_cast<char *>("Expected symbol ;"), lex, scaner->getLine(),
                scaner->getPos() - scaner->getNewLinePos());

    root->setCur(v);
}

void Diagram::typeMain() {
    int type = scaner->scan(lex);
    if (type != TypeInt)
        scaner->printError(const_cast<char *>("Expected int type of main func"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
    type = scaner->scan(lex);
    if (type != TypeMain)
        scaner->printError(const_cast<char *>("Expected keyword \'main\'"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
    Tree* v = root->semanticInclude(lex, OBJECT_TYPE::TYPE_FUNC, DATA_TYPE::TYPE_INTEGER);
    root->semanticSetInit(v, false);

    type = scaner->scan(lex);
    if (type != TypeLeftRB)
        scaner->printError(const_cast<char *>("Expected symbol ("),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    type = scaner->scan(lex);
    if (type != TypeRightRB)
        scaner->printError(const_cast<char *>("Expected symbol )"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    type = scaner->scan(lex);
    if (type == TypeLeftFB) {
        do {
            type = lookForward(1);
            if (isOperator(type))
                typeOperator();
            else if (type == TypeConst)
                typeConstData();
            else if (isType(type))
                typeVarData();
            else
                scaner->printError(const_cast<char*>("Expected constant, variables, operator or return of main"),
                                   lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

            type = lookForward(1);
        } while (type != TypeReturn);
        scaner->scan(lex);
        type = scaner->scan(lex);
        if (type != TypeConstInt)
            scaner->printError(const_cast<char*>("Expected int constant"),
                               lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
        type = scaner->scan(lex);
        if (type != TypeEndComma)
            scaner->printError(const_cast<char*>("Expected a symbol ;"),
                               lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
        type = scaner->scan(lex);
        if (type != TypeRightFB)
            scaner->printError(const_cast<char*>("Expected a symbol }"),
                               lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
        root->setCur(v);
    }
    else
        scaner->printError(const_cast<char *>("Expected symbol { of main compound operator"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
}

void Diagram::typeConstData() {
    int type = scaner->scan(lex);
    if (type != TypeConst)
        scaner->printError(const_cast<char *>("Expected keyword \'const\'"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
    type = scaner->scan(lex);
    if ((!isType(type)) || (type == TypeIdent))
        scaner->printError(const_cast<char*>("Expected variable type - int, short, long or double"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
    if (type == TypeInt)
        dataType = TYPE_INTEGER;
    else if (type == TypeShort)
        dataType = TYPE_SHORT;
    else if (type == TypeLong)
        dataType = TYPE_LONG;
    else if (type == TypeDouble)
        dataType = TYPE_DOUBLE;

    typeConstList();
    type = scaner->scan(lex);
    if (type != TypeEndComma)
        scaner->printError(const_cast<char *>("Expected symbol ;"),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
}

void Diagram::typeVarData() {
    dataType = typeType();
    if (dataType == DATA_TYPE::TYPE_DATASTRUCT)
        if (!root->isStruct(Tree::cur, lex))
            root->printError("Identifier is not a structure", lex);
    typeVarList();
    int type = scaner->scan(lex);
    if (type != TypeEndComma)
        scaner->printError(const_cast<char *>("Expected symbol ;"),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
}

void Diagram::typeConstList() {
    int type;
    TypeLex ident;
    do {
        type = scaner->scan(lex);
        if (type != TypeIdent) scaner->printError(const_cast<char *>("Expected identifier"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
        memcpy(ident, lex, sizeof(lex));
        Tree* v = root->semanticInclude(lex, OBJECT_TYPE::TYPE_VAR, dataType);

        type = scaner->scan(lex);
        if (type != TypeAssign) scaner->printError(const_cast<char *>("Expected symbol ="),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

        type = scaner->scan(lex);
        if (!isConst(type)) scaner->printError(const_cast<char *>("Expected constant"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
        root->semanticSetConst(v, true);
        root->semanticSetInit(v, true);
        root->semanticTypeCastCheck(ident, lex);
        root->semanticSetData(v, dataType, (char *)&lex);
        if ((type = lookForward(1)) == TypeComma)
            scaner->scan(lex);
    } while (type == TypeComma);
}

void Diagram::typeVarList() {
    int type;
    TypeLex ident;
    do {
        type = lookForward(2);
        if (type == TypeAssign) {
            if (dataType == DATA_TYPE::TYPE_DATASTRUCT)
                scaner->printError(const_cast<char *>("Expected variable type - int, short, long or double"),
                                   lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
            type = scaner->scan(lex);
            if (type != TypeIdent)
                scaner->printError(const_cast<char *>("Expected identifier"),
                                   lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
            memcpy(ident, lex, sizeof(lex));
            Tree *v = root->semanticInclude(lex, OBJECT_TYPE::TYPE_VAR, dataType);

            type = scaner->scan(lex);
            if (type != TypeAssign)
                scaner->printError(const_cast<char *>("Expected symbol ="),
                                   lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

            type = scaner->scan(lex);
            if (!isConst(type))
                scaner->printError(const_cast<char *>("Expected constant"),
                                   lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
            root->semanticSetInit(v, true);
            root->semanticSetData(v, dataType, (char *)&lex);
            root->semanticTypeCastCheck(ident, lex);
        }
        else {
            type = scaner->scan(lex);
            if (type != TypeIdent) scaner->printError(const_cast<char *>("Expected identifier"),
                    lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
            Tree* v = root->semanticInclude(lex, OBJECT_TYPE::TYPE_VAR, dataType);

            root->semanticSetInit(v, false);
            if (dataType == TYPE_DATASTRUCT) {
                root->semanticSetStruct(v, root->semanticGetStructData(structIdent));
                root->semanticSetInit(v, true);
            }

        }
        if ((type = lookForward(1)) == TypeComma)
            scaner->scan(lex);
    } while (type == TypeComma);
}

void Diagram::typeOperator() {
    int type = lookForward(1);
    if (type == TypeLeftFB)
        typeCompoundOperator();
    else if (type == TypeFor)
        typeFor();
    else if ((type == TypeIdent) && ((lookForward(2) == TypeAssign) || (lookForward(2) == TypeDot)))
        typeAssign();
    else if (type == TypeEndComma)
        scaner->scan(lex);
    else
        scaner->printError(const_cast<char *>("Expected compound operator or simple operator"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
}

void Diagram::typeCompoundOperator() {
    int type = scaner->scan(lex);
//    Tree* forReturn = Tree::cur;
    Tree* v = root->compoundOperator();
    do {
        type = lookForward(1);
        if (isOperator(type))
            typeOperator();
        else if (type == TypeConst)
            typeConstData();
        else if (isType(type))
            typeVarData();
        else
            scaner->printError(const_cast<char*>("Expected constant, variables or operator"),
                    lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

        type = lookForward(1);
    } while (type != TypeRightFB);
    scaner->scan(lex);
    //печать синтаксического дерева до его удаления
    std::cout << "\nPrint a compound operator before deleting:" << std::endl;
    root->print();
    //удаляем сложный оператор
    root->deleteTreeFrom(v);
    std::cout << "\nPrint a tree after deleting compound operator:" << std::endl;
    root->print();
//    root->setCur(forReturn);
}

void Diagram::typeAssign() {
    int type;
    TypeLex ident;
    DATA_TYPE dt;
    Tree* v = nullptr;
    bool flag = true; // флаг для первого нахождения правой ссылки структуры по идентификатору

    type = scaner->scan(lex);
    if (type != TypeIdent)
        scaner->printError(const_cast<char*>("Expected identifier"),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    memcpy(ident, lex, sizeof(lex));
    type = scaner->scan(lex);
    while (type == TypeDot) {
        if (flag) {
            flag = false;
            if (root->isStruct(Tree::cur, ident))
                root->printError("Found description of the structure (not object)", ident);
            v = root->semanticGetStructData(ident);
        }
        else
            v = root->semanticGetStructData(v);
        type = scaner->scan(lex);
        if (type != TypeIdent)
            scaner->printError(const_cast<char*>("Expected identifier"),
                               lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
        memcpy(ident, lex, sizeof(lex));
        v = root->semanticGetIdentifierInStruct(v, ident);
        if (v == nullptr)
            root->printError("Identifier not found", ident);
        type = scaner->scan(lex);
    }
    if (v != nullptr){
        root->checkConst(v);
        root->semanticSetInit(v, true);
        dt = root->getType(v);
    }
    else{
        root->checkConst(ident);
        v = root->findUp(Tree::cur, ident);
        root->semanticSetInit(v, true);
        dt = root->getType(ident);
    }
    // вывод значения до присваивания
//    root->printInfo(v, "Before assign: ");

    if (type != TypeAssign)
        scaner->printError(const_cast<char*>("Expected symbol ="),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    ExpresData* ed = new ExpresData();
    typeExpression(ed);

    root->semanticTypeCastCheck(dt, ed->dataType);
    root->semanticSetValue(v, ed);
    root->printInfo(v, "After assign: ");
//    printf("\n");
}

void Diagram::typeFor() {
    int type = scaner->scan(lex);

    if (type != TypeFor)
        scaner->printError(const_cast<char*>("Expected keyword \'for\'"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    type = scaner->scan(lex);
    if (type != TypeLeftRB)
        scaner->printError(const_cast<char*>("Expected symbol ("),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    type = scaner->scan(lex);
    if (type != TypeInt)
        scaner->printError(const_cast<char*>("Expected variable type \'int\'"),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    type = scaner->scan(lex);
    if (type != TypeIdent)
        scaner->printError(const_cast<char*>("Expected identifier"),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    Tree* iVar = root->semanticInclude(lex, OBJECT_TYPE::TYPE_VAR, DATA_TYPE::TYPE_INTEGER);

    type = scaner->scan(lex);
    if (type != TypeAssign)
        scaner->printError(const_cast<char*>("Expected symbol ="),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
    ExpresData* data = new ExpresData();
    typeExpression(data);
    // занести данные  в переменную i
    root->semanticSetInit(iVar, true);
    root->semanticSetValue(iVar, data);
    type = scaner->scan(lex);
    if (type != TypeEndComma)
        scaner->printError(const_cast<char*>("Expected symbol ;"),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    ExpresData* expr = new ExpresData();
    typeExpression(expr);
    type = scaner->scan(lex);
    if (type != TypeEndComma)
        scaner->printError(const_cast<char*>("Expected symbol ;"),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    typeAssign();

    type = scaner->scan(lex);
    if (type != TypeRightRB)
        scaner->printError(const_cast<char*>("Expected symbol )"),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    typeOperator();

    root->deleteTreeFrom(iVar);
}

void Diagram::typeExpression(ExpresData* ed) {
    typeXorEl(ed);

    int type = lookForward(1);
    while (type == TypeBitOr) {
        scaner->scan(lex);
        ExpresData* cmpData = new ExpresData();
        typeXorEl(cmpData);

        root->semanticMakeBiOperation(ed, cmpData, type);
        type = lookForward(1);
    }
}

void Diagram::typeXorEl(ExpresData* ed) {
    typeAndEl(ed);

    int type = lookForward(1);
    while (type == TypeBitXor) {
        scaner->scan(lex);
        ExpresData* cmpData = new ExpresData();
        typeAndEl(cmpData);
        root->semanticMakeBiOperation(ed, cmpData, type);
        type = lookForward(1);
    }
}

void Diagram::typeAndEl(ExpresData* ed) {
    typeComparesEl(ed);

    int type = lookForward(1);
    while (type == TypeBitAnd) {
        scaner->scan(lex);
        ExpresData* cmpData = new ExpresData();
        typeComparesEl(cmpData);
        root->semanticMakeBiOperation(ed, cmpData, type);
        type = lookForward(1);
    }
}

void Diagram::typeComparesEl(ExpresData* ed) {
    typeSummEl(ed);

    int type = lookForward(1);
    while (isCompare(type)) {
        scaner->scan(lex);
        ExpresData* cmpData = new ExpresData();
        typeSummEl(cmpData);
        root->semanticMakeBiOperation(ed, cmpData, type);
        type = lookForward(1);
    }
}

void Diagram::typeSummEl(ExpresData* ed) {
    typeMultEl(ed);

    int type = lookForward(1);
    while ((type == TypeAdd) || (type == TypeSub)) {
        scaner->scan(lex);
        ExpresData* cmpData = new ExpresData();
        typeMultEl(cmpData);
        root->semanticMakeBiOperation(ed, cmpData, type);
        type = lookForward(1);
    }
}

void Diagram::typeMultEl(ExpresData* ed) {
    typeElemExpression(ed);

    int type = lookForward(1);
    while ((type == TypeMul) || (type == TypeDiv) || (type == TypeDivPart)) {
        scaner->scan(lex);
        ExpresData* cmpData = new ExpresData();
        typeElemExpression(cmpData);
        root->semanticMakeBiOperation(ed, cmpData, type);
        type = lookForward(1);
    }
}

void Diagram::typeElemExpression(ExpresData* ed) {
    auto copyLex = lex;

    int type = lookForward(1);
    if (type == TypeLeftRB) {
        scaner->scan(lex);
        typeExpression(ed);
        type = scaner->scan(lex);
        if (type != TypeRightRB)
            scaner->printError(const_cast<char*>("Expected symbol )"),
                    lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
        return ;
    }
    else if (isConst(type)) {
        scaner->scan(lex);
        root->semanticGetStringValue(lex, ed);
        return ;
    }
    else if (type == TypeIdent) {
        int type;
        TypeLex ident;
        Tree* v = nullptr;
        bool flag = true; // флаг для первого нахождения правой ссылки структуры по идентификатору

        type = scaner->scan(lex);
        if (type != TypeIdent)
            scaner->printError(const_cast<char*>("Expected identifier"),
                               lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

        memcpy(ident, lex, sizeof(lex));
        if (lookForward(1) == TypeDot)
            type = scaner->scan(lex);
        while (type == TypeDot) {
            if (flag) {
                flag = false;
                v = root->semanticGetStructData(ident);
            }
            else
                v = root->semanticGetStructData(v);
            type = scaner->scan(lex);
            if (type != TypeIdent)
                scaner->printError(const_cast<char*>("Expected identifier"),
                                   lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
            memcpy(ident, lex, sizeof(lex));
            v = root->semanticGetIdentifierInStruct(v, ident);
            if (lookForward(1) == TypeDot)
                type = scaner->scan(lex);
        }
        if (v != nullptr){
            root->checkInit(v);
            root->semanticGetData(v, ed);
        }
        else{
            root->checkInit(ident);
            root->semanticGetData(root->semanticGetVar(ident), ed);
        }
        return ;
    }
    else
        scaner->printError(const_cast<char*>("Expected expression, constant or identifier"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
}
