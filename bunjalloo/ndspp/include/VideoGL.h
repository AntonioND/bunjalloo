/*
  Copyright (C) 2008 Richard Quirk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef VideoGL_h_seen
#define VideoGL_h_seen

// shouldn't #include gl.h on pc nor arm9/videogl.h on ds
// by using namespace nds::VideoGL it should look like regular open gl...
namespace nds
{
  namespace VideoGL
  {

    void glPopMatrix(int i);
    void glFlush (int mode);
    void glViewport(int x, int y, int w, int h);

    enum GL_MATRIX_MODE_ENUM {
      GL_PROJECTION,
      GL_POSITION,
      GL_MODELVIEW,
      GL_TEXTURE
    } ;

    void glMatrixMode(GL_MATRIX_MODE_ENUM mode);

    enum GL_GLBEGIN_ENUM {
      GL_TRIANGLES      = 0,
      GL_QUADS          = 1,
      GL_TRIANGLE_STRIP = 2,
      GL_QUAD_STRIP     = 3,
      GL_TRIANGLE       = 0,
      GL_QUAD           = 1
    };
    void glBegin(GL_GLBEGIN_ENUM mode);

    enum GL_TEXTURE_TYPE_ENUM {
      GL_RGB32_A3   = 1,
      GL_RGB4       = 2,
      GL_RGB16      = 3,
      GL_RGB256     = 4,
      GL_COMPRESSED = 5,
      GL_RGB8_A5    = 6,
      GL_RGBA       = 7,
      GL_RGB        = 8
    };
    enum GL_TEXTURE_PARAM_ENUM {
      GL_TEXTURE_WRAP_S = (1 << 16),
      GL_TEXTURE_WRAP_T = (1 << 17),
      GL_TEXTURE_FLIP_S = (1 << 18),
      GL_TEXTURE_FLIP_T = (1 << 19),
      GL_TEXTURE_COLOR0_TRANSPARENT = (1<<29),
      TEXGEN_OFF      = (0<<30),
      TEXGEN_TEXCOORD = (1<<30),
      TEXGEN_NORMAL   = (2<<30),
      TEXGEN_POSITION = (3<<30)
    };
    // int glTexImage2D(int target, int empty1, int type, int sizeX, int sizeY, int empty2, int param, const void* texture);
    // completely not compatible with open gl
    void glTexImage2D(int target, GL_TEXTURE_TYPE_ENUM type, int w, int h, int param, const void * data);

    void glInit();
    void glClearPolyID(int id);
    void gluPerspectivef32(int fovy, int aspect, int zNear, int zFar);
    void gluLookAtf32(int eyex, int eyey, int eyez, int lookAtx, int lookAty, int lookAtz, int upx, int upy, int upz);
    void glPolyFmt(int params);
    void glTranslate3f32(int x, int y, int z);
    void glRotatef32i(int angle, int x, int y, int z);
    void glRotateXi(int angle);
    void glRotateYi(int angle);
    void glRotateZi(int angle);
    void glVertex3v16(short x, short y, short z);
    void glTexCoord2f32(int u, int v);


    enum GL_POLY_FORMAT_ENUM {
      POLY_FORMAT_LIGHT0   = (1<<0), /*!< enable light number 0 */
      POLY_FORMAT_LIGHT1   = (1<<1), /*!< enable light number 1 */
      POLY_FORMAT_LIGHT2   = (1<<2), /*!< enable light number 2 */
      POLY_FORMAT_LIGHT3   = (1<<3), /*!< enable light number 3 */
      POLY_MODULATION      = (0<<4), /*!< enable modulation shading mode; this is the default */
      POLY_DECAL           = (1<<4), /*!< enable decal shading */
      POLY_TOON_HIGHLIGHT  = (2<<4), /*!< enable toon/highlight shading mode */
      POLY_SHADOW          = (3<<4), /*!< enable shadow shading */
      POLY_CULL_FRONT      = (1<<6), /*!< cull front polygons */
      POLY_CULL_BACK       = (2<<6), /*!< cull rear polygons */
      POLY_CULL_NONE       = (3<<6), /*!< don't cull any polygons */
      POLY_FOG             = (1<<15) /*!< enable/disable fog for this polygon */
    };

    int POLY_ALPHA(int i);

    enum DISP3DCNT_ENUM {
      GL_TEXTURE_2D      = 1,
      GL_ALPHA_TEST      = 4,
      GL_BLEND           = 8,
      GL_FOG             = 128
    };
    void glEnable(int bits);
    void glClearColor(int red, int green, int blue, int alpha);
    void glClearDepth(int depth);
    void glLoadIdentity();
    void glColor3b(int red, int green, int blue);
    void glPushMatrix();
    void glEnd();
    int glGenTexture();
    void glBindTexture2D(int texture);
  }
}

#endif
