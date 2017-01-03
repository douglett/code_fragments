#include "game.h"

using namespace std;


namespace gmap {
	int  posx = 1,  posy = 2,  dir = 0;
	vector<string> gmap = {
		".........",
		".       .",
		". ..    .",
		".       .",
		".........",
	};
	int is_empty(char c)   { return c == ' '; }
	int is_null(char c)    { return c == 'x'; }
	int is_nothing(char c) { return is_empty(c) || is_null(c); }

	string getrow(int row) {
		string r;
		int  xx = posx,  yy = posy,  rsize = 2;
		// make row
		switch (dir) {
		case 0:  // north
			yy -= row;
			for (int x = -rsize; x <= rsize; x++)
				if (yy < 0 || yy >= gmap.size() || xx+x < 0 || xx+x >= gmap[yy].size())  r += 'x';
				else  r += gmap[yy][xx+x];
			break;
		case 1:  // east
			xx += row;
			for (int y = -rsize; y <= rsize; y++)
				if (yy+y < 0 || yy+y >= gmap.size() || xx < 0 || xx >= gmap[yy+y].size())  r += 'x';
				else  r += gmap[yy+y][xx];
			break;
		case 2:  // south
			yy += row;
			for (int x = rsize; x >= -rsize; x--)
				if (yy < 0 || yy >= gmap.size() || xx+x < 0 || xx+x >= gmap[yy].size())  r += 'x';
				else  r += gmap[yy][xx+x];
			break;
		case 3:  // west
			xx -= row;
			for (int y = rsize; y >= -rsize; y--)
				if (yy+y < 0 || yy+y >= gmap.size() || xx < 0 || xx >= gmap[yy+y].size())  r += 'x';
				else  r += gmap[yy+y][xx];
			break;
		}
		// format row
		// for (int i = 0; i < r.length(); i++) {
		// 	if (r[i] != '.')  continue;
		// 	else if (i > 0 && gmap::is_empty(r[i-1]))  r[i] = '\\';
		// 	else if (i < r.length()-1 && gmap::is_empty(r[i+1]))  r[i] = '/';
		// 	else  r[i] = '_';
		// }
		return r;
	}
} // end gmap