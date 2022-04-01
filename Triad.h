//
// Created by kolya on 3/31/2022.
//

#ifndef ANALYZATOR_TRIAD_H
#define ANALYZATOR_TRIAD_H


class Operand {
    int index;
    bool isLink = 0;    //  ссылка на операнд
};

class Triad {
    int uid;        // уникальный идентификатор
    int operation;  // код операции
    Operand operands[2];
};


#endif //ANALYZATOR_TRIAD_H
