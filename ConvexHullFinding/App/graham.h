#include <vector>
#include <utility>
#include "forvis.h"
#include "algorithmthread.h"

#ifndef GRAHAM_H
#define GRAHAM_H

void Graham_algorithm(std::vector<std::pair<double, double>>& P, struct ForVis* for_vis, std::shared_ptr<std::vector<std::pair<double, double>>> current_stack);

#endif