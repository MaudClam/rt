NAME	= rt


SRC_DIR 	= srcs

HEADER_DIR	= srcs

OBJ_DIR		= obj

SRC		= ${SRC_DIR}/main.cpp \
			${SRC_DIR}/A_Scenery.cpp \
			${SRC_DIR}/ARGBColor.cpp \
			${SRC_DIR}/camera.cpp \
			${SRC_DIR}/geometry.cpp \
			${SRC_DIR}/Light.cpp \
			${SRC_DIR}/MlxImage.cpp \
			${SRC_DIR}/Ray.cpp \
			${SRC_DIR}/Scene.cpp \
			${SRC_DIR}/Sphere.cpp 

OBJ		= ${SRC:%.cpp=${OBJ_DIR}/%.o}

ifeq ($(shell uname -s), Linux)
	MLX_DIR = mlx_linux
	MLX_LIB = -L/usr/lib -Lmlx_linux
	LIB_FLAGS = -lmlx_Linux -lXext -lX11 -lm -lz
	SPEC_HEADER_DIR = linuxHeader

else
	MLX_DIR = mlx
	MLX_LIB = -framework OpenGL -framework AppKit -Lmlx
	LIB_FLAGS = -lmlx -lm
	SPEC_HEADER_DIR = macHeader

endif

HEADERS	= ${HEADER_DIR}/Header.h \
		  ${HEADER_DIR}/A_Scenery.hpp \
		  ${HEADER_DIR}/ARGBColor.hpp \
		  ${HEADER_DIR}/camera.hpp \
		  ${HEADER_DIR}/geometry.hpp \
		  ${HEADER_DIR}/Light.hpp \
		  ${HEADER_DIR}/mlx.h \
		  ${SPEC_HEADER_DIR}/MlxImage.hpp \
		  ${HEADER_DIR}/Ray.hpp \
		  ${HEADER_DIR}/Scene.hpp \
		  ${HEADER_DIR}/Sphere.hpp 

CPPFLAGS = -std=c++2a

all: ${NAME}

${OBJ_DIR}/%.o: %.cpp ${HEADERS}
	-sed -i '' -e 's/SPEC_HEADER_DIR/${SPEC_HEADER_DIR}/g' srcs/*
	mkdir -p ${@D}
	g++ ${CPPFLAGS} -Wall -Wextra -Werror -I${HEADER_DIR} -I${SPEC_HEADER_DIR} -I${MLX_DIR} -o ${subst /${SRC_DIR},,$@} -c $<

${NAME}: ${OBJ}
	make -C ${MLX_DIR} all
	g++ ${CPPFLAGS} -Wall -Wextra -Werror ${subst /${SRC_DIR},,${OBJ}} ${MLX_LIB} -I. -I${MLX_DIR} ${LIB_FLAGS} -o ${NAME}

clean:
	make -C ${MLX_DIR} clean
	rm -fr ${OBJ_DIR}

fclean: clean
	rm -fr ${NAME}

re: fclean all

.PHONY: all clean fclean re
