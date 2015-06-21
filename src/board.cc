#include "opengl.h"
#include "board.h"

static Mesh *gen_board_mesh();
static Mesh *gen_puck_mesh();

Board::Board()
{
	clear();
}

Board::~Board()
{
	destroy();
}

bool Board::init()
{
	if(!(board_mesh = gen_board_mesh())) {
		return false;
	}
	if(!(puck_mesh = gen_puck_mesh())) {
		return false;
	}
	return true;
}

void Board::destroy()
{
	delete board_mesh;
	delete puck_mesh;
	board_mesh = puck_mesh = 0;
}

void Board::clear()
{
	memset(slots, 0, sizeof slots);
}

void Board::draw() const
{
	if(board_mesh)
		board_mesh->draw();
}

static Mesh *gen_board_mesh()
{
	return 0;
}

static Mesh *gen_puck_mesh()
{
	return 0;
}
