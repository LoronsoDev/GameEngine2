/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <gltk/Mesh.hpp>
#include <gltk/Cube.hpp>
#include <gltk/Model.hpp>
#include <gltk/Model_Obj.hpp>
#include <gltk/Node.hpp>
#include <gltk/Light.hpp>
#include <gltk/Render_Node.hpp>
#include <gltk/Drawable.hpp>
#include <gltk/Material.hpp>
#include <gltk/Camera.hpp>

#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include <sdl2/SDL_mixer.h>
#include <glm/glm.hpp>
#include "Game.h"

#include <Deserializer/Scene3DDeserializer.h>

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/RigidbodyComponent.h>
#include <Components/Node3DComponent.h>

#include <Systems/Movement3DSystem.h>
#include <Systems/ModelRender3DSystem.h>
#include <Systems/EntityStartup3DSystem.h>
#include <Systems/PhysicsSystem.h>

#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include <b2_revolute_joint.h>
#include <b2_circle_shape.h>


using namespace engine;

namespace game
{
	Game::Game(Window& window, Kernel& kernel, std::shared_ptr<EventBus> eventBus)
	{
		registry = std::make_unique<Registry>();
		assetManager = std::make_unique<AssetManager>();
		this->eventBus = eventBus;

		this->kernel = &kernel;
		this->window = &window;

		window.SetWindowedFullscreen();
		window.SetVsync(true);

		spdlog::info("Game constructor called");
	}

	void Game::SetupScene()
	{
		/*
		*	We tell the eventBus (event manager for all entities) that we are listening for all events of the type InputEvent.
		*	eventBus will call the function we pass as parameter whenever the event is fired.
		*/
		eventBus->AddEventListener<InputEvent>(this, &Game::OnInputRegistered);
		eventBus->AddEventListener<OnTriggerEntryEvent>(this, &Game::OnTriggerEntry);

		/*
		*	Deserializes and spawns all static objects - not used for now...
		*/
		/*Scene3DDeserializer deserializer("../../../assets/scenes/test.scene", registry.get(), window);
		deserializer.Initialize();*/

		registry->AddSystem<Movement3DSystem>();
		registry->AddSystem<ModelRender3DSystem>(*window);
		registry->AddSystem<EntityStartup3DSystem>();
		registry->AddSystem<PhysicsSystem>(eventBus);

		/*
		*	We load up the .wav sounds we want to use in this demo.
		*/
		/*sound = Mix_LoadWAV("../../../assets/sounds/hit.wav");
		if (sound == NULL)
		{
			spdlog::error("Couldn't load .wav from the wanted path");
		}
		death = Mix_LoadWAV("../../../assets/sounds/death.wav");
		Mix_Volume(-1, 60);*/
		
	/*Ported box2D stuff*/
	//{
	//	m_speed = 200.0f;

	//	b2Body* ground = NULL;
	//	{
	//		b2BodyDef bd;
	//		ground = m_world->CreateBody(&bd);

	//		b2EdgeShape shape;

	//		b2FixtureDef fd;
	//		fd.shape = &shape;
	//		fd.density = 0.0f;
	//		fd.friction = 0.6f;
	//		shape.SetTwoSided(b2Vec2(-10.0f, 30.0f), b2Vec2(10.0f, 30.0f));
	//		ground->CreateFixture(&fd);

	//		float heights[10] = { 30.0f, 30.0f, 28.0f, 24.0f, 16.0f, 14.0f, 12.0f, 14.0f, 16.0f, 18.0f };


	//		float runningX = 0.0f, startingY = 30.0f, dx = 8.0f;

	//		for (int32 i = 0; i < 10; ++i)
	//		{
	//			float y2 = heights[i];
	//			shape.SetTwoSided(b2Vec2(runningX, startingY), b2Vec2(runningX + dx, y2));
	//			ground->CreateFixture(&fd);
	//			startingY = y2;
	//			runningX += dx;
	//		}

	//		float second_line_heights[10] = { 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 128.f };
	//		runningX += dx * 5;
	//		startingY = 0.0f;
	//		for (int32 i = 0; i < 10; ++i)
	//		{
	//			float y2 = second_line_heights[i];
	//			shape.SetTwoSided(b2Vec2(runningX, startingY), b2Vec2(runningX + dx, y2));
	//			ground->CreateFixture(&fd);
	//			startingY = y2;
	//			runningX += dx;
	//		}

	//		shape.SetTwoSided(b2Vec2(-10.0f, 30.0f), b2Vec2(10.0f, 30.0f));
	//		ground->CreateFixture(&fd);
	//	}

	//	// Boxes
	//	{
	//		b2PolygonShape box;
	//		box.SetAsBox(0.5f, 0.5f);

	//		b2Body* body = NULL;
	//		b2BodyDef bd;
	//		bd.type = b2_dynamicBody;

	//		bd.position.Set(230.0f, 0.5f);
	//		body = m_world->CreateBody(&bd);
	//		body->CreateFixture(&box, 0.5f);

	//		bd.position.Set(230.0f, 1.5f);
	//		body = m_world->CreateBody(&bd);
	//		body->CreateFixture(&box, 0.5f);

	//		bd.position.Set(230.0f, 2.5f);
	//		body = m_world->CreateBody(&bd);
	//		body->CreateFixture(&box, 0.5f);

	//		bd.position.Set(230.0f, 3.5f);
	//		body = m_world->CreateBody(&bd);
	//		body->CreateFixture(&box, 0.5f);

	//		bd.position.Set(230.0f, 4.5f);
	//		body = m_world->CreateBody(&bd);
	//		body->CreateFixture(&box, 0.5f);
	//	}

	//	// Car
	//	{
	//		b2PolygonShape chassis;
	//		chassis.SetAsBox(4.f, 0.4f);

	//		b2CircleShape circle;
	//		circle.m_radius = 0.8f;

	//		b2BodyDef bd;
	//		bd.type = b2_dynamicBody;
	//		bd.position.Set(0.0f, 41.0f);
	//		m_car = m_world->CreateBody(&bd);
	//		m_car->CreateFixture(&chassis, 1.0f);

	//		/* CAR_CONTAINER */
	//		{
	//			b2Body* car_container_bot;
	//			bd.type = b2_dynamicBody;
	//			bd.position.Set(m_car->GetPosition().x, m_car->GetPosition().y + 0.6f);
	//			car_container_bot = m_world->CreateBody(&bd);
	//			chassis.SetAsBox(4.f, 0.2f);
	//			car_container_bot->CreateFixture(&chassis, 1.0f);

	//			chassis.SetAsBox(0.2f, .7f);
	//			b2Body* car_container_left;
	//			bd.type = b2_dynamicBody;
	//			bd.position.Set(car_container_bot->GetPosition().x - 4.f, car_container_bot->GetPosition().y + .5f);
	//			car_container_left = m_world->CreateBody(&bd);
	//			car_container_left->CreateFixture(&chassis, 1.0f);

	//			b2Body* car_container_right;
	//			bd.type = b2_dynamicBody;
	//			bd.position.Set(car_container_bot->GetPosition().x + 4.f, car_container_bot->GetPosition().y + .5f);
	//			car_container_right = m_world->CreateBody(&bd);
	//			car_container_right->CreateFixture(&chassis, 1.0f);

	//			/* Welding them together... */
	//			{
	//				b2WeldJointDef jd;
	//				jd.Initialize(car_container_bot,
	//					car_container_left,
	//					{ car_container_bot->GetPosition() });
	//				m_world->CreateJoint(&jd);

	//				jd.Initialize(car_container_bot,
	//					car_container_right,
	//					{ car_container_bot->GetPosition() });
	//				m_world->CreateJoint(&jd);

	//				b2RevoluteJointDef container_motor;
	//				container_motor.bodyA = m_car;
	//				container_motor.localAnchorB = { -4.f, 0.f };
	//				container_motor.enableMotor = true;
	//				container_motor.maxMotorTorque = 500.0f;
	//				container_motor.enableLimit = true;

	//				container_motor.motorSpeed = 0.0f;
	//				container_motor.localAnchorA = { -4.f, 0.6f };
	//				container_motor.bodyB = car_container_bot;
	//				container_motor.lowerAngle = -100.0f * b2_pi / 180.0f;
	//				container_motor.upperAngle = 50.f * b2_pi / 180.0f;
	//				m_car_container_joint_motor = (b2RevoluteJoint*)m_world->CreateJoint(&container_motor);

	//			}

	//		}

	//		/* WHEELS */
	//		{
	//			b2FixtureDef fd;
	//			fd.shape = &circle;
	//			fd.density = 1.0f;
	//			fd.friction = 0.9f;

	//			bd.position.Set(m_car->GetPosition().x - 3.5, m_car->GetPosition().y - 0.6f);
	//			m_wheel1 = m_world->CreateBody(&bd);
	//			m_wheel1->CreateFixture(&fd);

	//			bd.position.Set(m_car->GetPosition().x + 3.5, m_car->GetPosition().y - 0.6f);
	//			m_wheel2 = m_world->CreateBody(&bd);
	//			m_wheel2->CreateFixture(&fd);

	//			b2WheelJointDef jd;
	//			b2Vec2 axis(0.0f, 1.0f);

	//			float mass1 = m_wheel1->GetMass();
	//			float mass2 = m_wheel2->GetMass();

	//			float hertz = 4.0f;
	//			float dampingRatio = 0.7f;
	//			float omega = 2.0f * b2_pi * hertz;

	//			jd.Initialize(m_car, m_wheel1, m_wheel1->GetPosition(), axis);
	//			jd.motorSpeed = 0.0f;
	//			jd.maxMotorTorque = 15.0f;
	//			jd.enableMotor = true;
	//			jd.stiffness = mass1 * omega * omega;
	//			jd.damping = 2.0f * mass1 * dampingRatio * omega;
	//			jd.lowerTranslation = -0.25f;
	//			jd.upperTranslation = 0.25f;
	//			jd.enableLimit = true;
	//			m_spring1 = (b2WheelJoint*)m_world->CreateJoint(&jd);

	//			jd.Initialize(m_car, m_wheel2, m_wheel2->GetPosition(), axis);
	//			jd.motorSpeed = 0.0f;
	//			jd.maxMotorTorque = 15.0f;
	//			jd.enableMotor = true;
	//			jd.stiffness = mass2 * omega * omega;
	//			jd.damping = 2.0f * mass2 * dampingRatio * omega;
	//			jd.lowerTranslation = -0.25f;
	//			jd.upperTranslation = 0.25f;
	//			jd.enableLimit = true;
	//			m_spring2 = (b2WheelJoint*)m_world->CreateJoint(&jd);
	//		}

	//		{
	//			b2Body* left_arm;
	//			//We can reuse the body definition
	//			bd.type = b2_kinematicBody;
	//			bd.position.Set(m_car->GetPosition().x + 10, m_car->GetPosition().y + 0.6f);
	//			left_arm = m_world->CreateBody(&bd);
	//			chassis.SetAsBox(2.f, 0.2f);
	//			left_arm->CreateFixture(&chassis, 1.0f);
	//			left_arm->SetTransform(left_arm->GetPosition(), 100);

	//			b2Body* right_arm;
	//			bd.type = b2_kinematicBody;
	//			bd.position.Set(left_arm->GetPosition().x + 3.f, left_arm->GetPosition().y);
	//			right_arm = m_world->CreateBody(&bd);
	//			right_arm->CreateFixture(&chassis, 1.0f);
	//			right_arm->SetTransform(right_arm->GetPosition(), -100);

	//			/* Welding them together... */
	//			{
	//				b2WeldJointDef jd;
	//				jd.Initialize(left_arm,
	//					right_arm,
	//					{ left_arm->GetPosition() });
	//				m_world->CreateJoint(&jd);
	//			}
	//		}

	//	}
	//}
	/**/
		// ... do stuff ...

		//To destroy joints:
		//NULLIFY to avoid errors...

		/*world->DestroyJoint(joint);
		joint = nullptr;*/

		/*
		*	We start up and add all needed components to the dynamic (moving) entities.
		*/

		m_speed = 30.0f;

		car_base = registry->CreateEntity();
		std::shared_ptr< glt::Model  > carBaseCubeModel(new glt::Model);
		carBaseCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		car_base.AddComponent<TransformComponent>(glm::vec3(-90.f, 60.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(8.f, 0.5f, 0.5f));
		car_base.AddComponent<RigidbodyComponent>();
		car_base.AddComponent<Node3DComponent>("car_base", carBaseCubeModel);
		car_base.AddComponent<BoxColliderComponent>(false, 4.f, 0.5f, 1.0f, 0.9f);

		TransformComponent & carbaseTransform = car_base.GetComponent<TransformComponent>();
		wheel_1 = registry->CreateEntity();
		std::shared_ptr< glt::Model  > wheelCubeModel(new glt::Model);
		wheelCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wheel_1.AddComponent<TransformComponent>(
			glm::vec3(carbaseTransform.position.x -5,
			carbaseTransform.position.y -1, 0.f),
			glm::vec3(0, 0, 0), glm::vec3(1,1,1));
		wheel_1.AddComponent<RigidbodyComponent>();
		wheel_1.AddComponent<Node3DComponent>("wheel_1", wheelCubeModel);
		wheel_1.AddComponent<CircleColliderComponent>(false);

		wheel_2 = registry->CreateEntity();
		std::shared_ptr< glt::Model  > cubeModel(new glt::Model);
		cubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wheel_2.AddComponent<TransformComponent>(
			glm::vec3(carbaseTransform.position.x + 5,
				carbaseTransform.position.y + -1, 0.f),
			glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		wheel_2.AddComponent<RigidbodyComponent>();
		wheel_2.AddComponent<Node3DComponent>("wheel_2", cubeModel);
		wheel_2.AddComponent<CircleColliderComponent>(false);

		Entity ground = registry->CreateEntity();
		std::shared_ptr< glt::Model  > groundCubeModel(new glt::Model);
		groundCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		ground.AddComponent<TransformComponent>(glm::vec3(-100.f, 50.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(20.f, 1, 1));
		ground.AddComponent<RigidbodyComponent>(staticBody);
		ground.AddComponent<Node3DComponent>("ground", groundCubeModel);
		ground.AddComponent<BoxColliderComponent>(false, 20.f, 1);

		Entity down_ramp = registry->CreateEntity();
		std::shared_ptr< glt::Model  > downRampCubeModel(new glt::Model);
		downRampCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		down_ramp.AddComponent<TransformComponent>(glm::vec3(-52.f, 21.f, 0.f), glm::vec3(0, 0, -0.8f), glm::vec3(40.f, 1, 1));
		down_ramp.AddComponent<RigidbodyComponent>(staticBody);
		down_ramp.AddComponent<Node3DComponent>("down_ramp", downRampCubeModel);
		down_ramp.AddComponent<BoxColliderComponent>(false, 40.f, 1);

		Entity down_ground = registry->CreateEntity();
		std::shared_ptr< glt::Model  > downgroundCubeModel(new glt::Model);
		downgroundCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		down_ground.AddComponent<TransformComponent>(glm::vec3(-20.f, -8.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(5.f, 1, 1));
		down_ground.AddComponent<RigidbodyComponent>(staticBody);
		down_ground.AddComponent<Node3DComponent>("down_ground", downgroundCubeModel);
		down_ground.AddComponent<BoxColliderComponent>(false, 5.f, 1);

		Entity up_ramp = registry->CreateEntity();
		std::shared_ptr< glt::Model  > upRampCubeModel(new glt::Model);
		upRampCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		up_ramp.AddComponent<TransformComponent>(glm::vec3(-11.f, -6.3f, 0.f), glm::vec3(0, 0, 0.3f), glm::vec3(5.f, 1, 1));
		up_ramp.AddComponent<RigidbodyComponent>(staticBody);
		up_ramp.AddComponent<Node3DComponent>("up_ramp", upRampCubeModel);
		up_ramp.AddComponent<BoxColliderComponent>(false, 5.f, 1);

		Entity right_ground = registry->CreateEntity();
		std::shared_ptr< glt::Model  > rightGroundCubeModel(new glt::Model);
		rightGroundCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		right_ground.AddComponent<TransformComponent>(glm::vec3(60.f, -10.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(30.f, 1, 1));
		right_ground.AddComponent<RigidbodyComponent>(staticBody);
		right_ground.AddComponent<Node3DComponent>("right_ground", rightGroundCubeModel);
		right_ground.AddComponent<BoxColliderComponent>(false, 30.f, 1, 1, 0.1f);

		Entity finish_wall = registry->CreateEntity();
		std::shared_ptr< glt::Model  > finishWallCubeModel(new glt::Model);
		finishWallCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		finish_wall.AddComponent<TransformComponent>(glm::vec3(90.f, -10.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(1.f, -10.f, 1));
		finish_wall.AddComponent<RigidbodyComponent>(staticBody);
		finish_wall.AddComponent<Node3DComponent>("finish_wall", finishWallCubeModel);
		finish_wall.AddComponent<BoxColliderComponent>(false, 0.f, 10.f);


		Entity light = registry->CreateEntity();
		std::shared_ptr< glt::Light  > lightNode(new glt::Light);
		lightNode->set_color(glm::vec3(0.4f, 0.7f, 0.929f));
		light.AddComponent<TransformComponent>(glm::vec3(10.f, 10.f, -30.f), glm::vec3(0.f, 0.f, 0.f));
		light.AddComponent<Node3DComponent>("light", lightNode);


		cam = registry->CreateEntity();
		std::shared_ptr< glt::Camera > cameraNode(new glt::Camera(20.f, 1.f, 500.f, 1.f));

		cam.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 100.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		cam.AddComponent<Node3DComponent>("camera", cameraNode);


		/*
		*	Telling the kernel what tasks does it need to initialize and what tasks does it have to keep running in loop.
		*/

		//Update registry to process the entities that are waiting
		kernel->InitializeTask(*registry);
		kernel->InitializeTask(registry->GetSystem<ModelRender3DSystem>());
		kernel->InitializeTask(registry->GetSystem<EntityStartup3DSystem>());
		kernel->InitializeTask(registry->GetSystem<PhysicsSystem>());

		//Physics need to be calculated before any rendering is done...
		kernel->AddPriorizedRunningTask(registry->GetSystem<PhysicsSystem>());

		kernel->AddRunningTask(registry->GetSystem<Movement3DSystem>());
		kernel->AddRunningTask(registry->GetSystem<ModelRender3DSystem>());
	}

	bool Game::Initialize()
	{
		back_wheel_motor  = registry->GetSystem<PhysicsSystem>().MotorizeAsWheel(&wheel_1, &car_base, 1500.f, {0, -1});
		front_wheel_motor = registry->GetSystem<PhysicsSystem>().MotorizeAsWheel(&wheel_2, &car_base, 1500.f, {0, -1});

		return true;
	}

	/*
	*	Runs every frame. Equivalent to similar functions in other engines like Update().
	*/
	void Game::Run(float deltaTime)
	{

		Movement3DSystem& movement3DSystem = registry->GetSystem<Movement3DSystem>();

		b2Vec2 position = wheel_2.GetComponent<RigidbodyComponent>().body->GetPosition();
		float angle = wheel_2.GetComponent<RigidbodyComponent>().body->GetAngle();
		TransformComponent& playerTransform = wheel_2.GetComponent<TransformComponent>();

		b2Vec2 wheelPosition = wheel_1.GetComponent<RigidbodyComponent>().body->GetPosition();
		float wheelAngle = wheel_1.GetComponent<RigidbodyComponent>().body->GetAngle();
		TransformComponent& wheelTransform = wheel_1.GetComponent<TransformComponent>();

		//Cam need Rigidbody to move... Enable if wanted.
		//cam.GetComponent<TransformComponent>().SetTransformation(glm::vec3(m_car->GetPosition().x, 0.f, 100.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		
		/*if (playerTransform.position.y > 14)
		{
			auto& transform = player.GetComponent<TransformComponent>();
			transform.position.y = 13.95f;
			registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
			Mix_PlayChannel(-1, sound, 0);
		}*/

	}

	void Game::OnInputRegistered(InputEvent& event)
	{
		RigidbodyComponent& cubeRigidbody = wheel_2.GetComponent<RigidbodyComponent>();
		switch (event.action)
		{
		case InputEvent::Action::QUIT:
			kernel->Stop();
			break;
		case InputEvent::Action::FORWARD:
			/*m_car_container_joint_motor->SetMotorSpeed(20.0f);*/
			break;
		case InputEvent::Action::BACKWARDS:
			back_wheel_motor.SetMotorSpeed(-1 * back_wheel_motor.GetMotorSpeed());
			front_wheel_motor.SetMotorSpeed(-1 * front_wheel_motor.GetMotorSpeed());
			if(back_wheel_motor.GetMotorSpeed() < 1.f)
			{
				back_wheel_motor.SetMotorSpeed(0);
				front_wheel_motor.SetMotorSpeed(0);
			}
			break;
		case InputEvent::Action::LEFT:
			back_wheel_motor.SetMotorSpeed(m_speed);
			front_wheel_motor.SetMotorSpeed(m_speed);
			break;
		case InputEvent::Action::RIGHT:
			back_wheel_motor.SetMotorSpeed(-m_speed);
			front_wheel_motor.SetMotorSpeed(-m_speed);
			break;
		}
	}

	void Game::OnTriggerEntry(OnTriggerEntryEvent& event)
	{
		spdlog::info("TRIGGER ENTRY");
	}
}