#include <OpenGL/gl.h>
#include "globj.h"

using namespace std;


/*** GLobj class ***/
void GLobj::translate (float nx, float ny, float nz) {
	x=nx,  y=ny,  z=nz;
}
void GLobj::rotate (float x, float y, float z, float nrot) {
	rx=x,  ry=y,  rz=z,  rot=nrot;
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

	int paintall() {
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw objects
		for (const auto& o : objlist) {
			glLoadIdentity();
			glTranslatef( o.x, o.y, o.z );
			glRotatef( o.rot, o.rx, o.ry, o.rz );
			// draw each tri
			for (const auto& t : o.tris) {
				const auto& c = t.col;  // col reference
				const auto& v = t.vec;  // vec reference
				glBegin(GL_TRIANGLES);
					glColor3f ( c[0], c[1], c[2] );
					glVertex3f( v[0], v[1], v[2] );
					glVertex3f( v[3], v[4], v[5] );
					glVertex3f( v[6], v[7], v[8] );
				glEnd();
			}
		}
		// swap backbuffer
		// SDL_GL_SwapWindow(win);
		return 0;
	}
}; // end globj


namespace glbuild {
	static GLobj  obj;
	static float  c[3] = {0.8f};
	
	void make() {
		obj = GLobj();  // reset
	}
	GLobj* finalize() {
		globj::objlist.push_back(obj);
		return  &globj::objlist.back();
	}
	void col(float r, float g, float b) {
		c[0]=r,  c[1]=g,  c[2]=b;
	}
	void tri(std::vector<float> v) {
		if (v.size() != 9) {
			fprintf(stderr, "wrong number of verticies passed for tri.\n");
			return;
		}
		obj.tris.push_back({
			{ v[0],v[1],v[2],  v[3],v[4],v[5],  v[6],v[7],v[8] },
			{ c[0],c[1],c[2] }
		});
	}
	void quad(std::vector<float> v) {
		if (v.size() != 12) {
			fprintf(stderr, "wrong number of verticies passed for quad.\n");
			return;
		}
		obj.tris.push_back({
			{ v[0],v[1],v[2],  v[3],v[4],v[5],    v[6],v[7],v[8] },
			{ c[0],c[1],c[2] }
		});
		obj.tris.push_back({
			{ v[6],v[7],v[8],  v[9],v[10],v[11],  v[0],v[1],v[2] },
			{ c[0],c[1],c[2] }
		});
	}
}  // end glbuild