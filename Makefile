#Makefile
SRCS := $(wildcard ./source/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

#########################################
TARGET = cell_move_router
CXX = g++ -O3 -Wall -g
INCLUDE = -I $(SRC_INC)
#LIBRARY = 
##########################################
SRC_INC = ./include/
SRC_SRC = ./source/
##########################################

$(TARGET): $(OBJS)
	$(CXX) $^ $(LIBARAY) -o $@
%.o : %.cpp
	$(CXX) $(INCLUDE) -c $< -o $@
.PHONY : clean
clean:
	rm -f $(TARGET) $((OBJS)
