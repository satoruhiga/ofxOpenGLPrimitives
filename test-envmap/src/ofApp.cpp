#include "ofMain.h"

#include "ofxOpenGLPrimitives.h"

using namespace ofxOpenGLPrimitives;

class ofApp : public ofBaseApp
{
public:
	
	ofLight light;
	
	CubeMap::Ref cube;
	EnvMap::Ref envmap;
	
	void setup()
	{
		ofSetFrameRate(60);
		ofSetVerticalSync(true);
		ofBackground(255);
		
		cube = new CubeMap(512, 512,
						   TextureFormat::RGB,
						   TextureInternalFormat::RGB8);
		
		envmap = new EnvMap(cube);
		envmap->setNearClip(10);
		envmap->setFarClip(4000);
	}
	
	void update()
	{
		for (int i = 0; i < 6; i++)
		{
			envmap->begin(i);
			drawScene();
			envmap->end();
		}
	}
	
	void drawScene()
	{
		ofPushStyle();
		
		light.enable();
		
		float t = ofGetElapsedTimef() * 0.1;
		for (int i = 0; i < 10; i++)
		{
			int idx = i + 1;
			ofVec3f p;
			p.x = ofSignedNoise(idx, 0, 0, t) * 2000;
			p.y = ofSignedNoise(0, idx, 0, t) * 2000;
			p.z = ofSignedNoise(0, 0, idx, t) * 2000;
			
			ofSetColor(ofColor::fromHsb(ofMap(i, 0, 10, 0, 255), 255, 255));
			ofDrawBox(p, 200);
		}
		
		light.disable();
		
		ofPopStyle();
	}
	
	ofEasyCam cam;
	void draw()
	{
		cam.setFarClip(10000);
		
		ofEnableDepthTest();
		
		cam.begin();
		
		drawScene();
		
		cube->enable();
		cube->bind();
		{
			light.enable();
			light.setAmbientColor(ofColor(127, 127, 127));
			
			float t = ofGetElapsedTimef() * 0.5;
			light.setPosition(ofSignedNoise(1, 0, 0, t) * 2000,
							  ofSignedNoise(0, 1, 0, t) * 2000,
							  ofSignedNoise(0, 0, 1, t) * 2000);
			
			ofPushStyle();
			ofSetColor(255);
			ofDrawSphere(300);
			ofPopStyle();
			
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
