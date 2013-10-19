#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cassert>

#include "Timewhale.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"

#include "AudioStreamResource.h"
#include "PlayerInputComp.h"
#include "ButtonComponent.h"
#include "TWLogger.h"

using namespace std;
using namespace Timewhale;

void createScene1(ECSManager* ECS);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
    
	vector<string> init_packs;

	/* Engine needs to be initialized first. If false is returned then quit
	 * the game cuz nothings gonna work! */
	bool result = TimewhaleEngine::Initialize(init_packs);
	if(!result) return 0;

	/* This is the standard method for gettings a manager. Note the included
	 * header at the top. Use the managers static get method to get the pointer
	 * to the manager. */
	auto resourceManager = ResourceManager::get();

	/* Here we are adding a new blbr file for the engine to use. You may
	 * add any number of blbr files as you want. This allows us to organize
	 * our blbr files as we want.
	 */
	//resourceManager->AddBlub("whale.blbr");
	
	/* You may also add manifest files to the engine, this allows for you to specify
	 * resources in a location outside a blbr file by given nicknames and scopes.
	 * The syntax for such a call would be as follows:
	 * resourceManager->AddManifest("manifest_path_here"); */
    //resourceManager->AddManifest("manifest.txt");

	/* Adding texture packs can be done like this!
	 */
	resourceManager->AddTextureAtlas("level_pack");

	/* This is the standard method for gettings a manager. Note the included
	 * header at the top. Use the managers static get method to get the pointer
	 * to the manager. */
	auto sceneManager = SceneManager::get();

	/* Here we make a new scene. First we pass in the name of the scene and then
	 * a initialization function. You can use the SCENE_INIT_FUNC macro to make
	 * this easier. Just type in your ECS code to get started!. */
    sceneManager->CreateScene("TestScene", createScene1);

	sceneManager->CreateScene("TestScene2",
		SCENE_INIT_FUNC(
			EntityPtr e1 = ECS->CreateEntity("TestEntity");

			ButtonComponentPtr button = 
				ECS->CreateComponent<ButtonComponent>(e1->getID());
			Transform2DPtr button_t1 = 
				ECS->GetComponent<Transform2D>(e1->getID());
			SpriteComponentPtr button_sprite = 
				ECS->GetComponent<SpriteComponent>(e1->getID());
			assert(button);
			assert(button_t1);
			assert(button_sprite);
			button_t1->SetPosition(100, 100);
			button_t1->SetSize(200, 200);
			button_sprite->SetSpriteSheet(Texture("CrystalOwned"));
			button_sprite->SetLayer(0);

			SpriteComponentPtr s2 =
				ECS->CreateComponentp<SpriteComponent>(e1);
			assert(s2);
			s2->SetSpriteSheet(Texture("FailWhale"));
			s2->SetColor(1.0f, 0.0f, 0.0f);
			s2->SetLayer(1);

			button->SetSimpleHandler(
			 [](void){
					auto mgr = SceneManager::get();
					/*auto top_scene = mgr->current();
					if(top_scene->getSceneName() == "TestScene2") {
						log_sxinfo("SceneHandler", "======Popping TestScene2");
						mgr->pop();
					}*/
					mgr->ChangeScene("TestScene");
				});
			
		)
	);
	sceneManager->ChangeScene("TestScene");

//	sceneManager->push("TestScene2");



	/* Once everything is initialized call this to start running the engine. */
	TimewhaleEngine::Run();

	/* After we return from Run, it's time to shut down everything. */
    TimewhaleEngine::Shutdown();

	/* We're done! And no leaks!!!... HAH HAH YEAH RIGHT */
    return 0;
}

void createScene1(ECSManager* ECS) {
    /* First we will create a couple entities and add some components to them */
	EntityPtr e1 = ECS->CreateEntity("SomeEntity");
	EntityPtr e2 = ECS->CreateEntity("AnotherEntity");

    EntityPtr e3 = ECS->CreateEntity("SomeEntity");

    size_t id1 = (size_t)e1->getID();
    InstanceID id2 = e2->getID();
    int id3 = e3->getID();

	Transform2DPtr t1 = ECS->CreateComponent<Transform2D>(e1->getID());
	t1->SetPosition(400, 400);
	t1->SetSize(64, 64);

	/* Thanks to General Dunbar's resource manager loading textures are
		* super easy! */
	SpriteComponentPtr s1 = ECS->CreateComponent<SpriteComponent>(e1->getID());
	s1->SetSpriteSheet(Texture("victory")); // <-- This is all you need to load/use textures.
	s1->SetLayer(1.0f);

	/* This is a user made component! */
	PlayerInputCompPtr p1 = ECS->CreateComponent<PlayerInputComp>(e1->getID());

	//Lets play some tunes!!!
	//BasicAudio("hey").Play();
    BasicAudio("boo");

    auto button = ECS->CreateComponent<ButtonComponent>(e2->getID());
    auto t2 = ECS->GetComponent<Transform2D>(e2->getID());
    auto s2 = ECS->GetComponent<SpriteComponent>(e2->getID());

    t2->SetPosition(500, 500);
    t2->SetSize(200, 200);
    s2->SetSpriteSheet(Texture("anim_resource_idle"));
	s2->SetMaxFrames(12, 1);
	s2->SetLooping(true);
	s2->SetFrameTickDuration(2);
	s2->SetLoopDirection(SpriteAnimDir::Forward);
    s2->SetLayer(0.5f);

    button->SetSimpleHandler(
    [](void) {
        auto mgr = SceneManager::get();
		//auto top_scene = mgr->GetCurrentScene();
		/*if(top_scene->getSceneName() == "TestScene") {
			log_sxinfo("Scene", "======Pushing TestScene2");
			mgr->push("TestScene2");
        }*/
		mgr->ChangeScene("TestScene2");
    });
}