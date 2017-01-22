#include <sstream>
#include <iomanip>
#include <OpenGL/gl.h>
#include "globj.h"

using namespace std;


/*** GLobj class ***/
void GLobj::translate (float nx, float ny, float nz) {
	x=nx,  y=ny,  z=nz;
}
void GLobj::rotate (float nrot, float x, float y, float z) {
	rot=nrot,  rx=x,  ry=y,  rz=z;
}


namespace globj {
	std::vector<GLobj>  objlist;

	GLobj* mkobj() {
		objlist.push_back(GLobj());
		return  &objlist.back();
	}

	GLobj* clone(const GLobj* obj) {
		objlist.push_back(*obj);
		return  &objlist.back();
	}

	string serialize(const GLobj* obj) {
		const int COLW = 15;
		stringstream ss, s2;
		vector<string> cols;
		for (const auto t : obj->tris) {
			// vertexes, in groups of 3
			for (int i = 0; i < 3; i++) {
				s2.str(""), s2.clear();
				for (int j = 0; j < 3; j++)
					s2 << t.vec[i*3+j] << ' ';
				ss << setw(COLW) << left << s2.str();
			}
			// colors
			s2.str(""), s2.clear();
			for (int i = 0; i < 3; i++)
				s2 << t.col[i] << ' ';
			ss << setw(COLW) << left << s2.str() << endl;
		}
		// return result
		return ss.str();
	}

	GLobj* deserialize(const string& s) {
		// float f;
		// stringstream ss(s);
		// while (ss >> f)
		return NULL;
	}

	int paintall() {
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw objects
		for (const auto& o : objlist) {
			// glLoadIdentity();
			glPushMatrix();
			glTranslatef( o.x, o.y, o.z );
			glRotatef( o.rot, o.rx, o.ry, o.rz );
			// draw each tri
			for (const auto& t : o.tris) {
				const auto& c = t.col;  // col reference
				const auto& v = t.vec;  // vec reference
				glBegin(GL_TRIANGLES);
					glColor4f ( c[0], c[1], c[2], c[3] );
					glVertex3f( v[0], v[1], v[2] );
					glVertex3f( v[3], v[4], v[5] );
					glVertex3f( v[6], v[7], v[8] );
				glEnd();
			}
			glPopMatrix();
		}
		// swap backbuffer
		// SDL_GL_SwapWindow(win);
		return 0;
	}
}; // end globj


namespace glbuild {
	static GLobj  obj;
	static float  c[4] = {0.8f, 0.8f, 0.8f, 0.8f};
	
	void make() {
		obj = GLobj();  // reset
	}
	GLobj* finalize() {
		globj::objlist.push_back(obj);
		return  &globj::objlist.back();
	}
	void col(float r, float g, float b) {
		c[0]=r,  c[1]=g,  c[2]=b,  c[3]=1;
	}
	void col(float r, float g, float b, float a) {
		c[0]=r,  c[1]=g,  c[2]=b,  c[3]=a;
	}
	void tri(const std::vector<float>& v) {
		if (v.size() != 9) {
			fprintf(stderr, "wrong number of verticies passed for tri.\n");
			return;
		}
		obj.tris.push_back({
			{ v[0],v[1],v[2],  v[3],v[4],v[5],  v[6],v[7],v[8] },
			{ c[0],c[1],c[2],c[3] }
		});
	}
	void quad(const std::vector<float>& v) {
		if (v.size() != 12) {
			fprintf(stderr, "wrong number of verticies passed for quad.\n");
			return;
		}
		obj.tris.push_back({
			{ v[0],v[1],v[2],  v[3],v[4],v[5],    v[6],v[7],v[8] },
			{ c[0],c[1],c[2],c[3] }
		});
		obj.tris.push_back({
			{ v[6],v[7],v[8],  v[9],v[10],v[11],  v[0],v[1],v[2] },
			{ c[0],c[1],c[2],c[3] }
		});
	}
	void tris(const std::vector<float>& v) {
		if (v.size() == 0 || v.size() % 9 != 0) {
			fprintf(stderr, "wrong number of verticies passed for tri. must be a multiple of 9. found: %d\n", (int)v.size());
			return;
		}
		for (int i = 0; i < v.size(); i += 9)
			tri( vector<float>( &v[i], &v[i+9] ) );
	}
	void quads(const std::vector<float>& v) {
		if (v.size() == 0 || v.size() % 12 != 0) {
			fprintf(stderr, "wrong number of verticies passed for quad. must be a multiple of 12. found: %d\n", (int)v.size());
			return;
		}
		for (int i = 0; i < v.size(); i += 12)
			tri( vector<float>( &v[i], &v[i+12] ) );
	}
}  // end glbuild