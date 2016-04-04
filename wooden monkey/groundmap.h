const int MAP_SIZE = 64;
const float MAP_SCALE = 0.2f;

class GroundMap {
private:
    GLuint TexID;

public:
    GLuint LoadTexture( char * source);
	void Render(char *source);

};
