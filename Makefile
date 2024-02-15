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
	@rm -f request/folder/*
	@rm -fr $(TARGET) $(OBJDIR)

re: fclean $(TARGET)

git: fclean
	@rm -f request/folder/*
	git add .
	git commit -m "PARSSING CLIENT REQUEST ALMOST DONE : binary chunk file transfert : image OK , video need to be tested"
	git push

.PHONY: clean fclean re