//
// Created by kolya on 10/21/2021.
//

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
        //root->semanticSetData(v, (char *)&lex);
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
            root->semanticSetData(v, nullptr);
            root->semanticTypeCastCheck(ident, lex);
        }
        else {
            type = scaner->scan(lex);
            if (type != TypeIdent) scaner->printError(const_cast<char *>("Expected identifier"),
                    lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
            Tree* v = root->semanticInclude(lex, OBJECT_TYPE::TYPE_VAR, dataType);

            root->semanticSetData(v, nullptr);
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
    root->setCur(v);
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
            if (root->isStruct(root->cur, ident))
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

    if (type != TypeAssign)
        scaner->printError(const_cast<char*>("Expected symbol ="),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    DATA_TYPE dt1 = typeExpression();
    root->semanticTypeCastCheck(dt, dt1);
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

    type = scaner->scan(lex);
    if (type != TypeAssign)
        scaner->printError(const_cast<char*>("Expected symbol ="),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    typeExpression();

    type = scaner->scan(lex);
    if (type != TypeEndComma)
        scaner->printError(const_cast<char*>("Expected symbol ;"),
                           lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());

    typeExpression();

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
}

DATA_TYPE Diagram::typeExpression() {
    DATA_TYPE dt = typeXorEl();

    int type = lookForward(1);
    while (type == TypeBitOr) {
        scaner->scan(lex);
        DATA_TYPE dt1 = typeXorEl();
        dt = max(dt1, dt);
        type = lookForward(1);
    }

    return dt;
}

DATA_TYPE Diagram::typeXorEl() {
    DATA_TYPE dt = typeAndEl();

    int type = lookForward(1);
    while (type == TypeBitXor) {
        scaner->scan(lex);
        DATA_TYPE dt1 = typeAndEl();
        dt = max(dt1, dt);
        type = lookForward(1);
    }

    return dt;
}

DATA_TYPE Diagram::typeAndEl() {
    DATA_TYPE dt = typeComparesEl();

    int type = lookForward(1);
    while (type == TypeBitAnd) {
        scaner->scan(lex);
        DATA_TYPE dt1 = typeComparesEl();
        dt = max(dt1, dt);
        type = lookForward(1);
    }

    return dt;
}

DATA_TYPE Diagram::typeComparesEl() {
    DATA_TYPE dt = typeSummEl();

    int type = lookForward(1);
    while (isCompare(type)) {
        scaner->scan(lex);
        DATA_TYPE dt1 = typeSummEl();
        dt = max(dt1, dt);
        type = lookForward(1);
    }

    return dt;
}

DATA_TYPE Diagram::typeSummEl() {
    DATA_TYPE dt = typeMultEl();

    int type = lookForward(1);
    while ((type == TypeAdd) || (type == TypeSub)) {
        scaner->scan(lex);
        DATA_TYPE dt1 = typeMultEl();
        dt = max(dt1, dt);
        type = lookForward(1);
    }

    return dt;
}

DATA_TYPE Diagram::typeMultEl() {
    DATA_TYPE dt = typeElemExpression();

    int type = lookForward(1);
    while ((type == TypeMul) || (type == TypeDiv) || (type == TypeDivPart)) {
        scaner->scan(lex);
        DATA_TYPE dt1 = typeElemExpression();
        dt = max(dt1, dt);
        type = lookForward(1);
    }

    return dt;
}

DATA_TYPE Diagram::typeElemExpression() {
    DATA_TYPE dt;
    auto copyLex = lex;

    int type = lookForward(1);
    if (type == TypeLeftRB) {
        scaner->scan(lex);
        dt = typeExpression();
        type = scaner->scan(lex);
        if (type != TypeRightRB)
            scaner->printError(const_cast<char*>("Expected symbol )"),
                    lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
        return dt;
    }
    else if (isConst(type)) {
        scaner->scan(lex);
        return root->getType(lex);
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
            return root->getType(v);
        }
        else{
            root->checkInit(ident);
            return root->getType(ident);
        }

    }
    else
        scaner->printError(const_cast<char*>("Expected expression, constant or identifier"),
                lex, scaner->getLine(), scaner->getPos() - scaner->getNewLinePos());
}
