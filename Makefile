CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

NAME = nexus-fileid

SRC = src/main.cpp \
	src/FileID.cpp \
	src/FileReport.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re