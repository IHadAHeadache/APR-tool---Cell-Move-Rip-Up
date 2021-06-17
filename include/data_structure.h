#ifndef _DATA_STRUCTURE_H
#define _DATA_STRUCTURE_H
#define Vertical 1
#define Horizontal 0
#define Fixed 1
#define Movable 0
#include <vector>
#include <string>
#include <map>

class Grid;
class MasterCell;
class Cell;
class Pin;
class Blockage;
class Net;
class Layer;
class VoltageArea;
class Design;

typedef struct coordinate
{
	int row, col;
	coordinate(int r, int c) :row(r), col(c){}
}coordinate;

typedef struct net_unit
{
	Cell* cell;
	Pin* pin;
	net_unit(Cell* c, Pin* p) :cell(c), pin(p) {}
}net_unit;

class Layer
{
public:
	Layer();
	Layer(std::string n, int col_n, int row_n, int direction, int ind, double ds, double pf_);
	Layer(const Layer& l);
	Layer& operator=(const Layer& l);
	~Layer();
//	int get_row_n();
//	int get_col_n();
//	int get_size();
//	void set_dir(int direction);
//	Grid* get_grids();
	Grid& at(int x, int y);
	//private:
	Grid* grids;
	std::string  name;
	int row_num, col_num, default_supply;
	int size;
	int dir;
	int index;
	double pf;
};

class Grid
{
public:
	Grid();
	Grid(int s);
//private:
	double demand, supply, overflow;
	std::map<std::string, Cell*> cells;
	std::map<std::string, Net*> nets;
};

class MasterCell
{
public:
	MasterCell();
	MasterCell(std::string NAME, int p_num, int b_num);
//private:
	int pin_num, block_num;
	std::string name;
	std::vector<Blockage> blockages;
	std::vector<Pin> pins;
};

class Cell
{
public:
	Cell();
	Cell(std::string NAME, int ROW, int COL, MasterCell* MC, int type);
//private:
	std::string name;
	int row, col;
	int movable;
	std::map<std::string, Blockage> blockages;
	std::map<std::string, Pin> pins;
	std::map<std::string, Net*> nets;
	VoltageArea* varea;

};

class Pin
{
public:
	Pin();
	Pin(std::string NAME, Layer* Layer);
	Pin(const Pin& p);
	std::string get_name();
//private:
	std::string name;
	Layer* layer;
};

class Blockage
{
public:
	Blockage();
	Blockage(std::string NAME, Layer* LAYER, int DEMAND);
	Blockage(const Blockage& b);
	//std::string get_name();
	//Layer* get_layer();
	int get_demand();
//private:
	std::string name;
	Layer* layer;
	int demand;
};

class Net
{
public:
	Net();
	Net(std::string Name, int p_num, std::string Constraint, double Weight);
	friend class Design;
//private:
	std::string constr;
	std::string name;
	int pin_num;
	double weight;
	std::vector<net_unit> nu;
	std::vector<std::vector<int> > routes;
};

class VoltageArea
{
public:
	VoltageArea();
	VoltageArea(std::string n);
	std::string name;
	std::vector<Cell*> cells;
	std::vector<coordinate> grids;
};

class Design
{
public:
	Design();
	int read_file(const char* filename);
	void set_dim(int rn, int cn);
//private:
	//std::vector<MasterCell> mcells;
	//std::vector<Cell> cells;
	//std::vector<Net> nets;
	//std::vector<Layer> layers;
	std::map<std::string, MasterCell> mcells;
	std::map<std::string, Layer> layers;
	std::map<std::string, Cell> cells;
	std::map<std::string, Net> nets;
	std::map<std::string, VoltageArea> vareas;
	int MaxCellMove;
	int row_num, col_num;
	int start_ri, end_ri, start_ci, end_ci;
	int layer_num, mcell_num, cell_num, net_num, varea_num;
};

void sstream_init(std::stringstream* ss);
#endif
