#include "set_utils.h"
#include <stdio.h>

Pointer set_find_eq_or_greater(Set set, Pointer value){
    Pointer value_pointer=set_find(set, value);
    if(value_pointer != NULL)
        return value_pointer;
    set_insert(set, value);
    SetNode node= set_find_node(set, value);
    node = set_next(set, node);
    value_pointer = set_node_value(set, node);
    return value_pointer;
}

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    Pointer value_pointer=set_find(set, value);
    if(value_pointer != NULL)
        return value_pointer;
    set_insert(set, value);
    SetNode node= set_find_node(set, value);
    node = set_previous(set, node);
    value_pointer = set_node_value(set, node);
    return value_pointer;
}