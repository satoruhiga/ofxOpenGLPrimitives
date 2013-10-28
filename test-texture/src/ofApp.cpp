#include "ofMain.h"

#include "ofxOpenGLPrimitives.h"

using namespace ofxOpenGLPrimitives;

class ofApp : public ofBaseApp
{
public:
	
	Texture2D::Ref tex;
	
	void setup()
	{
		ofSetFrameRate(60);
		ofSetVerticalSync(true);
		ofBackground(0);
		
		ofPixels pix;
		ofLoadImage(pix, "Lenna.jpg");
		
		tex = new Texture2D(pix.getWidth(), pix.getHeight(),
							TextureFormat::RGB,
							TextureInternalFormat::RGB8);
		
		tex->bind();
		tex->update(pix.getPixels());
		tex->unbind();
	}
	
	void update()
	{
	}
	
	void draw()
	{
		tex->draw(0, 0);
	}

	void keyPressed(int key) {}
	void keyReleased(int key) {}
	
	void mouseMoved(int x, int y) {}
	void mouseDragged(int x, int y, int button) {}
	void mousePressed(int x, int y, int button) {}
	void mouseReleased(int x, int y, int button) {}
	
	void windowResized(int w, int h) {}
};


int main(int argc, const char** argv)
{
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp);
	return 0;
}
