TARGET  = a.out

CC      = c++

OBJDIR  = obj

# CPPFLAG = -Werror -Wall -Wextra -std=c++98 -g3 -fsanitize=address

SRC     = main.cpp Server.cpp HTTPRequest.cpp

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

git:
	git add .
	git commit -m "Got stuck at separated header-body request"
	git push

.PHONY: clean fclean re