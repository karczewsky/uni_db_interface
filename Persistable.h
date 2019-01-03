//
// Created by jakub on 29/12/18.
//

#ifndef DZIEKANAT_PERSISTABLE_H
#define DZIEKANAT_PERSISTABLE_H


class Persistable {
public:
    virtual void insert_db() = 0;
    virtual void update_db() = 0;
    virtual void remove_db() = 0;
};


#endif //DZIEKANAT_PERSISTABLE_H
