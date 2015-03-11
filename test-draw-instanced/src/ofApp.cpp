#include "ofMain.h"

#include "ofxOpenGLPrimitives.h"

using namespace ofxOpenGLPrimitives;

struct InstancePosition : public Attribute_<10, ofVec3f, GL_FLOAT, 3>
{
	static string getAttributeName() { return "instance_position"; }
	
	void position(const ofVec3f& v) { value = v; }
	void position(float x, float y, float z) { value.set(x, y, z); }
};

struct InstanceColor : public Attribute_<11, ofFloatColor, GL_FLOAT, 4>
{
	static string getAttributeName() { return "instance_color"; }
	
	void color(const ofFloatColor& v) { value = v; }
};

class ofApp : public ofBaseApp
{
public:
	
	ofxOpenGLPrimitives::ShaderLoader loader;
	
	VertexAttribute_<InstancePosition, InstanceColor> per_instance_attr;
	Geometry_<> model;
	
	Renderer_<
		Program_<InstancePosition, InstanceColor>,
		RendererCapability::ModelTransform,
		RendererCapability::Camera
	> renderer;
	
	ofEasyCam cam;
	
	void setup()
	{
		ofSetFrameRate(0);
		ofSetVerticalSync(false);
		ofBackground(0);

		loader.setVersion(400);
		loader.load("instanced", renderer);
		
		renderer.dumpInfo();

		{
			per_instance_attr.begin();
			
			float s = 200;
			
			for (int i = 0; i < 30; i++)
			{
				per_instance_attr.position(ofRandom(-s, s),
							  ofRandom(-s, s),
							  ofRandom(-s, s));
				per_instance_attr.color(ofColor::fromHsb(ofMap(i, 0, 30, 0, 255), 255, 255));
				per_instance_attr.push();
			}
			
			per_instance_attr.end();
		}
		
		{
			model.begin(GL_LINES);
			
			float s = 4;
			model.vertex(-s, 0, 0);
			model.vertex(s, 0, 0);
			
			model.vertex(0, -s, 0);
			model.vertex(0, s, 0);
			
			model.vertex(0, 0, -s);
			model.vertex(0, 0, s);
			
			model.end();
			
			model.bindInstancedAttribute(per_instance_attr);
		}
	}
	
	void update()
	{
		per_instance_attr.begin();
		
		int N = 10000;
		float s = 3000;
		
		float t = ofGetElapsedTimef() * 0.025;
		
		for (int i = 0; i < N; i++)
		{
			per_instance_attr.position(ofSignedNoise(1, 0, 0, t + i * 0.01) * s,
						  ofSignedNoise(0, 1, 0, t + i * 0.01) * s,
						  ofSignedNoise(0, 0, 1, t + i * 0.01) * s);
			per_instance_attr.color(ofColor::fromHsb(ofMap(i, 0, N, 0, 255), 255, 255));
			per_instance_attr.push();
		}
		
		per_instance_attr.end();
		
		model.bindInstancedAttribute(per_instance_attr);
	}
	
	void draw()
	{
		ofDrawBitmapString(ofToString(ofGetFrameRate(), 1), 10, 20);
		ofEnableDepthTest();
		
		renderer.setCamera(cam);
		
		renderer.use();
		renderer.setUniform1f("time", ofGetElapsedTimef() * 3);
		renderer.pushMatrix();
		renderer.multMatrix(ofMatrix4x4::newRotationMatrix(ofGetElapsedTimef() * 20, 0, 1, 0));
		renderer.draw(model, per_instance_attr.getNumVertices());
		renderer.popMatrix();
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
	ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp);
	return 0;
}
