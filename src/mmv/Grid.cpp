//
// Created by lucas on 04/12/23.
//

#include "Grid.h"

int Grid::Index(int i, int j) const {
    return rows * i + j;
}

bool Grid::Inside(int i, int j) const {
    return (i >= 0 && i < rows) && (j >= 0 && j < cols);
}
