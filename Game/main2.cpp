#include "irrlicht.h"
#include "exampleHelper.h"


using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class Atest
{
public:
	int test() { return 0; }
	template<class TModuleInterface>
	TModuleInterface test()
	{
		return 0.0; 
	}
};

int main(int argc, char** argv)
{
	//IrrlichtDevice* device = createDevice(video::EDT_OPENGL, dimension2d<u32>(640, 480), 16, false, false, false, 0);
	IrrlichtDevice* device = createDevice(video::EDT_OPENGL_4_6, dimension2d<u32>(640, 480), 16, false, false, false, 0);

	if (!device)
		return 1;

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	/*
	We add a hello world label to the window, using the GUI environment.
	The text is placed at the position (10,10) as top left corner and
	(260,22) as lower right corner.
	*/
	guienv->addStaticText(L"Hello World! This is Irrlicht with the burnings software renderer!", rect<s32>(10, 10, 260, 22), true);

	const io::path mediaPath = getExampleMediaPath();

	scene::ISceneNode* CubeNode = smgr->addCubeSceneNode();
	if (CubeNode) 
	{
		//CubeNode->setPosition(core::vector3df(0, 0, 60));
		CubeNode->setMaterialTexture(0, driver->getTexture(mediaPath + "wall.bmp"));
		CubeNode->setMaterialFlag(video::EMF_LIGHTING, false);
	}
	//IAnimatedMesh* mesh = smgr->getMesh(mediaPath + "sydney.md2");
	//if (!mesh)
	//{
	//	device->drop();
	//	return 1;
	//}
	//IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);

	/*
	To let the mesh look a little bit nicer, we change its material. We
	disable lighting because we do not have a dynamic light in here, and
	the mesh would be totally black otherwise. Then we set the frame loop,
	such that the predefined STAND animation is used. And last, we apply a
	texture to the mesh. Without it the mesh would be drawn using only a
	color.
	*/
	//if (node)
	//{
	//	node->setMaterialFlag(EMF_LIGHTING, false);
	//	node->setMD2Animation(scene::EMAT_RUN);
	//	//node->setMaterialTexture(0, driver->getTexture(mediaPath + "sydney.bmp"));
	//}

	
	ICameraSceneNode* cameranNode = smgr->addCameraSceneNodeBlender();// 0, vector3df(0, 30, -40), vector3df(0, 5, 0));
	cameranNode->setTarget(vector3df(0, 0, 0));
	//smgr->addCameraSceneNode(0, vector3df(0, 30, -40), vector3df(0, 5, 0));
	//smgr->addCameraSceneNodeFPS();

	int lastFPS = -1;
	while (device->run())
	{
		//if (device->isWindowActive())
		//{
		//driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 124, 140, 166));
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 166, 183, 210));

			smgr->drawAll();
			guienv->drawAll();

			driver->endScene();

			int fps = driver->getFPS();

			//if (lastFPS != fps)
			//{
			//	core::stringw str = L"Irrlicht Engine - example [";
			//	str += driver->getName();
			//	str += "] FPS:";
			//	str += fps;

			//	device->setWindowCaption(str.c_str());
			//	lastFPS = fps;
			//}
			//else
			//	device->yield();
		//}

	}
	device->drop();

	return 0;
}
