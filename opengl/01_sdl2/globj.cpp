#include "globj.h"

using namespace std;


/*** GLobj class ***/
void GLobj::translate (GLfloat nx, GLfloat ny, GLfloat nz) {
	x=nx,  y=ny,  z=nz;
}
void GLobj::rotate (GLfloat x, GLfloat y, GLfloat z, GLfloat nrot) {
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
	static float  c;
	
	void make() {
		obj = GLobj();  // reset
	}
	GLobj* finalize() {
		globj::objlist.push_back(obj);
		return  &globj::objlist.back();
	}
	void col(float r, float g, float b) {
		c[0]=r,  c[0]=g,  c[0]=b;
	}
	void tri(std::vector<float> v) {
		if (v.size() != 9) {
			fprintf(stderr, "wrong number of verticies passed for tri.\n");
			return;
		}
		obj.tris.push_back({
			{ v[0],v[1],v[2],  v[3],v[4],v[5],  v[6],v[7],v[8] },
			{ c[0],c[1],c[1] }
		});
	}
}  // end glbuild