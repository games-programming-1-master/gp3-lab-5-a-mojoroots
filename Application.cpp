#include "pch.h"
#include "Application.h"
#include "Common.h"
#include "MeshRenderer.h"
#include "Quad.h"
#include "CameraComp.h"
#include "Input.h"
#include "Resources.h"
#include "Physics.h"
#include "BoxShape.h"
#include "RigidBody.h"
#include "Camera.h"
#include "chrono"
#include "thread"
#include"cSound.h"
#include"cSoundMgr.h"



Application* Application::m_application = nullptr;
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
Entity* a = new Entity(); // ground object
Entity* b = new Entity(); // player 1
Entity* c = new Entity(); // player 2
Entity* d = new Entity(); // unused 
Entity* e = new Entity(); // player 1 ball
Entity* f = new Entity(); // top wall
Entity* g = new Entity(); // bottom wall
Entity* h = new Entity(); // left wall
Entity* i = new Entity(); // right wall
Entity* k = new Entity(); //player 2 ball
Entity* j = new Entity(); // player 1 wins
Entity* l = new Entity(); // player 2 wins
float camDistance = 10.0f; // float used for applying zoom in and zoom out
float camrotateX = 0; // unused
float camrotatey = 0; // unused
CameraComp* cc = new CameraComp(); // camera object
bool firstCamActivie = true; // bool for controling cam type
bool ballInPlay = false; // is the ball in play
bool p1Shot = false; // bool for if player one has fired
bool p2Shot = false; // bool for if player 2 has fired
glm::vec3 play1Pos; // player 1 position for use in ball spawn
glm::quat play1Rot; // player 1 rotation unused
glm::vec3 play2Pos; // player 2 position for use in ball spawn
glm::quat play2Rot; // player 2 rotation unused
int player1Lives = 5; // inital player 1 lives integer
int player2Lives = 5; // inital player 2 lives integer
bool musicon = true;
Application::Application()
{
}



void Application::Init()
{

	//Initialise the soundmanager
	if(theSoundMgr->initMixer())
	{ //Load game sounds 
		soundList={"theme","shoot"};
		soundTypes={soundType::music,soundType::sfx};
		soundsToUse={ASSET_AUDIO_PATH_Theme+"ThemeTune.wav",ASSET_AUDIO_PATH_SFX+"LaserP1.wav"};
		for(unsigned int sounds=0;sounds<soundList.size();sounds++)
		{theSoundMgr->add(soundList[sounds],soundsToUse[sounds],soundTypes[sounds]);}
		//Playthethemeonaconstantloop
		theSoundMgr->getSnd("theme")->play(-1);

		
	}
	


	//performing initialization
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		LOG_DEBUG(SDL_GetError());
		exit(-1);
	}

	//setting openGL version to 4.2 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	//setting openGL context to core, not compatibility
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//creating window
	m_window = SDL_CreateWindow("GP3-LAB-5", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight,
		SDL_WINDOW_OPENGL);

	SDL_CaptureMouse(SDL_TRUE);

	OpenGlInit();
	GameInit();
}

void Application::OpenGlInit()
{
	//creating context (our opengl statemachine in which all our GL calls will refer to)
	m_glContext = SDL_GL_CreateContext(m_window);
	CHECK_GL_ERROR();

	SDL_GL_SetSwapInterval(1);

	//initialsing glew (do this after establishing context!)
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	//Smooth shading
	GL_ATTEMPT(glShadeModel(GL_SMOOTH));

	//enable depth testing
	GL_ATTEMPT(glEnable(GL_DEPTH_TEST));
	//set less or equal func for depth testing
	GL_ATTEMPT(glDepthFunc(GL_LEQUAL));

	//enabling blending
	glEnable(GL_BLEND);
	GL_ATTEMPT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	//turn on back face culling
	GL_ATTEMPT(glEnable(GL_CULL_FACE));
	GL_ATTEMPT(glCullFace(GL_BACK));

	glDisable(GL_CULL_FACE);

}
void Application::Spawn()
{
	// code for spawning player 1 ball, controling  ball spawn position, scale, rigidbody, model, shader and texture

	// code for setting the model, texture and shader for the ball object
		Resources::GetInstance()->AddModel("Models/Sphere.obj");
		Resources::GetInstance()->AddTexture("Images/Textures/Blue.png");
		Resources::GetInstance()->AddShader(std::make_shared<ShaderProgram>(ASSET_PATH + "Shaders/simple_VERT.glsl",
			ASSET_PATH + "Shaders/simple_FRAG.glsl"),
			"simple");
		m_entities.push_back(e);
		e->AddComponent(
			new MeshRenderer(
				Resources::GetInstance()->GetModel("Models/Sphere.obj"),
				Resources::GetInstance()->GetShader("simple"),
				Resources::GetInstance()->GetTexture("Images/Textures/Blue.png"))
		);
		MeshRenderer* p = e->GetComponent<MeshRenderer>();
		// gets player 1 position
		play1Pos = b->GetTransform()->GetPosition();
		// gets player 1 rotation, currently unused
		//play1Rot = b->GetTransform()->GetRotation();
		//e->GetTransform()->SetRotation(play1Rot);
		// sets balls inital spawn position
		e->GetTransform()->SetPosition(play1Pos + glm::vec3(0.f, 0.f, 0.5f));

		// adds rigidbody
		e->AddComponent<RigidBody>();
		// sets rigidbody size and shape
		e->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(0.5f, 0.5f, 0.5f)));
		// sets ball scale
		e->GetTransform()->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
	
		
	ballInPlay = true;

	

}
void Application::Delay(int nSecs)
{
	// delay timer used to change player colour currently unused

	e->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
	ballInPlay = false;
	// sets timer
	int seconds = 1000 * nSecs;
	clock_t startTime = clock();

	while (clock() < startTime + seconds)
	{
		c->GetComponent<MeshRenderer>()->ChangeTexture(Resources::GetInstance()->GetTexture("Images/Textures/red.png"));
	}

	
}

void Application::timer1()
{

	// inital concept for player colour change proved unusable, timer 1 used to change player colour currently unused

//	using namespace std::this_thread;
	using namespace std::chrono;
	// sleeps for set number of seconds
	//sleep_for(nanoseconds(10));
	//sleep_until(system_clock::now() + seconds(2));
	c->GetComponent<MeshRenderer>()->ChangeTexture(Resources::GetInstance()->GetTexture("Images/Textures/Red.png"));
}

void Application::Spawn2()
{
	// code for spawning player 2 ball, controling  ball spawn position, scale, rigidbody, model, shader and texture

	// code for setting the model, texture and shader for the ball object
	Resources::GetInstance()->AddModel("Models/Sphere.obj");
	Resources::GetInstance()->AddTexture("Images/Textures/Red.png");
	Resources::GetInstance()->AddShader(std::make_shared<ShaderProgram>(ASSET_PATH + "Shaders/simple_VERT.glsl",
		ASSET_PATH + "Shaders/simple_FRAG.glsl"),
		"simple");
	m_entities.push_back(k);
	k->AddComponent(
		new MeshRenderer(
			Resources::GetInstance()->GetModel("Models/Sphere.obj"),
			Resources::GetInstance()->GetShader("simple"),
			Resources::GetInstance()->GetTexture("Images/Textures/Red.png"))
	);
	MeshRenderer* w = k->GetComponent<MeshRenderer>();
	// gets player 2 position
	play2Pos = c->GetTransform()->GetPosition();
	// gets player 2 rotation, currently unused
	//play2Rot = c->GetTransform()->GetRotation();
	//e->GetTransform()->SetRotation(play1Rot);
	// sets ball spawn point
	k->GetTransform()->SetPosition(play2Pos + glm::vec3(0.f, 0.f, -0.5f));

	// adds rigid body 
	k->AddComponent<RigidBody>();
	// sets rigid body shape
	k->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(0.5f, 0.5f, 0.5f)));
	// sets ball scale
	k->GetTransform()->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

	// bool for controling player 2 shooting
	p2Shot = true;

}

void Application::MousePos()
{

}

void Application::GameInit()
{
	//loading all resources
	// loading all models
	Resources::GetInstance()->AddModel("Models/cube.obj");
	Resources::GetInstance()->AddModel("Models/cube2.obj");
	Resources::GetInstance()->AddModel("Models/dog.obj");
	Resources::GetInstance()->AddModel("Models/dalek.obj");
	Resources::GetInstance()->AddModel("Models/Plane.obj");
	Resources::GetInstance()->AddModel("Models/Spaceship.obj");
	Resources::GetInstance()->AddModel("Models/SShip.obj");
	Resources::GetInstance()->AddModel("Models/Sphere.obj");
	Resources::GetInstance()->AddModel("Models/ProperPlane.obj");
	Resources::GetInstance()->AddModel("Models/Wall.obj");
	Resources::GetInstance()->AddModel("Models/Player1Wins.obj");
	Resources::GetInstance()->AddModel("Models/Player2Wins.obj");
	// loading all textures
	Resources::GetInstance()->AddTexture("Images/Textures/Wood.jpg");
	Resources::GetInstance()->AddTexture("Images/Textures/Metal.png");
	Resources::GetInstance()->AddTexture("Images/Textures/Red.png");
	Resources::GetInstance()->AddTexture("Images/Textures/Blue.png");
	Resources::GetInstance()->AddTexture("Images/Textures/Space.jpg");
	

	Resources::GetInstance()->AddShader(std::make_shared<ShaderProgram>(ASSET_PATH + "Shaders/simple_VERT.glsl",
		ASSET_PATH + "Shaders/simple_FRAG.glsl"),
		"simple"
	);

	// floor object
	m_entities.push_back(a);
	a->AddComponent(
		new MeshRenderer(
			// sets the model to be used
			Resources::GetInstance()->GetModel("Models/ProperPlane.obj"),
			// sets the shader to be used
			Resources::GetInstance()->GetShader("simple"),
			// sets the texture to be used
			Resources::GetInstance()->GetTexture("Images/Textures/Space.jpg"))
	);
	MeshRenderer* m = a->GetComponent<MeshRenderer>();
	// sets the objects position
	a->GetTransform()->SetPosition(glm::vec3(0.f, 0.f, 0.f));
	// adds a rigidbody
	a->AddComponent<RigidBody>();
	// sets rigidbody size and shape
	a->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(250.f, 0.5f, 250.f)));
	a->GetComponent<RigidBody>()->Get()->setMassProps(0, btVector3());
	// sets the objects scale
	a->GetTransform()->SetScale(glm::vec3(0.05f, 1.f, 0.05f));

	// top wall object
	m_entities.push_back(f);
	f->AddComponent(
		new MeshRenderer(
			// sets the model to be used
			Resources::GetInstance()->GetModel("Models/Wall.obj"),
			// sets the shader to be used
			Resources::GetInstance()->GetShader("simple"),
			// sets the texture to be used
			Resources::GetInstance()->GetTexture("Images/Textures/Red.png"))
	);
	MeshRenderer* p = f->GetComponent<MeshRenderer>();
	// sets the position of the wall object
	f->GetTransform()->SetPosition(glm::vec3(0.f, -7.5f, 25.f));
	// adds a rigidbody
	f->AddComponent<RigidBody>();
	// sets rigidbody size and shape
	f->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(25.f, 20.f, 3.f)));
	f->GetComponent<RigidBody>()->Get()->setMassProps(0, btVector3());
	// sets rigidbody scale
	f->GetTransform()->SetScale(glm::vec3(0.05f, 1.f, 1.f));


	// bottom wall object
	m_entities.push_back(g);
	g->AddComponent(
		new MeshRenderer(
			//sets the model to be used
			Resources::GetInstance()->GetModel("Models/Wall.obj"),
			// sets the shader to be used
			Resources::GetInstance()->GetShader("simple"),
			// sets the texture to be used
			Resources::GetInstance()->GetTexture("Images/Textures/Red.png"))
	);
	MeshRenderer* q= g->GetComponent<MeshRenderer>();
	// sets object position
	g->GetTransform()->SetPosition(glm::vec3(0.f, -7.5f, -25.f));
	// adds rigidbody
	g->AddComponent<RigidBody>();
	// sets rigidbody size and shape
	g->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(25.f, 20.f, 3.f)));
	g->GetComponent<RigidBody>()->Get()->setMassProps(0, btVector3());
	// sets objects scale
	g->GetTransform()->SetScale(glm::vec3(0.05f, 1.f, 1.f));

	// side wall left
	m_entities.push_back(h);
	h->AddComponent(
		new MeshRenderer(
			// sets model to be used
			Resources::GetInstance()->GetModel("Models/Wall.obj"),
			// sets shader to be used
			Resources::GetInstance()->GetShader("simple"),
			// sets texture to be used
			Resources::GetInstance()->GetTexture("Images/Textures/Red.png"))
	);
	MeshRenderer* x = h->GetComponent<MeshRenderer>();
	// sets objects position
	h->GetTransform()->SetPosition(glm::vec3(25.f, -7.5f, 0.f));
	// sets objects rotation
	h->GetTransform()->SetRotation(glm::quat(1.f, 0.f, -1.f, 0.f));
	// adds rigidbody
	h->AddComponent<RigidBody>();
	// sets size and shape of rigidbody 
	h->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(25.f, 20.f, 1.f)));
	h->GetComponent<RigidBody>()->Get()->setMassProps(0, btVector3());
	// sets scale of object
	h->GetTransform()->SetScale(glm::vec3(0.05f, 1.f, 1.f));


	// side wall right
	m_entities.push_back(i);
	i->AddComponent(
		new MeshRenderer(
			// sets model to be used
			Resources::GetInstance()->GetModel("Models/Wall.obj"),
			// sets shader to be used
			Resources::GetInstance()->GetShader("simple"),
			// sets texture to be used
			Resources::GetInstance()->GetTexture("Images/Textures/Red.png"))
	);
	MeshRenderer* s = i->GetComponent<MeshRenderer>();
	// sets objects position
	i->GetTransform()->SetPosition(glm::vec3(-25.f, -7.5f, 0.f));
	// sets objects rotation
	i->GetTransform()->SetRotation(glm::quat(1.f, 0.f, -1.f, 0.f));
	// adds rigidbody
	i->AddComponent<RigidBody>();
	// sets size and shape of rigidbody
	i->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(25.f, 20.f, 1.f)));
	i->GetComponent<RigidBody>()->Get()->setMassProps(0, btVector3());
	// sets scale of object
	i->GetTransform()->SetScale(glm::vec3(0.05f, 1.f, 1.f));



	// inital spider model currently unused
	m_entities.push_back(d);
	d->AddComponent(
		new MeshRenderer(
			Resources::GetInstance()->GetModel("Models/cube.obj"),
			Resources::GetInstance()->GetShader("simple"),
			Resources::GetInstance()->GetTexture("Images/Textures/Wood.jpg"))
	);
	MeshRenderer* t = d->GetComponent<MeshRenderer>();
	d->GetTransform()->SetPosition(glm::vec3(0.f, -5.f , 10.f));
	d->AddComponent<RigidBody>();
	d->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(0.1f, 0.1f, 0.1f)));
	d->GetTransform()->SetScale(glm::vec3(0.01f, 0.01f, 0.01f));
	


	// player 1 mmodel
	m_entities.push_back(b);
	
	b->AddComponent(
		new MeshRenderer(
			// sets model to be used
			Resources::GetInstance()->GetModel("Models/Spaceship.obj"),
			// sets shader to be used
			Resources::GetInstance()->GetShader("simple"),
			// sets texture to be used
			Resources::GetInstance()->GetTexture("Images/Textures/Blue.png"))
	);
	MeshRenderer* n = b->GetComponent<MeshRenderer>();
	// sets position
	b->GetTransform()->SetPosition(glm::vec3(0.f, 1.f, -20.f));
	// adds rigidbody
	b->AddComponent<RigidBody>();
	// sets rigidbody size and shape
	b->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(1.f, 1.f, 1.f)));
	//b->GetComponent<RigidBody>()->Get()->setMassProps(0, btVector3());
	// sets models scale
	b->GetTransform()->SetScale(glm::vec3(0.005f, 0.005f, 0.005f));


	// player 2 model
	m_entities.push_back(c);
	c->AddComponent(
		new MeshRenderer(
			// sets model to be used
			Resources::GetInstance()->GetModel("Models/SShip.obj"),
			// sets shader to be used
			Resources::GetInstance()->GetShader("simple"),
			// sets texture to be used
			Resources::GetInstance()->GetTexture("Images/Textures/Red.png"))
	);
	MeshRenderer* o = c->GetComponent<MeshRenderer>();
	// sets models position

	c->GetTransform()->SetPosition(glm::vec3(1.f, 1.f, -10.f));

	c->GetTransform()->SetPosition(glm::vec3(1.f, 1.f, 20.f));

	// sets rotation
	c->GetTransform()->SetRotation(glm::quat(1.f, 0.f, 30.f, 0.f));
	// adds Rigidbody
	c->AddComponent<RigidBody>();
	// sets rigidbody size and shape
	c->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(0.5f, 0.5f, 0.5f)));
	//b->GetComponent<RigidBody>()->Get()->setMassProps(0, btVector3());
	// sets scale of model
	c->GetTransform()->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));



	for (int i = 0; i < 5;i++)
	{

		
		


	}

	//a = new Entity();
	//m_entities.push_back(a);

	b->AddComponent(cc);

	cc->Start();

	Resources::GetInstance()->ReleaseUnusedResources();

}

void Application::player1Wins()
{


	
	// creates the player 1 wins object


	// adds a 3d object showing player 2 has won
	m_entities.push_back(l);
	l->AddComponent(
		new MeshRenderer(
			// sets the model to be used
			Resources::GetInstance()->GetModel("Models/Player2Wins.obj"),
			// sets the shader to be used
			Resources::GetInstance()->GetShader("simple"),
			// sets the texture to be used
			Resources::GetInstance()->GetTexture("Images/Textures/Blue.png"))
	);
	MeshRenderer* b = l->GetComponent<MeshRenderer>();
	// sets the position of the player 1 wins object
	l->GetTransform()->SetPosition(glm::vec3(0.f, 5.f, 0.f));
	// sets the rotation of the player 1 wins object
	l->GetTransform()->SetRotation(glm::quat(1.f, 0.f, 30.f, 0.f));
	// adds a rigidbody to the player 1 wins object
	l->AddComponent<RigidBody>();
	// sets the rigidbody size of the player 1 wins object
	l->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(1.f, 1.f, 1.f)));
	l->GetComponent<RigidBody>()->Get()->setMassProps(0, btVector3());
	// sets the scale of the player 1 wins object
	l->GetTransform()->SetScale(glm::vec3(2.f, 2.f, 2.f));

	




		
	
	

}

void Application::player2Wins()
{

	// adds a 3d object showing player 2 has won
	m_entities.push_back(j);
	j->AddComponent(
		new MeshRenderer(
			// sets the model to be used
			Resources::GetInstance()->GetModel("Models/Player2Wins.obj"),
			// sets the shader to be used
			Resources::GetInstance()->GetShader("simple"),
			// sets the texture to be used
			Resources::GetInstance()->GetTexture("Images/Textures/Red.png"))
	);
	MeshRenderer* a = j->GetComponent<MeshRenderer>();
	// sets the location for player 2 wins object
	j->GetTransform()->SetPosition(glm::vec3(0.f, 5.f, 0.f));
	// sets the rotation for player 2 wins object
	j->GetTransform()->SetRotation(glm::quat(1.f, 0.f, 30.f, 0.f));
	// adds a rigid body to player 2 wins object
	j->AddComponent<RigidBody>();
	// sets the rigidbody size of player 2 wins object
	j->GetComponent<RigidBody>()->Init(new BoxShape(glm::vec3(1.f, 1.f, 1.f)));
	j->GetComponent<RigidBody>()->Get()->setMassProps(0, btVector3());
	// sets the scale of player 2 wins object
	j->GetTransform()->SetScale(glm::vec3(2.f, 2.f, 2.f));
}


void Application::Loop()
{
	m_appState = AppState::RUNNING;
	//std::cout << playerX << std::endl;
	auto prevTicks = std::chrono::high_resolution_clock::now();
	SDL_Event event;

	while (m_appState != AppState::QUITTING)
	{
		if (Physics::GetInstance()->Collision3D(b->GetComponent<RigidBody>()->Get(), 0, 0, d->GetComponent<RigidBody>()->Get(), 1, 1) == true)
		{
			std::cout << "Hit A" << std::endl;
		}

		if (Physics::GetInstance()->Collision3D(b->GetComponent<RigidBody>()->Get(), 0, 0, c->GetComponent<RigidBody>()->Get(), 1, 1) == true)
		{
			std::cout << "Hit C" << std::endl;
		}

		// code for controling the collision for player 1's ball
		if (ballInPlay == true)
		{
			// controls balls collision with player 2
			if (Physics::GetInstance()->Collision3D(e->GetComponent<RigidBody>()->Get(), 0, 0, c->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				std::cout << "Hit Player 2" << std::endl;

				//c->GetComponent<MeshRenderer>()->ChangeTexture(Resources::GetInstance()->GetTexture("Images/Textures/Blue.png"));
				//Delay(1);
				//timer1();
				//repositions player 1's ball under the map once it has collided
				e->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				
				//using namespace std::this_thread;
				//using namespace std::chrono;

				//sleep_for(nanoseconds(10));
				//sleep_until(system_clock::now() + seconds(1));
				//c->GetComponent<MeshRenderer>()->ChangeTexture(Resources::GetInstance()->GetTexture("Images/Textures/Blue.png"));

				// reduces player 2 lives
				player2Lives--;
				// prints player 2's current lifes to console
				std::cout << player2Lives << std::endl;
				// sets ball in play to false
				ballInPlay = false;
			}
			
			// controls player 1's ball collision with walls
			if (Physics::GetInstance()->Collision3D(e->GetComponent<RigidBody>()->Get(), 0, 0, f->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				//repositions player 1's ball under the map once it has collided
				e->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				ballInPlay = false;			
			}

			if (Physics::GetInstance()->Collision3D(e->GetComponent<RigidBody>()->Get(), 0, 0, g->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				//repositions player 1's ball under the map once it has collided
				e->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				ballInPlay = false;
			}

			if (Physics::GetInstance()->Collision3D(e->GetComponent<RigidBody>()->Get(), 0, 0, h->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				//repositions player 1's ball under the map once it has collided
				e->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				ballInPlay = false;
			}

			if (Physics::GetInstance()->Collision3D(e->GetComponent<RigidBody>()->Get(), 0, 0, i->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				//repositions player 1's ball under the map once it has collided
				e->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				ballInPlay = false;

			}

		}

		// code for controling the collision for player 2's ball
		if (p2Shot == true)
		{
			
			// controls balls collision with player 1
			if (Physics::GetInstance()->Collision3D(k->GetComponent<RigidBody>()->Get(), 0, 0, b->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				//Prints to console that player 1 has been hit
				std::cout << "Hit Player 1" << std::endl;

				//b->GetComponent<MeshRenderer>()->ChangeTexture(Resources::GetInstance()->GetTexture("Images/Textures/Red.png"));
				//repositions player 2's ball under the map once it has collided
				k->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				// reduces player 1's lives
				player1Lives--;

				//using namespace std::this_thread;
				//using namespace std::chrono;

				//sleep_for(nanoseconds(10));
				//sleep_until(system_clock::now() + seconds(1));
				//c->GetComponent<MeshRenderer>()->ChangeTexture(Resources::GetInstance()->GetTexture("Images/Textures/Blue.png"));

				std::cout << player1Lives << std::endl;
				p2Shot = false;

			}

			// controls Player 2's ball collision with walls
			if (Physics::GetInstance()->Collision3D(k->GetComponent<RigidBody>()->Get(), 0, 0, f->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				//repositions player 2's ball under the map once it has collided
				k->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				p2Shot = false;
			}

			if (Physics::GetInstance()->Collision3D(k->GetComponent<RigidBody>()->Get(), 0, 0, g->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				//repositions player 2's ball under the map once it has collided
				k->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				p2Shot = false;
			}

			if (Physics::GetInstance()->Collision3D(k->GetComponent<RigidBody>()->Get(), 0, 0, h->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				//repositions player 2's ball under the map once it has collided
				k->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				p2Shot = false;
			}

			if (Physics::GetInstance()->Collision3D(k->GetComponent<RigidBody>()->Get(), 0, 0, i->GetComponent<RigidBody>()->Get(), 1, 1) == true)
			{
				//repositions player 2's ball under the map once it has collided
				k->GetTransform()->AddPosition(glm::vec3(0.f, -10.f, 0.f));
				p2Shot = false;
			}

		


		}

		// win condition for Player 2
		if (player1Lives <= 0)
		{
			player2Wins();

			
		}

		// win condition for player 1
		if (player2Lives <= 0)
		{ 
			player1Wins();
			
		}

		//b->GetComponent<RigidBody>()->Get()->applyForce

		// switch case events to control character movement, ability to shoot, camera position, which camera is displaying, camera zoom level
		//poll SDL events
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				m_appState = AppState::QUITTING;
				break;
			case SDL_WINDOWEVENT_CLOSE:
				m_appState = AppState::QUITTING;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_a:
					// alters player 1 transfrom to move them left by one
					b->GetTransform()->AddPosition(glm::vec3(1.0f, 0.f, 0.f));
					break;
				case SDLK_d:
					// alters player 1 transfrom to move them right by 1
					b->GetTransform()->AddPosition(glm::vec3(-1.0f, 0.f, 0.f));
					break;
				case SDLK_s:
					// alters player 1 transfrom to move them backwards by 1
					b->GetTransform()->AddPosition(glm::vec3(0.0f, 0.f, -1.f));
					break;
				case SDLK_w:
					// alters player 1 transfrom to move them forward by 1
					b->GetTransform()->AddPosition(glm::vec3(0.0f, 0.f, 1.f));
					break;
				case SDLK_q:
					// alters player 1 rpation to rotate the left by 90 degrees
					b->GetTransform()->AddRotation(glm::quat(1.f, 0.f, 1.f, 0.f));
					//c->GetTransform()->AddRotation(glm::quat(1.f, 0.f, 1.f, 0.f));	
					break;	
				case SDLK_e:
					// alters player 1 rotation to rotate them right by 90 degrees
					b->GetTransform()->AddRotation(glm::quat(1.f, 0.f, -1.f, 0.f));
					//c->GetTransform()->AddRotation(glm::quat(1.f, 0.f, -1.f, 0.f));
					break;
				case SDLK_f:
					if (ballInPlay == false)
					{
						theSoundMgr->getSnd("shoot")->play(0);
						Spawn();
						// applies a central impules to player 1 ball to shoot it forwards
						e->GetComponent<RigidBody>()->Get()->applyCentralImpulse(btVector3(0.0f, 0.0f, 35.0f));
						ballInPlay = true;
					}
					break;
				case SDLK_r:
					// resets player 1's position in case they get stuck

					b->GetTransform()->SetPosition(glm::vec3(0.f, 1.f, 0.f));
				case SDLK_3:
					m_mainCamera->SetProjOrtho(-500, (float)WINDOW_W, 0, (float)WINDOW_H, 0.1f, 1000.f);

					b->GetTransform()->SetPosition(glm::vec3(0.f, 1.f, -20.f));

					break;
				/*case SDLK_3:
					m_mainCamera->SetProjOrtho(100.f, (float)WINDOW_W, 0, (float)WINDOW_H, 0.1f, 1000.f);
					break;*/
				case SDLK_4:
					m_mainCamera->SetProjPersp(45.f, (float)WINDOW_W / (float)WINDOW_H, 0.1f, 1000.f);
					break;
				case SDLK_m:
					if (camDistance <= 0) {
						camDistance += 10.0f;
					}
					std::cout << camDistance;
					break;
				case SDLK_n:
					if (camDistance >= 0) {
						camDistance -= 10.0f;
					}
					std::cout << camDistance;
					break;
				case SDLK_2:
					b->AddComponent(cc);
					cc->Start();
					m_mainCamera->SetM_camera();
					break;
				case SDLK_1:
					a->AddComponent(cc);
					cc->Start2();
					m_mainCamera->SetS_camera();
					break;
				case SDLK_i:
					c->GetTransform()->AddPosition(glm::vec3(0.0f, 0.f, 1.f));
					break;
				case SDLK_k:
					c->GetTransform()->AddPosition(glm::vec3(0.0f, 0.f, -1.f));
					break;
				case SDLK_j:
					c->GetTransform()->AddPosition(glm::vec3(1.0f, 0.f, 0.f));
					break;
				case SDLK_l:
					c->GetTransform()->AddPosition(glm::vec3(-1.0f, 0.f, 0.f));
					break;
				case SDLK_h:
					if (p2Shot == false)
					{
						theSoundMgr->getSnd("shoot")->play(0);
						Spawn2();
						k->GetComponent<RigidBody>()->Get()->applyCentralImpulse(btVector3(0.0f, 0.0f, -25.0f));
						p2Shot = true;
					}
					break;
				case SDL_MOUSEMOTION:
					INPUT->MoveMouse(glm::ivec2(event.motion.xrel, event.motion.yrel));
					SDL_SetRelativeMouseMode(SDL_TRUE);
					if (SDL_GetRelativeMouseMode() == true)
					{
						b->GetTransform()->RotateEulerAxis((m_worldDeltaTime * 1) * event.motion.xrel, b->GetTransform()->GetUp());
						b->GetTransform()->RotateEulerAxis((m_worldDeltaTime * 1) * event.motion.yrel, b->GetTransform()->GetRight());
					}
					break;
				case SDLK_b:
					if (musicon == true)
					{
						Mix_PauseMusic();
						musicon = false;
						}
					else if(musicon == false)
					{
						Mix_ResumeMusic();
						musicon = true;
					}
						break;
				case SDLK_u:
					c->GetTransform()->AddRotation(glm::quat(1.f, 0.f, 1.f, 0.f));
					c->GetTransform()->SetPosition(glm::vec3(0.f, 1.f, 0.f));
					break;
				case SDLK_o:
					c->GetTransform()->AddRotation(glm::quat(1.f, 0.f, -1.f, 0.f));
					break;
				}
				




				//record when the user releases a key
			case SDL_MOUSEMOTION:
				INPUT->MoveMouse(glm::ivec2(event.motion.xrel, event.motion.yrel));
				break;
			}
		}

		auto currentTicks = std::chrono::high_resolution_clock::now();
		float deltaTime = (float)std::chrono::duration_cast<std::chrono::microseconds>(currentTicks - prevTicks).count() / 100000;
		m_worldDeltaTime = deltaTime;
		prevTicks = currentTicks;

		Physics::GetInstance()->Update(deltaTime);
		//update and render all entities
		Update(deltaTime);
		Render();



		SDL_GL_SwapWindow(m_window);
	}
}

void Application::Quit()
{
	//Close SDL
	Physics::GetInstance()->Quit();
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
}

void Application::Update(float deltaTime)
{
	for (auto& a : m_entities)
	{
		a->OnUpdate(deltaTime);
	}
}

void Application::Render()
{
	/* Clear context */
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_mainCamera->Recalculate();
	for (auto& a : m_entities)
	{
		a->OnRender();
	}
}


Application::~Application()
{

}

Application* Application::GetInstance()
{
	if (m_application == nullptr)
	{
		m_application = new Application();
	}
	return m_application;
}

void Application::Run()
{
	Init();
	Loop();
	Quit();
}

void Application::SetCamera(Camera* camera)
{
	if (camera != nullptr)
	{
		m_mainCamera = camera;
	}
}
