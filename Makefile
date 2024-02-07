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
	@rm -fr $(TARGET) $(OBJDIR)

re: fclean $(TARGET)

git: fclean
	@rm request/folder/*
	git add .
	git commit -m "PARSSING CLIENT REQUEST ALMOST DONE : revived after delete"
	git push

.PHONY: clean fclean re