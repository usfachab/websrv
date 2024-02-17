TARGET  = a.out

CC      = c++ -fsanitize=address -g3

OBJDIR  = obj

SRC     = main.cpp ./server/Server.cpp ./request/HTTPRequestParser.cpp

OBJ     = $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f data/*
	@rm -fr $(TARGET) $(OBJDIR)

re: fclean $(TARGET)

git: fclean
	@rm -f data/*
	git add .
	git commit -m "PARSSING CLIENT POST REQUEST DONE : Large video uploaded -> OK"
	git push -f

.PHONY: clean fclean re