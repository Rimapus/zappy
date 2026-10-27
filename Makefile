##
## EPITECH PROJECT, 2026
## Zappy
## File description:
## Root Makefile (required by the subject: it carries the zappy_server, zappy_gui
## and zappy_ai rules). Each binary is built through its own CMake project
## (server/, gui/, ai/) and dropped at the repo root for `./zappy_ai`. CMake is
## configured once per component; rebuilds are incremental.
##

CMAKE	:=	cmake
JOBS	:=	-j

all:	zappy_server zappy_gui zappy_ai

## Configure each component's CMake once; `cmake --build` re-configures on its
## own when a CMakeLists changes.
server/build/CMakeCache.txt:
	$(CMAKE) -S server -B server/build
gui/build/CMakeCache.txt:
	$(CMAKE) -S gui -B gui/build
ai/build/CMakeCache.txt:
	$(CMAKE) -S ai -B ai/build

zappy_server:	server/build/CMakeCache.txt
	$(CMAKE) --build server/build $(JOBS)
	cp -f server/zappy_server zappy_server

zappy_gui:	gui/build/CMakeCache.txt
	$(CMAKE) --build gui/build $(JOBS)
	cp -f gui/zappy_gui zappy_gui

zappy_ai:	ai/build/CMakeCache.txt
	$(CMAKE) --build ai/build $(JOBS)
	cp -f ai/zappy_ai zappy_ai

clean:
	@for c in server gui ai; do \
		[ -d $$c/build ] && $(CMAKE) --build $$c/build --target clean >/dev/null 2>&1 || true; \
	done

fclean:
	$(RM) -r server/build gui/build ai/build build
	$(RM) zappy_server zappy_gui zappy_ai
	$(RM) server/zappy_server gui/zappy_gui ai/zappy_ai

re:	fclean all

.PHONY:	all clean fclean re zappy_server zappy_gui zappy_ai
