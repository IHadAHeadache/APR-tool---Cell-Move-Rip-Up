#include "data_structure.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
Grid::Grid()
{
	this->demand = 0;
	this->supply = 0;
	this->overflow = 0;
}

Grid::Grid(int s)
{
	this->supply = s;
	this->demand = 0;
	this->overflow = 0;
}

MasterCell::MasterCell()
{
	this->pin_num = 0;
	this->block_num = 0;
}

MasterCell::MasterCell(std::string NAME, int p_num, int b_num)
{
	this->name = NAME;
	this->pin_num = p_num;
	this->block_num = b_num;
	this->pins.reserve(p_num);
	this->blockages.reserve(b_num);
}

Blockage::Blockage()
{
	this->demand = -1;
	this->layer = NULL;
}

Blockage::Blockage(std::string NAME, Layer* LAYER, int DEMAND)
{
	this->name = NAME;
	this->layer = LAYER;
	this->demand = DEMAND;
}

Blockage::Blockage(const Blockage& b)
{
	this->name = b.name;
	this->demand = b.demand;
	this->layer = b.layer;
}

int Blockage::get_demand()
{
	return this->demand;
}

Cell::Cell()
{
	this->row = this->col = -1;
	this->movable = Fixed;
	this->varea = NULL;
}

Cell::Cell(std::string NAME, int ROW, int COL, MasterCell* MC, int type)
{
	this->name = NAME;
	this->row = ROW;
	this->col = COL;
	this->movable = type;
	for (std::vector<Blockage>::iterator iter = MC->blockages.begin(); iter != MC->blockages.end(); iter++)
		this->blockages[iter->name] = *iter;
	for (std::vector<Pin>::iterator iter = MC->pins.begin(); iter != MC->pins.end(); iter++)
		this->pins[iter->name] = *iter;
	this->varea = NULL;
}

Pin::Pin()
{
	this->name = "";
	this->layer = NULL;
}

Pin::Pin(std::string NAME, Layer* Layer)
{
	this->name = NAME;
	this->layer = Layer;
}

Pin::Pin(const Pin& p)
{
	this->name = p.name;
	this->layer = p.layer;
}

Net::Net()
{
	this->constr = -1;
	this->name = "";
	this->pin_num = -1;
}

Net::Net(std::string Name, int p_num, std::string Constraint, double Weight)
{
	this->name = Name;
	this->pin_num = p_num;
	this->nu.reserve(p_num);
	this->constr = Constraint;
	this->weight = Weight;
}

Layer::Layer()
{
	this->name = "default";
	this->row_num = this->col_num = this->size = this->dir = this->index = -1;
	this->pf = -1.0;
	this->grids = NULL;
}

Layer::Layer(std::string n, int row_n, int col_n, int direction, int ind, double ds, double pf_)
{
	this->name = n;
	this->row_num = row_n;
	this->col_num = col_n;
	this->grids = new Grid[row_n * col_n];
	this->size = row_n * col_n;
	this->index = ind;
	this->pf = pf_;
	this->dir = direction;
	this->default_supply = ds;
	for (int i = 0; i < this->size; ++i)
		this->grids[i].supply = ds; //****
}

Layer::Layer(const Layer& l)
{
	this->col_num = l.col_num;
	this->default_supply = l.default_supply;
	this->dir = l.dir;
	this->index = l.index;
	this->pf = l.pf;
	this->size = l.size;
	this->name = l.name;
	this->row_num = l.row_num;
	this->col_num = l.col_num;
	this->grids = new Grid[size];

	for (int i = 0; i < l.size; ++i)
	{
		this->grids[i].supply = l.grids[i].supply;
		this->grids[i].demand = l.grids[i].demand;
		this->grids[i].overflow = l.grids[i].overflow;
	}
}

Layer& Layer::operator=(const Layer& l)
{
	this->col_num = l.col_num;
	this->default_supply = l.default_supply;
	this->dir = l.dir;
	this->index = l.index;
	this->pf = l.pf;
	this->size = l.size;
	this->name = l.name;
	this->row_num = l.row_num;
	this->col_num = l.col_num;
	this->grids = new Grid[l.size];
	for (int i = 0; i < l.size; ++i)
	{
		this->grids[i].supply = l.grids[i].supply;
		this->grids[i].demand = l.grids[i].demand;
		this->grids[i].overflow = l.grids[i].overflow;
	}
	return*this;
}

Layer::~Layer()
{
	delete[] this->grids;
}


Grid& Layer::at(int x, int y)
{
	return this->grids[x + (col_num * y)];
}

VoltageArea::VoltageArea()
{
	this->name = "";
}

VoltageArea::VoltageArea(std::string n)
{
	this->name = n;
}

int Design::read_file(const char* filename)
{
	std::fstream f;
	f.open(filename);
	if (!f)	return 0;
	std::string line, pattern;
	std::stringstream ss;
	while (f.peek() != EOF)
	{
		pattern = "";
		getline(f, line);
		sstream_init(&ss);
		ss << line;
		ss >> pattern;
		if (pattern == "MaxCellMove")
		{
			//std::cout << "MaxCellMove\n";
			ss >> this->MaxCellMove;
		}
		else if (pattern == "GGridBoundaryIdx")
		{
			//std::cout << "GGriBoundaryIdx\n";
			ss >> this->start_ri >> this->start_ci >> this->end_ri >> this->end_ci;
			this->set_dim(end_ri - start_ri + 1, end_ci - start_ci + 1);
		}
		else if (pattern == "NumLayer")
		{
			//std::cout << "NumLayer\n";
			std::string name;
			int index, supply;
			char dir;
			double pf;
			ss >> this->layer_num;
			for (int i = 0; i < layer_num; ++i)
			{
				getline(f, line);
				sstream_init(&ss);
				ss << line;
				ss >> line >> name >> index >> dir >> supply >> pf;
				this->layers.insert(std::pair<std::string, Layer>(name, Layer(name, this->row_num, this->col_num, (dir == 'H') ? Horizontal : Vertical, index, supply, pf)));
			}
		}
		else if (pattern == "NumNonDefaultSupplyGGrid")
		{
			//std::cout << "NumNonDefaultSupplyGGrid\n";
			int v;
			ss >> v;
			for (int i = 0; i < v; ++i)
			{
				getline(f, line);
				sstream_init(&ss);
				ss << line;
				int row, col, index, increment;
				ss >> row >> col >> index >> increment;
				char layer[10];
				sprintf(layer, "M%d", index);
				this->layers[std::string(layer)].at(col - this->start_ci, row - this->start_ri).supply += increment;
			}
		}
		else if (pattern == "NumMasterCell")
		{
			//std::cout << "NumMasterCell\n";
			ss >> this->mcell_num;
			std::string mcname, pinname, layername, blkname;
			int npin, nblk, blkdemand;
			for (int i = 0; i < this->mcell_num; ++i)
			{
				getline(f, line);
				sstream_init(&ss);
				ss << line;
				ss >> line >> mcname >> npin >> nblk;
				//this->addmc(mcname, npin, nblk);
				this->mcells[mcname] = MasterCell(mcname, npin, nblk);
				MasterCell* mc = &this->mcells[mcname];
				for (int i = 0; i < npin; ++i)
				{
					getline(f, line);
					sstream_init(&ss);
					ss << line;
					ss >> line >> pinname >> layername;
					mc->pins.push_back(Pin(pinname, &this->layers[layername]));
				}
				for (int i = 0; i < nblk; ++i)
				{
					getline(f, line);
					sstream_init(&ss);
					ss << line;
					ss >> line >> blkname >> layername >> blkdemand;
					mc->blockages.push_back(Blockage(blkname, &this->layers[layername], blkdemand));
				}
			}
		}
		else if (pattern == "NumCellInst")
		{
			//std::cout << "NumCellInst\n";
			int row, col;
			std::string name, mcname, constr;
			ss >> this->cell_num;
			for (int i = 0; i < this->cell_num; ++i)
			{
				getline(f, line);
				sstream_init(&ss);
				ss << line;
				ss >> line >> name >> mcname >> row >> col >> constr;
				row -= start_ri; // row = row - start_row_index
				col -= start_ci;
				MasterCell* mc = &this->mcells[mcname];
				this->cells[name] = Cell(name, row, col, mc, (constr == "Fixed") ? Fixed : Movable);
				//compute demand & overflow
				for (std::vector<Blockage>::iterator iter = mc->blockages.begin(); iter != mc->blockages.end(); iter++)
				{
					iter->layer->at(col, row).demand += iter->demand;
					iter->layer->at(col, row).overflow = std::max(0.0, iter->layer->at(col, row).demand - iter->layer->at(col, row).supply);
				}
			}
		}
		else if (pattern == "NumNets")
		{
			//std::cout << "NumNets\n";
			int npin;
			ss >> this->net_num;
			std::string name, constr;
			double weight;
			for (int i = 0; i < this->net_num; ++i)
			{
				getline(f, line);
				sstream_init(&ss);
				ss << line;
				ss >> line >> name >> npin >> constr >> weight;
				//this->nets[name] = Net(name, npin, constr, weight);
				//Net* n = &this->nets[name];
				std::map<std::string, Net>::iterator n = this->nets.insert(std::pair<std::string, Net>(name, Net(name, npin, constr, weight))).first;
				for (int j = 0; j < npin; ++j)
				{
					std::string cellname, pinname;
					getline(f, line);
					sstream_init(&ss);
					ss << line;
					ss >> line >> line;
					int pos = line.find("/");
					cellname = line.substr(0, pos);
					pinname = line.substr(pos + 1, line.length() - pos - 1);
					//n->nu.push_back(net_unit(&cells[cellname], &(cells[cellname].pins[pinname])));
					n->second.nu.push_back(net_unit(&cells[cellname], &(cells[cellname].pins[pinname])));
					this->cells[cellname].nets.insert(std::pair<std::string, Net*>(name, &(n->second)));
				}
			}
		}
		else if (pattern == "NumRoutes")
		{
			//std::cout << "NumRountes\n";
			int nroute;
			ss >> nroute;
			for (int i = 0; i < nroute; ++i)
			{
				std::vector<int> route(6);
				std::string netname;
				getline(f, line);
				sstream_init(&ss);
				ss << line;
				ss >> route[0] >> route[1] >> route[2] >> route[3] >> route[4] >> route[5] >> netname;
				route[0] -= this->start_ri;
				route[3] -= this->start_ri;
				route[1] -= this->start_ci;
				route[4] -= this->start_ci;
				this->nets[netname].routes.push_back(route); //may roughly estimate the space before push_back
				int start_row = std::min(route[0], route[3]);
				int end_row = std::max(route[0], route[3]);
				int start_col = std::min(route[1], route[4]);
				int end_col = std::max(route[1], route[4]);
				if (route[2] == route[5]) //x,y direction
				{
					char startlayer[10];
					sprintf(startlayer, "M%d", route[2]);
					std::map<std::string, Layer>::iterator layer = this->layers.find(std::string(startlayer));
					for (int row = start_row; row <= end_row; ++row)
					{
						for (int col = start_col; col <= end_col; ++col)
						{
							layer->second.at(col, row).nets.insert(std::pair<std::string, Net*>(netname, &(this->nets[netname])));
						}
					}
				}
				else //z direction
				{
					int start_layer = std::min(route[2], route[5]);
					int end_layer = std::max(route[2], route[5]);
					for (int l = start_layer; l <= end_layer; ++l)
					{
						char startlayer[10];
						sprintf(startlayer, "M%d", l);
						this->layers[std::string(startlayer)].at(start_col, start_row).nets.insert(std::pair<std::string, Net*>(netname, &(this->nets[netname])));
					}
				}
			}


			//compute demand
			for (std::map<std::string, Layer>::iterator layer = this->layers.begin(); layer != this->layers.end(); layer++)
				for (int i = 0; i < layer->second.size; ++i)
					layer->second.grids[i].demand += layer->second.grids[i].nets.size();
			//computer overflow
			for (std::map<std::string, Layer>::iterator layer = this->layers.begin(); layer != this->layers.end(); layer++)
				for (int i = 0; i < layer->second.size; ++i)
					layer->second.grids[i].overflow = std::max(0.0, layer->second.grids[i].demand - layer->second.grids[i].supply);
		}
		else if (pattern == "NumVoltageAreas")
		{
			ss >> this->varea_num;
			for (int i = 0; i < this->varea_num; ++i)
			{
				std::string name;
				int gridcount, instcount, row, col;
				getline(f, line);
				sstream_init(&ss);
				ss << line;
				ss >> line >> name;
				std::map<std::string, VoltageArea>::iterator pos = this->vareas.insert(std::pair<std::string, VoltageArea>(name, VoltageArea(name))).first;
				getline(f, line);
				sstream_init(&ss);
				ss << line;
				ss >> line >> gridcount;
				pos->second.grids.reserve(gridcount);
				for (int gc = 0; gc < gridcount; ++gc)
				{
					getline(f, line);
					sstream_init(&ss);
					ss << line;
					ss >> row >> col;
					pos->second.grids.push_back(coordinate(row - this->start_ri, col - this->start_ci));
				}
				getline(f, line);
				sstream_init(&ss);
				ss << line;
				ss >> line >> instcount;
				pos->second.cells.reserve(instcount);
				for (int instc = 0; instc < instcount; ++instc)
				{
					f >> name;
					this->cells[name].varea = &(pos->second);
					pos->second.cells.push_back(&(this->cells[name]));
				}
			}

		}
	}

	return 1;
}

Design::Design()
{
	this->cell_num = this->layer_num = this->mcell_num = this->net_num = this->varea_num = -1;
	this->row_num = this->col_num = -1;
	this->MaxCellMove = -1;
	this->start_ci = this->start_ri = this->end_ci = this->end_ri = -1;
}

void Design::set_dim(int rn, int cn)
{
	this->row_num = rn;
	this->col_num = cn;
}

void sstream_init(std::stringstream* ss)
{
	ss->str("");
	ss->clear();
}
