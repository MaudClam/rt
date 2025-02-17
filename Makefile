NAME	= rt


SRC_DIR 	= srcs

HEADER_DIR	= srcs

SRC		= ${SRC_DIR}/main.cpp \
			${SRC_DIR}/A_Scenery.cpp \
			${SRC_DIR}/ARGBColor.cpp \
			${SRC_DIR}/camera.cpp \
			${SRC_DIR}/geometry.cpp \
			${SRC_DIR}/Light.cpp \
			${SRC_DIR}/MlxImage.cpp \
			${SRC_DIR}/Ray.cpp \
			${SRC_DIR}/Scene.cpp \
			${SRC_DIR}/Sphere.cpp \
			${SRC_DIR}/PhotonMap.cpp \
			${SRC_DIR}/PhotonTrace.cpp \
			${SRC_DIR}/Power.cpp

OBJ		= ${SRC:%.cpp=%.o}

ifeq ($(shell uname -s), Linux)
	MLX_DIR = mlx_linux
	MLX_LIB = -L/usr/lib -Lmlx_linux
	LIB_FLAGS = -lmlx_Linux -lXext -lX11 -lm -lz
	SPEC_HEADER = linuxKeys.h
	
else
	MLX_DIR = mlx
	MLX_LIB = -framework OpenGL -framework AppKit -Lmlx
	LIB_FLAGS = -lmlx -lm
	SPEC_HEADER = macKeys.h

endif

HEADERS	= ${HEADER_DIR}/Header.h \
		  ${HEADER_DIR}/A_Scenery.hpp \
		  ${HEADER_DIR}/ARGBColor.hpp \
		  ${HEADER_DIR}/camera.hpp \
		  ${HEADER_DIR}/geometry.hpp \
		  ${HEADER_DIR}/Light.hpp \
		  ${HEADER_DIR}/mlx.h \
		  ${HEADER_DIR}/MlxImage.hpp \
		  ${HEADER_DIR}/Ray.hpp \
		  ${HEADER_DIR}/Scene.hpp \
		  ${HEADER_DIR}/Sphere.hpp \
		  ${HEADER_DIR}/keys.h \
		  ${HEADER_DIR}/random.hpp \
		  ${HEADER_DIR}/PhotonMap.h \
		  ${HEADER_DIR}/PhotonTrace.h \
		  ${HEADER_DIR}/Average.h \
		  ${HEADER_DIR}/Power.hpp

CPPFLAGS = -std=c++2a -O2

all: os_comp ${NAME}

os_comp:
	-cp srcs/${SPEC_HEADER} srcs/keys.h


%.o: %.cpp ${HEADERS}
	mkdir -p ${@D}
	g++ ${CPPFLAGS} -Wall -Wextra -Werror -I${HEADER_DIR} -I${MLX_DIR} -o $@ -c $<

${NAME}: ${OBJ}
	@make -C ${MLX_DIR} all > /dev/null 2>&1
	g++ ${CPPFLAGS} -Wall -Wextra -Werror ${OBJ} ${MLX_LIB} -I${HEADER_DIR} -I${MLX_DIR} ${LIB_FLAGS} -o ${NAME}

clean:
	make -C ${MLX_DIR} clean
	rm -fr ${OBJ} 

fclean: clean
	rm -fr ${NAME}

re: fclean all

.PHONY: all clean fclean re
