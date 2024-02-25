#pragma once

#include <utility>

constexpr int get1DIndex(int i, int j, int num_columns)
{
    return i * num_columns + j;
}

constexpr std::pair<int, int> get2DIndex(int index, int num_columns)
{
    int i = index / num_columns;
    int j = index % num_columns;
    return {i, j};
}