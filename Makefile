TARGET  = a.out

CC      = c++ -fsanitize=address -g3

OBJDIR  = obj

SRC     = main.cpp ./server/Server.cpp ./request/HTTPRequest.cpp

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
	@rm -f request/folder/*
	@rm -fr $(TARGET) $(OBJDIR)

re: fclean $(TARGET)

git: fclean
	@rm -f request/folder/*
	git add .
	git commit -m "PARSSING CLIENT REQUEST ALMOST DONE : start parsing query strings"
	git push

.PHONY: clean fclean re