#include "ofMain.h"

#include "ofxOpenGLPrimitives.h"

using namespace ofxOpenGLPrimitives;

class ofApp : public ofBaseApp
{
public:
	
	ofLight light;
	CubeMap::Ref cube;
	
	void setup()
	{
		ofSetFrameRate(60);
		ofSetVerticalSync(true);
		ofBackground(0);
		
		const char* files[] = {
			"room2px.png",
			"room2nx.png",
			"room2py.png",
			"room2ny.png",
			"room2pz.png",
			"room2nz.png"
		};
		
		cube = new CubeMap(128, 128,
						   TextureFormat::RGB,
						   TextureInternalFormat::RGB8);
		
		cube->bind();
		for (int i = 0; i < 6; i++)
		{
			CubeMapTexture::Ref tex = cube->getCubeMapTexture(i);
			
			ofPixels pix;
			ofLoadImage(pix, files[i]);
			tex->update(pix.getPixels());
		}
		cube->unbind();
	}
	
	void update()
	{
	}
	
	ofEasyCam cam;
	void draw()
	{
		ofEnableDepthTest();
		
		cam.begin();
		
		ofDisableLighting();
		cube->draw(5000);
		
		cube->enable();
		cube->bind();
		{
			light.enable();
			light.setAmbientColor(ofColor(50, 50, 50));
			
			float t = ofGetElapsedTimef() * 0.5;
			light.setPosition(ofSignedNoise(1, 0, 0, t) * 2000,
							  ofSignedNoise(0, 1, 0, t) * 2000,
							  ofSignedNoise(0, 0, 1, t) * 2000);

			ofDrawSphere(0, 0, 0, 200);
			
			light.disable();
		}
		cube->unbind();
		cube->disable();
		
		cam.end();
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
