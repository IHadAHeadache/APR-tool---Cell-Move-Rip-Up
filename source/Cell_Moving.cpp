#include "Cell_Moving.h"
#include <math.h>
#include <cstdio>
#include <iostream>
#include <sstream>
template <typename T>
std::string NumberToString ( T Number )
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}
std::pair<int,int> Newc(Cell c)
{
double grids_count = 0, center_row = 0, center_col = 0;
double distance = 99999999, row, col; 
	if(c.varea != NULL)
	{
		for(std::vector<coordinate>::iterator it = c.varea->grids.begin(); it != c.varea->grids.end(); ++it)
		{
			center_row += it->row; center_col += it->col;
			grids_count++;
		}
		center_row = center_row / grids_count;
		center_col = center_col / grids_count;
		for(std::vector<coordinate>::iterator it = c.varea->grids.begin(); it != c.varea->grids.end(); ++it)
		{
			if(distance > pow(it->row-center_row,2) + pow(it->col-center_col,2))
			{
				distance = pow(it->row-center_row,2) + pow(it->col-center_col,2);
				row = it->row;
				col = it->col;
			}
		}
	}
	else
	{
	double x, y;
	int cell_count;
	std::vector<int> gravity_x;
	std::vector<int> gravity_y;
		for(std::map<std::string,Net*>::iterator it = c.nets.begin(); it != c.nets.end(); ++it)
		{
			x = 0; y = 0;
			cell_count = 0;
			for(std::vector<net_unit>::iterator that = (*(it->second)).nu.begin(); that != (*(it->second)).nu.end(); ++that)
			{
				if(that->cell->name != c.name)
				{
					x += that->cell->row; y+= that->cell->col;
					cell_count++;
				}
			}
			gravity_x.push_back((int)round(x/cell_count));
			gravity_y.push_back((int)round(y/cell_count));
		}
		std::sort(gravity_x.begin(),gravity_x.end());
		std::sort(gravity_y.begin(),gravity_y.end());
		if(gravity_x.size() % 2 == 1)
			return std::make_pair(gravity_x.at(gravity_x.size()/2),gravity_y.at(gravity_y.size()/2));
		else
			return std::make_pair((gravity_x.at(gravity_x.size()/2-1)+gravity_x.at(gravity_x.size()/2))/2 , (gravity_y.at(gravity_y.size()/2-1)+gravity_y.at(gravity_y.size()/2))/2);
	}
	return std::make_pair ((int)row,(int)col);
}
std::pair < double,std::pair <int,int> > Calculate_DeltaWL(Cell c)
{
std::pair <int,int> old_coor (c.row,c.col);
std::pair <int,int> new_coor = Newc(c);
double DeltaWL = 0;
double Total_DeltaWL = 0;
int x, y;
int cell_count;
	for(std::map<std::string, Net*>::iterator it=c.nets.begin(); it!=c.nets.end(); ++it)
	{
		x = 0; y = 0;
		cell_count = 0;
		for(std::vector<net_unit>::iterator that = (*(it->second)).nu.begin(); that != (*(it->second)).nu.end(); ++that)
		{
			if(that->cell->name != c.name)
			{
				x += that->cell->row; y+= that->cell->col;
				cell_count++;
			}
		}
		x = (int)round((double)x/cell_count);
		y = (int)round((double)y/cell_count);
		DeltaWL = pow((old_coor.first-x)*(old_coor.first-x)+(old_coor.second-y)*(old_coor.second-y),0.5) - pow((new_coor.first-x)*(new_coor.first-x)+(new_coor.second-y)*(new_coor.second-y),0.5);
		Total_DeltaWL += DeltaWL;
	}
	return std::make_pair(Total_DeltaWL,new_coor);
}
std::vector<std::string> MoveCell(std::map<std::string, Cell>& cells, int MaxCellMove)
{
std::vector< std::pair<double,std::string> > pq;
std::map<std::string,std::pair<int,int> > coor;
std::vector<std::string> cell_name;
	for(std::map<std::string,Cell>::iterator it = cells.begin(); it != cells.end(); ++it)
	{
	if((it->second).movable == 1) continue;
	std::pair< double,std::pair<int,int> > deltawl_coor;
		deltawl_coor = Calculate_DeltaWL(it->second);
		pq.push_back(std::make_pair(deltawl_coor.first,it->first));
		coor.insert(std::pair<std::string,std::pair<int,int> >(it->first,deltawl_coor.second));
	}
	for(std::vector< std::pair<double,std::string> > ::iterator it = pq.begin();it!=pq.end();++it)
		printf("deltaWL:%lf\n",(*it).first);
	for (int i = 0; i < MaxCellMove;++i)
  	{
	double max = -9999999;
	std::string max_cell;
	int max_index;
	for(int j = 0; j < pq.size();++j)
	{
		if(pq.at(j).first >= max)
		{
			max = pq.at(j).first;
			max_cell = pq.at(j).second;
			max_index = j;
		}
	}
	if(max <= 0)
		break;
	pq.erase(pq.begin()+max_index);
	int row = cells[max_cell].row;
	int col = cells[max_cell].col;
		for(std::map<std::string, Blockage>::iterator it = cells[max_cell].blockages.begin(); it != cells[max_cell].blockages.end(); ++it)
		{
		double* temp_demand = &((*((it->second).layer)).at(col,row).demand);
			*temp_demand -= (it->second).get_demand();
		}
		cells[max_cell].row = coor[max_cell].first;
		cells[max_cell].col = coor[max_cell].second;
		row = cells[max_cell].row;
		col = cells[max_cell].col;
		for(std::map<std::string, Blockage>::iterator it = cells[max_cell].blockages.begin(); it != cells[max_cell].blockages.end(); ++it)
		{
		double* temp_demand = &((*((it->second).layer)).at(col,row).demand);
			*temp_demand += (it->second).get_demand();
		}
		cell_name.push_back(max_cell);
  	}
	return cell_name;
}
unsigned int decrement[2000][2000][1]= {{{0}}};
void Rip_up(std::vector<std::string> cell_name,std::map<std::string, Cell>& cells,std::map<std::string, Layer>& layers)
{
int x1,y1,z1,x2,y2,z2;
std::string layer;
	for(std::vector<std::string>::iterator it = cell_name.begin();it != cell_name.end();++it)
		for(std::map<std::string, Net*>::iterator that = cells[*it].nets.begin();that != cells[*it].nets.end();++that)
		{
			if((that->second->routes).empty()) continue;
			for(std::vector<std::vector<int> >::iterator those = (that->second->routes).begin(); those != (that->second->routes).end();++those)
			{
				for(int i = 0; i < 6;++i)
				{
					if(i==0)
						x1 = (*those).at(i);
					else if(i==1)
						y1 = (*those).at(i);
					else if(i==2)
						z1 = (*those).at(i);
					else if(i==3)
						x2 = (*those).at(i);
					else if(i==4)
						y2 = (*those).at(i);
					else if(i==5)
						z2 = (*those).at(i);
				}
				if(x1!=x2)
				{
					//layer = "M" + std::to_string(z1);
					layer = "M" + NumberToString(z1);
					if(x2 > x1)
					{
						for(int i = x1; i <= x2; ++i)
							if(((decrement[i][y1][0] >> (z1-1)) & 1U) == 0)
							{
								layers[layer].at(y1,i).demand--;
								layers[layer].at(y1,i).nets.erase(that->first);
								decrement[i][y1][0] |= 1UL << (z1-1);
							}
					}
					else
					{
						for(int i = x1; i >= x2; --i)
							if(((decrement[i][y1][0] >> (z1-1)) & 1U) == 0)
							{
								layers[layer].at(y1,i).demand--;
								layers[layer].at(y1,i).nets.erase(that->first);
								decrement[i][y1][0] |= 1UL << (z1-1);
							}
					}
				}
				else if(y1!=y2)
				{
					//layer = "M" + std::to_string(z1);
					layer = "M" + NumberToString(z1);
					if(y2 > y1)
					{
						for(int i = y1; i <= y2; ++i)
							if(((decrement[x1][i][0] >> (z1-1)) & 1U) == 0)
							{
								layers[layer].at(i,x1).demand--;
								layers[layer].at(i,x1).nets.erase(that->first);
								decrement[x1][i][0] |= 1UL << (z1-1);
							}
					}
					else
					{
						for(int i = y1; i >= y2; --i)
							if(((decrement[x1][i][0] >> (z1-1)) & 1U) == 0)
							{
								layers[layer].at(i,x1).demand--;
								layers[layer].at(i,x1).nets.erase(that->first);
								decrement[x1][i][0] |= 1UL << (z1-1);
							}
					}
				}
				else
				{
					if(z2 > z1)
						for(int i = z1; i <= z2; ++i)
						{
							//layer = "M" + std::to_string(i);
							layer = "M" + NumberToString(i);
							if(((decrement[x1][y1][0] >> (i-1)) & 1U) == 0)
							{
								layers[layer].at(y1,x1).demand--;
								layers[layer].at(y1,x1).nets.erase(that->first);
								decrement[x1][y1][0] |= 1UL << (i-1);
							}
						}
					else
					{
						for(int i = z1; i >= z2; --i)
						{
							//layer = "M" + std::to_string(i);
							layer =  "M" + NumberToString(i);
							if(((decrement[x1][y1][0] >> (i-1)) & 1U) == 0)
							{
								layers[layer].at(y1,x1).demand--;
								layers[layer].at(y1,x1).nets.erase(that->first);
								decrement[x1][y1][0] |= 1UL << (i-1);
							}
						}
					}
				}
			}
			(that->second->routes).clear();
			for(int i = 0; i < 2000; ++i)
				for(int j = 0; j < 2000; ++j)
					for(int k = 0; k < 1; ++k)
						decrement[i][j][k] = 0;
		}	
}
void MovedCell_Output(std::vector<std::string> cell_name, std::map<std::string, Cell> cells)
{
	printf("NumMovedCellInst %lu\n",cell_name.size());
	for(std::vector<std::string>::iterator it = cell_name.begin(); it!= cell_name.end(); ++it)
	{
		printf("CellInst ");	
		std::cout << *it;
		printf(" %d",cells[*it].row);
		printf(" %d\n",cells[*it].col);
	}
}
