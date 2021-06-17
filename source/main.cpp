#include "Cell_Moving.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdio>
int main(int argc,char** argv)
{
	Design test;

	if(argc < 2)
	{
		std::cout << "Error : need input file \n" ;
		return 1;
	}

	std::string bench_path = "./benchmark/";
	std::string file_in = bench_path + argv[1];
	std::cout << file_in << std::endl;
	if(!test.read_file(file_in.c_str())) 
	{
		std::cout << "can't open file :" << file_in << "\n"; 
		//std::cout << "error\n";
		return 1;
	}
//std::pair< double, std::pair <int,int> > test_ch;
//std::pair <int,int> test_c;
	//test_c = Newc(test.cells["C3"]); 
	//printf("%d %d\n",test_c.first,test_c.second);
	//test_ch = Calculate_DeltaWL(test.cells["C3"]);
	//printf("%lf %d %d\n",test_ch.first,test_ch.second.first,test_ch.second.second);
	std::vector<std::string> MovedCell_List = MoveCell(test.cells,test.MaxCellMove);
	MovedCell_Output(MovedCell_List,test.cells);
std::cout << "Demand :\n";
	for (std::map<std::string, Layer>::iterator iter = test.layers.begin(); iter != test.layers.end(); iter++)
	{
		std::cout << iter->first << "\n";
		for (int i = 0; i < test.row_num; ++i)
		{
			for (int j = 0; j < test.col_num; ++j)
			{
				std::cout << std::setw(2) << iter->second.at(j, i).demand << " ";
			}
			std::cout << "\n";
		}
	}
	Rip_up(MovedCell_List,test.cells,test.layers);
	std::cout << "New Demand :\n";
	for (std::map<std::string, Layer>::iterator iter = test.layers.begin(); iter != test.layers.end(); iter++)
	{
		std::cout << iter->first << "\n";
		for (int i = 0; i < test.row_num; ++i)
		{
			for (int j = 0; j < test.col_num; ++j)
			{
				std::cout << std::setw(2) << iter->second.at(j, i).demand << " ";
			}
			std::cout << "\n";
		}
	}
	return 0;
}
