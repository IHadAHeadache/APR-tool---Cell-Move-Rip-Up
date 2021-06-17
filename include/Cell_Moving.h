#ifndef CELL_MOVING_H
#define CELL_MOVING_H
#include "data_structure.h"
std::pair<int,int> Newc(Cell);
std::pair < double,std::pair <int,int> > Calculate_DeltaWL(Cell);
std::vector<std::string> MoveCell(std::map<std::string, Cell>&, int);
void Rip_up(std::vector<std::string>, std::map<std::string, Cell>&, std::map<std::string, Layer>&);
void MovedCell_Output(std::vector<std::string>, std::map<std::string, Cell>);
#endif
