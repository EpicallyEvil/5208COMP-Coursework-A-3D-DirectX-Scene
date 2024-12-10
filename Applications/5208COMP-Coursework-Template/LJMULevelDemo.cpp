//------------Include the Application Header File----------------------------
#include "LJMULevelDemo.h"

//------------DX TK AND STD/STL Includes-------------------------------------
#include <sstream>

//------------Include Hieroglyph Engine Files--------------------------------

//Include the Logging System
#include "Log.h"

//Include the Event System
#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"
#include "ScriptManager.h"

//Include the DirectX Rendering Components
#include "PipelineManagerDX11.h"
#include "BlendStateConfigDX11.h"
#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "MaterialGeneratorDX11.h"

#include "FirstPersonCamera.h"
#include "../../Source/Objects/TextActor.h"

#include "LJMUMeshOBJ.h"
#include <FileSystem.h>

#include <SamplerStateConfigDX11.h>
#include <iomanip> // For std::fixed and std::setprecision for a cleaner hud presentation


//Add a Using Directive to avoid typing Glyph3 for basic constructs
using namespace Glyph3;
//Include our own application Namespace
using namespace LJMUDX;

LJMULevelDemo AppInstance; 

//---------CONSTRUCTORS-------------------------------------------------------

LJMULevelDemo::LJMULevelDemo():
m_pRender_text(nullptr),
m_pRenderView(nullptr),
m_pCamera(nullptr),
m_pRenderer11(nullptr),
m_pWindow(nullptr),
m_iSwapChain(0),
m_DepthTarget(nullptr),
m_RenderTarget(nullptr)
{
	
}

//---------METHODS------------------------------------------------------------

//////////////////////////////////////
// Get the Window Name of this Application
//////////////////////////////////////
std::wstring LJMULevelDemo::GetName()
{
	return(std::wstring(L"5108COMP: Coursework - Aaron Forrester"));
}

/////////////////////////////////////
// Assemble our Input Layouts for this
// Stage of the Pipeline.
/////////////////////////////////////
void LJMULevelDemo::inputAssemblyStage()
{			
	//-----SETUP OUR GEOMETRY FOR THIS SCENE-----------------------------------------
	m_racetrackTexture = RendererDX11::Get()->LoadTexture(L"racetrack.png");
	m_treetrunkTexture = RendererDX11::Get()->LoadTexture(L"treetrunk.png");
	m_leavesTexture = RendererDX11::Get()->LoadTexture(L"leaves.png");
	m_skyboxTexture = RendererDX11::Get()->LoadTexture(L"sky1.png");

	// Racetrack setup
	m_platformActor = new LJMUGeometryActor();
	m_platformActor->SetColor(Vector4f(0.6f, 0.3f, 0.1f, 1.0f));
	m_platformActor->DrawRect( Vector3f(0.0f, 0.0f, 0.0f),
														Vector3f(1.0f, 0.0f, 0.0f),
														Vector3f(0.0f, 0.0f, -1.0f),
														Vector2f(m_platformWidth, m_platformLength));
	setupMaterialProperties(m_platformActor->m_pLitTexturedMaterial);
	setLights2Material(m_platformActor->m_pLitTexturedMaterial);
	m_platformActor->UseLitTexturedMaterial(m_racetrackTexture);
	m_platformActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	this->m_pScene->AddActor(m_platformActor);

	// Skybox actor 
	m_skyboxActor = new LJMUGeometryActor();
	m_skyboxActor->SetColor(Vector4f(0.6f, 0.3f, 0.1f, 1.0f));
	m_skyboxActor->DrawSphere(Vector3f(1.0f, 1.0f, 1.0f) , 450,   80,   80);
	m_skyboxActor->UseSolidMaterial();
	m_skyboxActor->LJMUGeometryActor::UseSkyboxMaterial(m_skyboxTexture);
	m_skyboxActor->GetNode()->Position() = Vector3f(0.0f, -25.0f, 0.0f);
	this->m_pScene->AddActor(m_skyboxActor);

	PointLight* tlight = new PointLight();
	tlight->GetSpecular() = Vector4f(1.2f, 1.2f, 1.2, 1);
	tlight->GetDiffuse() = Vector4f(1.2f, 1.2f, 1.2, 1);
	tlight->GetAmbient() = Vector4f(1.2f, 1.2f, 1.2, 1);
	tlight->GetNode()->Position() = Vector3f(-128.0f, 128.0f, -128.0f);
	m_pScene->AddLight(tlight);

	m_carActor = new GeometryActor();
	//-------------------------------<<<<<This seems to work for now so will leave it as is>>>>>-------------------------------------------
	//m_carActor->SetColor(Vector4f(0.0f, 1.0f, 0.0f, 1.0f)); <<<<<This seems to work for now so will leave it as is>>>>>
	//m_carActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(4.0f, 8.0f, 12.0f));
	//m_carActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	//this->m_pScene->AddActor(m_carActor);

	const int NumberOfCheckpoints = 85;
	Vector3f checkpointCoords[NumberOfCheckpoints] = {
		{101, 0, 176},
		{122, 0, 177 },
		{149, 0, 170},
		{170, 0, 159},
		{187, 0, 144},
		
		{194, 0, 139},
		{207, 0, 112},
		{213, 0, 90},
		{214, 0, 69},
		{211, 0, 49},

		{206, 0 , 27},
		{185, 0, 3},
		{172, 0, -8},
		{158, 0, -17},
		{139, 0, -31},

		{125, 0, -40},
		{13, 0, -114},
		{-11, 0, -127},
		{-28, 0, -131},
		{-47, 0, -131},
		{-65, 0, -126},
		{-77, 0, -119},

		{-88, 0 , -108},
		{-100, 0, -86},
		{-103, 0, -70},
		{-103, 0, -56},
		{-100, 0, -44},

		{-91, 0, -29},
		{-67, 0, -5},
		{-48, 0, 4},
		{-24, 0, 10},	
		{-7, 0, 12},
		{20, 0, 12},
		{40, 0, 8},
		{58, 0, 4},	
		{74, 0, 1},
		{92, 0, -0},

		{101, 0 , 0},
		{135, 0, 9},
		{152, 0, 20},
		{162, 0, 32},
		{170, 0, 44},
		{175, 0, 60},
		{177, 0, 74},

		{178, 0, 87},
		{168, 0, 108},
		{158, 0, 121},		
		{145, 0, 130},
		{128, 0, 138},
		{112, 0, 140},

		{99, 0, 139},
		{76, 0, 131},
		{62, 0, 122},

		{-71, 0 , 33},
		{-106, 0, 10},
		{-118, 0, -2},
		{-125, 0, -13},
		{-132, 0, -26},
		{-135, 0, -37},

		{-138, 0, -55},
		{-140, 0, -69},
		{-137,  0, -88},
		{-132,  0, -104},
		{-123,  0, -120},

		{-108, 0, -154},
		{-106, 0, -167},
		{-109,  0, -178},
		{-115, 0, -188},

		{-126, 0 , -195},
		{-146, 0, -195},
		{-156, 0, -190},
		{-168, 0, -179},
		{-178, 0, -165},

		{-180, 0, -157},
		{-185, 0, -127},
		{-185, 0, -108},
		{-183, 0, -93},

		{-175, 0, -62},
		{-169, 0, -24},
		{-162, 0, -9},
		{-153, 0, 7},

		{-130, 0 , 33},
		{-114, 0, 48},

		{52, 0, 158},
		{68, 0, 168},
		};

	for (int i = 0; i < NumberOfCheckpoints; i++)
	{
		m_checkpoints.push_back(checkpointCoords[i]);
	}
	
	for (int i = 0; i < m_checkpoints.size(); i++)
	{
		LJMUGeometryActor* pathSegmentActor = new LJMUGeometryActor();
		pathSegmentActor->SetColor(Vector4f(1.0f, 0.1f, 0.1f, 1.0f));
		int startidx = i;
		int endidx = (i + 1) % m_checkpoints.size();
		Vector3f cylinderStartPoint = m_checkpoints[startidx] + Vector3f(0, 3, 0);
		Vector3f cylinderEndPoint = m_checkpoints[endidx] + Vector3f(0, 3, 0);
		//pathSegmentActor->DrawCylinder(cylinderStartPoint, cylinderEndPoint, 0.4f, 0.4f); // Can comment out to make invis
		pathSegmentActor->UseSolidMaterial();
		pathSegmentActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
		this->m_pScene->AddActor(pathSegmentActor);

		float carTextSize = 10.0f;
		m_carLabelText = new TextActor();
		m_carLabelText->SetColor(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		m_carLabelText->SetText(L"Player 1");
		m_carLabelText->SetLineJustification(LineJustification::RIGHT);
		m_carLabelText->GetNode()->Scale() = Vector3f(1.0f, 1.0f, 1.0f) * carTextSize;
		this->m_pScene->AddActor(m_carLabelText);
	}

	m_currentCheckpointIndex = 0;
	m_nextCheckpointIndex = m_currentCheckpointIndex + 1;

	Matrix3f tstartRotation;

	m_referenceCarDirection = Vector3f(0, 0, 1);
	m_currentCarDirection = m_referenceCarDirection;
	m_targetCarDirection = m_checkpoints[m_nextCheckpointIndex] - m_checkpoints[m_currentCheckpointIndex];
	m_carDistance2NextCheckpoint = m_targetCarDirection.Magnitude();
	m_targetCarDirection.Normalize();

	float angle = acos(m_referenceCarDirection.Dot(m_targetCarDirection));

	Vector3f axis = m_referenceCarDirection.Cross(m_targetCarDirection);
	axis.Normalize();

	tstartRotation.RotationEuler(axis, angle);

	m_carActor->GetNode()->Rotation() = tstartRotation;
	m_carActor->GetNode()->Position() = m_checkpoints[m_currentCheckpointIndex];

	m_carLinearSpeed = 40.0f; //40.0f
	m_carAngularSpeed = 1.5f; //1.5f

	// m_carState = 0 : Car is moving forward
	// m_carState = 1 : Car is turning around
	m_carState = 0;

	// Code for spawning in the .obj of the Lamborginhi
	Vector4f base_colour = Vector4f(1, 1, 1, 1);
	m_carTexture = RendererDX11::Get()->LoadTexture(L"Lamborginhi_Aventador_diffuse.png");
	BasicMeshPtr car_geometry = this->generateOBJMesh(L"Lamborghini_Aventador_triangles.obj", base_colour);
	MaterialPtr car_material = this->createLitTexturedMaterial();

	setupMaterialProperties(car_material);
	setLights2Material(car_material);

	applyTexture2Material(car_material, m_carTexture);
	m_carActor = new GeometryActor();
	m_carActor->GetBody()->SetGeometry(car_geometry);
	m_carActor->GetBody()->SetMaterial(car_material);
	m_carActor->GetBody()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	m_carActor->GetBody()->Scale() = Vector3f(0.05f, 0.05f, 0.05f);
	this->m_pScene->AddActor(m_carActor);

	// Trees code
	float treetrunklength = 8.0f;
	float treetrunksize = 1.5f;

	// Array of tree coordinates
	const int NumOfTrees = 10;
	Vector3f treePos[NumOfTrees] = {
		{ -250, 0, 250},
		{ -190, 0, 250},
		{ -130, 0, 250},
		{ -70, 0, 250},
		{ -10, 0, 250},
		{ 50, 0, 250},
		{ 110, 0, 250},
		{ 170, 0, 250},
		{ 250, 0, 250},

	};

	// Spawns a tree for each point in the array
	for (int i = 0; i < NumOfTrees; i++)
	{
		// Creating tree trunk actor
		m_treeTrunkActor = new LJMUGeometryActor();
		m_treeTrunkActor->SetColor(Vector4f(0.65f, 0.16f, 0.16f, 1.0f));
		m_treeTrunkActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, treetrunklength, 1.0f));
		m_treeTrunkActor->GetNode()->Position() = Vector3f(0.0f, treetrunklength * treetrunksize, 0.0f);
		m_treeTrunkActor->GetNode()->Scale() = Vector3f(1, 1, 1) * treetrunksize;
		m_treeTrunkActor->UseLitTexturedMaterial(m_treetrunkTexture);
		this->m_pScene->AddActor(m_treeTrunkActor);

		// Creating tree top actor
		m_treeTopActor = new LJMUGeometryActor();
		m_treeTopActor->SetColor(Vector4f(0.0f, 0.5f, 0.0f, 1.0f));
		m_treeTopActor->DrawCylinder(Vector3f(0, 0, 0), Vector3f(0, 12, 0), 6, 0, 5, 20);
		m_treeTopActor->DrawDisc(Vector3f(0, 0, 0), Vector3f(0, -1, 0), 6);
		m_treeTopActor->GetNode()->Position() = Vector3f(0.0f, treetrunklength, 0.0f);
		m_treeTopActor->UseLitTexturedMaterial(m_leavesTexture);
		this->m_pScene->AddActor(m_treeTopActor);

		// Attaching the tree top to trunk
		Glyph3::Node3D* treeTopNode = m_treeTopActor->GetNode();
		m_treeTrunkActor->GetNode()->AttachChild(treeTopNode);

		// Creating pivot point for tree for realistic tree sway
		m_treeTrunkPivot = new Actor();
		m_treeTrunkPivot->GetNode()->AttachChild(m_treeTrunkActor->GetNode());
		m_treeTrunkPivot->GetNode()->Position() = treePos[i];
		this->m_pScene->AddActor(m_treeTrunkPivot);

		m_treeActor.push_back(m_treeTrunkPivot);
	}

	// Container object
	m_containerTexture = RendererDX11::Get()->LoadTexture(L"container.jpg");
	BasicMeshPtr container_geometry = this->generateOBJMesh(L"container.obj", base_colour);
	MaterialPtr container_material = this->createLitTexturedMaterial();

	setupMaterialProperties(container_material);
	setLights2Material(container_material);

	// First container
	applyTexture2Material(container_material, m_containerTexture);
	m_containerActor = new GeometryActor();
	m_containerActor->GetBody()->SetGeometry(container_geometry);
	m_containerActor->GetBody()->SetMaterial(container_material);
	m_containerActor->GetBody()->Position() = Vector3f(0.0f, 0.0f, -30.0f);
	m_containerActor->GetBody()->Scale() = Vector3f(0.06f, 0.06f, 0.06f);
	this->m_pScene->AddActor(m_containerActor);

	// Second container
	m_containerActor = new GeometryActor();
	m_containerActor->GetBody()->SetGeometry(container_geometry);
	m_containerActor->GetBody()->SetMaterial(container_material);
	m_containerActor->GetBody()->Position() = Vector3f(20.0f, 0.0f, -30.0f);
	m_containerActor->GetBody()->Scale() = Vector3f(0.06f, 0.06f, 0.06f);
	this->m_pScene->AddActor(m_containerActor);

	// Test object
	m_lamppostTexture = RendererDX11::Get()->LoadTexture(L"light.jpg");
	BasicMeshPtr lamppost_geometry = this->generateOBJMesh(L"light.obj", base_colour);
	MaterialPtr lamppost_material = this->createLitTexturedMaterial();
	setupMaterialProperties(lamppost_material);
	setLights2Material(lamppost_material);
	applyTexture2Material(lamppost_material, m_lamppostTexture);
	m_lamppostActor = new GeometryActor();
	m_lamppostActor->GetBody()->SetGeometry(lamppost_geometry);
	m_lamppostActor->GetBody()->SetMaterial(lamppost_material);
	m_lamppostActor->GetBody()->Position() = Vector3f(100.0f, 0.0f, 100.0f);
	m_lamppostActor->GetBody()->Scale() = Vector3f(1.25f, 1.25f, 1.25f);
	this->m_pScene->AddActor(m_lamppostActor);

}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
	// Setup the three light sources parameters
	setupLightSources();

	//Call the Input Assembly Stage to setup the layout of our Engine Objects
	this->inputAssemblyStage();

	this->m_pCamera = new FirstPersonCamera();
	this->m_pCamera->SetEventManager(&this->EvtManager);

	Vector3f tcamerapos(0.0f, 150.0f, -350.0f);
	this->m_pCamera->Spatial().SetTranslation(tcamerapos);
	this->m_pCamera->Spatial().RotateXBy(20 * DEG_TO_RAD);

	this->m_pRenderView = new ViewPerspective(*this->m_pRenderer11,
		                                     this->m_RenderTarget, this->m_DepthTarget);
	this->m_pRenderView->SetBackColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	this->m_pCamera->SetCameraView(this->m_pRenderView);

	this->m_pRender_text = new LJMUTextOverlay(*this->m_pRenderer11, 
		                                      this->m_RenderTarget, 
											  std::wstring(L"Cambria"), 
											  25);	

	this->m_pCamera->SetOverlayView(this->m_pRender_text);

	this->_obj_sprite_cam = new Camera();

	this->_render_sprite = new LJMUSpriteOverlay(*this->m_pRenderer11,
																								this->m_RenderTarget,
																				std::wstring(L"CarSprite.png"));

	this->_obj_sprite_cam->SetOverlayView(this->_render_sprite);

	this->m_pCamera->SetProjectionParams(0.1f, 1000.0f, m_iscreenWidth / m_iscreenHeight,
		                                   static_cast<float>(GLYPH_PI) / 2.0f);
	
	this->m_pScene->AddCamera(this->m_pCamera);

	this->m_pScene->AddCamera(this->_obj_sprite_cam);
}

///////////////////////////////////
// Update the State of our Game and 
// Output the Results to Screen (Render)
/////////////////////////////////// 
void LJMULevelDemo::Update()
{
	this->m_pTimer->Update();
	EvtManager.ProcessEvent(EvtFrameStartPtr( new EvtFrameStart(this->m_pTimer->Elapsed())));

	float tpf = m_pTimer->Elapsed();
	m_totalPlayTime += tpf;

	updateLightSources();
	applyLights2AllMaterials();

	if (tpf > 10.0f / 60.0f)
	{
		// if this is triggered, most likely the user us debugging the code
		// hence time per frame is much larger than 10x 60fps
		tpf = 1 / 60.0f;
	}

	if (m_carState == 0)
	{
		// THE CAR IS TRAVELLING IN A STRAIGHT LINE
		// calculate the car's target vector (i.e the vector from the car's current position and the
		// car's next checkpoint) and calculate distance to next checkpoint.
		// if the car's distance to the next checkpoint is shorter than the previous distance to the next
		// checkpoint that means the car has reached or passed the checkpoint and needs to turn (change
		// m_carState to 1).
		// Otherwise, move the car in the direction of the car's target vector at the specified linear speed

		m_targetCarDirection = m_checkpoints[m_nextCheckpointIndex] - m_carActor->GetNode()->Position();
		float distance2NextCheckpoint = m_targetCarDirection.Magnitude();
		m_targetCarDirection.Normalize();
		m_currentCarDirection = m_targetCarDirection;

		if (distance2NextCheckpoint > m_carDistance2NextCheckpoint)
		{
			m_currentCheckpointIndex++;
			m_nextCheckpointIndex++;
			m_currentCheckpointIndex = m_currentCheckpointIndex % m_checkpoints.size();
			m_nextCheckpointIndex = m_nextCheckpointIndex % m_checkpoints.size();
			m_carActor->GetNode()->Position() = m_checkpoints[m_currentCheckpointIndex];

			m_targetCarDirection = m_checkpoints[m_nextCheckpointIndex] - m_carActor->GetNode()->Position();
			distance2NextCheckpoint = m_targetCarDirection.Magnitude();
			m_targetCarDirection.Normalize();
			
			float angle = acos(m_currentCarDirection.Dot(m_targetCarDirection));
			if (angle > m_carAngularSpeed * tpf)
			{
				m_carState = 1;
			}
		}
		else
		{
			m_carActor->GetNode()->Position() =   m_carActor->GetNode()->Position() + 
																				m_currentCarDirection * m_carLinearSpeed * tpf;
			m_carDistance2NextCheckpoint = distance2NextCheckpoint;
		}
	}
	else
	{
		// THE CAR IS TURNING AROUND
		// Calculate the angle difference between the car's current direction and the car's target direction
		// using the vectors dot product.
		// If the angle is less than the increment angle, which is m_carAngularSpeed * tpf, it means the car
		// is roughly facing the target location. Snap the car's orientation to the target vector and make
		// the car moving forward in the next frame (change m_carState to 0)
		// Otherwise, keep turning the car in the direction of the car's target vector at the specified angular speed

		float angle = acos(m_currentCarDirection.Dot(m_targetCarDirection));
		if (angle < m_carAngularSpeed * tpf)
		{
			m_currentCarDirection = m_targetCarDirection;
			float angle = acos(m_referenceCarDirection.Dot(m_targetCarDirection));

			Vector3f axis = m_referenceCarDirection.Cross(m_currentCarDirection);
			axis.Normalize();

			Matrix3f tstartRotation;
			tstartRotation.RotationEuler(axis, angle);
			m_carActor->GetNode()->Rotation() = tstartRotation;

			m_targetCarDirection = m_checkpoints[m_nextCheckpointIndex] - m_carActor->GetNode()->Position();
			m_carDistance2NextCheckpoint = m_targetCarDirection.Magnitude();

			m_carState = 0;
		}
		else
		{
			m_carActor->GetNode()->Position() = m_carActor->GetNode()->Position() +
				m_currentCarDirection * m_carLinearSpeed * tpf;

			angle = angle + m_carAngularSpeed * tpf;
			Vector3f axis = m_currentCarDirection.Cross(m_targetCarDirection);
			axis.Normalize();

			Matrix3f tstartRotation;
			tstartRotation.RotationEuler(axis, m_carAngularSpeed * tpf);
			m_carActor->GetNode()->Rotation() *= tstartRotation;
			m_currentCarDirection = m_carActor->GetNode()->Rotation() * m_referenceCarDirection;
			m_currentCarDirection.Normalize();
		}
	}

	// Tree sway code
	for (size_t i = 0; i < m_treeActor.size(); i++)
	{
		m_totalPlayTime += tpf;
		Matrix3f trotation;
		float rotationSpeed = 0.25;
		float maxswayangle = 7.5 / 360.0 * GLYPH_PI; // 10 degrees

		// calculate sway angle and applying rotation
		trotation.RotationZ(cos(m_totalPlayTime * rotationSpeed) * maxswayangle);
		m_treeActor[i]->GetNode()->Rotation() = trotation;
	}

	// Updating text above car
	m_carLabelText->GetNode()->Position() = m_carActor->GetNode()->Position() + Vector3f(-5, 20, 0);

	//-----------------------2D Text Rendering------------------------------------------------
	Matrix4f  ttextpos = Matrix4f::Identity();
	float tx = 300.0f; float ty = 1030.0f;
	static Vector4f twhiteclr(1.0f, 1.0f, 1.0f, 1.0f);
	static Vector4f tredclr(1.0f, 0.0f, 0.0f, 1.0f);

	ttextpos.SetTranslation(Vector3f(tx, ty - 66, 0.0f));
	this->m_pRender_text->writeText(this->outputFPSInfo(), ttextpos, twhiteclr);

	ttextpos.SetTranslation(Vector3f(tx, ty -39, 0.0f));
	this->m_pRender_text->writeText(this->outputCarSpeedInfo(), ttextpos, twhiteclr);

	ttextpos.SetTranslation(Vector3f(tx, ty -12, 0.0f));
	this->m_pRender_text->writeText(this->outputCarPosInfo(), ttextpos, twhiteclr);

	ttextpos.SetTranslation(Vector3f(tx, ty + 15, 0.0f));
	this->m_pRender_text->writeText(this->outputCarRotInfo(), ttextpos, twhiteclr);

	//-----------------------2D Sprite Rendering------------------------------------------------
	if(this->_sprite_visible)
	{
		// Rendering the sprite if sprite is visible
		Matrix4f tspritexform = Matrix4f::Identity();
		tspritexform.SetTranslation(Vector3f(25, 985, 0));
		this->_render_sprite->drawSprite(tspritexform, Vector4f(1, 1, 1, 1));
	}

	// Day night cycle code
	{
		float cycleDuration = 600.0f; // Total time for a complete cycle (day -> night -> day)
		float progress = fmod(m_totalPlayTime, cycleDuration);

		if (progress <= 120.0f)
		{
			// Day phase
			DirectionalLightColour = Vector4f(0.07f, 0.07f, 0.07f, 1.0f);

		}
		else if (progress <= 240.0f)
		{
			// Transition to night
			float normalizedProgress = (progress - 120.0f) / 120.0f;
			DirectionalLightColour = Vector4f(
				0.07f - normalizedProgress * (0.07f - 0.002f),
				0.07f - normalizedProgress * (0.07f - 0.002f),
				0.07f - normalizedProgress * (0.07f - 0.002f),
				1.0f);


		}
		else if (progress <= 360.0f)
		{
			// Night phase
			DirectionalLightColour = Vector4f(0.001f, 0.001f, 0.001f, 1.0f);

		}
		else if (progress <= 480.0f)
		{
			// Transition to day
			float normalizedProgress = (progress - 360.0f) / 120.0f;
			DirectionalLightColour = Vector4f(
				0.002f + normalizedProgress * (0.07f - 0.002f),
				0.002f + normalizedProgress * (0.07f - 0.002f),
				0.002f + normalizedProgress * (0.07f - 0.002f),
				1.0f);

		}
		else
		{
			// Day phase
			DirectionalLightColour = Vector4f(0.07f, 0.07f, 0.07f, 1.0f);
		}
	}

	//----------START RENDERING--------------------------------------------------------------
	this->m_pScene->Update(m_pTimer->Elapsed());
	this->m_pScene->Render(this->m_pRenderer11);

	//--------END RENDERING-------------------------------------------------------------
	this->m_pRenderer11->Present(this->m_pWindow->GetHandle(), this->m_pWindow->GetSwapChain());
}

///////////////////////////////////
// Configure the DirectX 11 Programmable
// Pipeline Stages and Create the Window
// Calls 
///////////////////////////////////
bool LJMULevelDemo::ConfigureEngineComponents()
{

	// Set the render window parameters and initialize the window
	this->m_pWindow = new Win32RenderWindow();
	this->m_pWindow->SetPosition(25, 25);
	this->m_pWindow->SetSize(m_iscreenWidth, m_iscreenHeight);
	this->m_pWindow->SetCaption(this->GetName());
	this->m_pWindow->Initialize(this);


	// Create the renderer and initialize it for the desired device
	// type and feature level.
	this->m_pRenderer11 = new RendererDX11();

	if (!this->m_pRenderer11->Initialize(D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0))
	{
		Log::Get().Write(L"Could not create hardware device, trying to create the reference device...");

		if (!this->m_pRenderer11->Initialize(D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_10_0))
		{
			ShowWindow(this->m_pWindow->GetHandle(), SW_HIDE);
			MessageBox(this->m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device!", 
						L"5108COMP Coursework Template", MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
			this->RequestTermination();
			return(false);
		}
		// If using the reference device, utilize a fixed time step for any animations.
		this->m_pTimer->SetFixedTimeStep(1.0f / 10.0f);
	}

	// Create a swap chain for the window that we started out with.  This
	// demonstrates using a configuration object for fast and concise object
	// creation.
	SwapChainConfigDX11 tconfig;
	tconfig.SetWidth(this->m_pWindow->GetWidth());
	tconfig.SetHeight(this->m_pWindow->GetHeight());
	tconfig.SetOutputWindow(this->m_pWindow->GetHandle());
	this->m_iSwapChain = this->m_pRenderer11->CreateSwapChain(&tconfig);
	this->m_pWindow->SetSwapChain(this->m_iSwapChain);
	
	//Create Colour and Depth Buffers
	this->m_RenderTarget = this->m_pRenderer11->GetSwapChainResource(this->m_iSwapChain);

	Texture2dConfigDX11 tdepthconfig;
	tdepthconfig.SetDepthBuffer(m_iscreenWidth, m_iscreenHeight);
	this->m_DepthTarget = this->m_pRenderer11->CreateTexture2D(&tdepthconfig, 0);

	// Bind the swap chain render target and the depth buffer for use in rendering.  
	this->m_pRenderer11->pImmPipeline->ClearRenderTargets();
	this->m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, this->m_RenderTarget->m_iResourceRTV);
	this->m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.DepthTargetViews.SetState(this->m_DepthTarget->m_iResourceDSV);
	this->m_pRenderer11->pImmPipeline->ApplyRenderTargets();

	D3D11_VIEWPORT tviewport;
	tviewport.Width = static_cast< float >(m_iscreenWidth);
	tviewport.Height = static_cast< float >(m_iscreenHeight);
	tviewport.MinDepth = 0.0f;
	tviewport.MaxDepth = 1.0f;
	tviewport.TopLeftX = 0;
	tviewport.TopLeftY = 0;

	int tvpindex = this->m_pRenderer11->CreateViewPort(tviewport);
	this->m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.ViewportCount.SetState(1);
	this->m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.Viewports.SetState(0, tvpindex);
	return(true);
}

//////////////////////////////////
//Handle Input Events in the Application
//////////////////////////////////
bool LJMULevelDemo::HandleEvent(EventPtr pevent)
{
	eEVENT e = pevent->GetEventType();

	if (e == SYSTEM_KEYBOARD_KEYDOWN)
	{
		EvtKeyDownPtr tkey_down = std::static_pointer_cast<EvtKeyDown>(pevent);
		unsigned int  tkeycode = tkey_down->GetCharacterCode();

		if (tkeycode == '2')
			this->_curr_obj++;
		if (this->_curr_obj >= this->_objects.size())
			this->_curr_obj = 0;

		if (tkeycode == '3')
			this->_sprite_visible = !this->_sprite_visible;
	}
	else if (e == SYSTEM_KEYBOARD_KEYUP)
	{
		EvtKeyUpPtr tkey_up = std::static_pointer_cast<EvtKeyUp>(pevent);
		unsigned int tkeycode = tkey_up->GetCharacterCode();
	}

	return(Application::HandleEvent(pevent));
}

//////////////////////////////////
// Destroy Resources created by the engine
//////////////////////////////////
void LJMULevelDemo::ShutdownEngineComponents()
{
	if (this->m_pRenderer11)
	{
		this->m_pRenderer11->Shutdown();
		delete this->m_pRenderer11;
	}

	if (this->m_pWindow)
	{
		this->m_pWindow->Shutdown();
		delete this->m_pWindow;
	}
}

//////////////////////////////////
// Shutdown the Application
//////////////////////////////////
void LJMULevelDemo::Shutdown()
{
	//NOTHING TO DO HERE
}

//////////////////////////////////
// Take a Screenshot of the Application
//////////////////////////////////
void LJMULevelDemo::TakeScreenShot()
{
	if (this->m_bSaveScreenshot)
	{
		this->m_bSaveScreenshot = false;
		this->m_pRenderer11->pImmPipeline->SaveTextureScreenShot(0, this->GetName());
	}
}

//////////////////////////////////////
// Output our Frame Rate
//////////////////////////////////////
std::wstring LJMULevelDemo::outputFPSInfo()
{
	std::wstringstream out;
	out << L"FPS: " << m_pTimer->Framerate();
	return out.str();
}

void LJMULevelDemo::setupLightSources()
{
	// Directional Light properties
	Vector3f directionalLightDir = Vector3f(0.0f, -0.5f, 0.5f);
	directionalLightDir.Normalize();
	DirectionalLightDirection = Vector4f(directionalLightDir, 1.0f);
	DirectionalLightColour = Vector4f(0.001f, 0.001f, 0.001f, 1.0f);

	// Point Light properties
	PointLightPosition = Vector4f(-140.0f, 30.0f, -155.0f, 1.0f);
	PointLightColour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	PointLightRange = Vector4f(90.0f, 0.0f, 0.0f, 0.0f);
	// The range of the point light source is set to 30.0
	// We only use the first component of the 4D vector because
	// we cannot just set a scalar number to the GPU

	// Spotlight properties
	// Spotlight 1
	Vector3f spotLightDir = Vector3f(2.0f, -1.0f, 2.0f);
	spotLightDir.Normalize();
	SpotLightDirection = Vector4f(spotLightDir, 1.0f);
	SpotLightColour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	SpotLightPosition = Vector4f(100.0f, 10.0f, 100.0f, 1.0f);
	SpotLightRange = Vector4f(150.0f, 0.0f, 0.0f, 0.0f);
	SpotLightFocus = Vector4f(20.0f, 0.0f, 0.0f, 0.0f);
	// The focus of the spotlight source is set to 10.0
	// We only use the first component of the 4F vector because
	// we cannot just send a scalar number to the GPU

	// Spotlight 2
	Vector3f spotLight2Dir = Vector3f(2.0f, -1.0f, 2.0f);
	spotLight2Dir.Normalize();
	SpotLight2Direction = Vector4f(spotLight2Dir, 1.0f);
	SpotLight2Colour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	SpotLight2Position = Vector4f(100.0f, 10.0f, 100.0f, 1.0f);
	SpotLight2Range = Vector4f(150.0f, 0.0f, 0.0f, 0.0f);
	SpotLight2Focus = Vector4f(20.0f, 0.0f, 0.0f, 0.0f);
}

void LJMULevelDemo::setLights2Material(MaterialPtr material)
{
	material->Parameters.SetVectorParameter(L"DirectionalLightColour", DirectionalLightColour);
	material->Parameters.SetVectorParameter(L"DirectionalLightDirection", DirectionalLightDirection);

	material->Parameters.SetVectorParameter(L"PointLightColour", PointLightColour);
	material->Parameters.SetVectorParameter(L"PointLightPosition", PointLightPosition);
	material->Parameters.SetVectorParameter(L"PointLightRange", PointLightRange);

	material->Parameters.SetVectorParameter(L"SpotLight1Colour", SpotLightColour);
	material->Parameters.SetVectorParameter(L"SpotLight1Direction", SpotLightDirection);
	material->Parameters.SetVectorParameter(L"SpotLight1Position", SpotLightPosition);
	material->Parameters.SetVectorParameter(L"SpotLight1Range", SpotLightRange);
	material->Parameters.SetVectorParameter(L"SpotLight1Focus", SpotLightFocus);

	material->Parameters.SetVectorParameter(L"SpotLight2Colour", SpotLight2Colour);
	material->Parameters.SetVectorParameter(L"SpotLight2Direction", SpotLight2Direction);
	material->Parameters.SetVectorParameter(L"SpotLight2Position", SpotLight2Position);
	material->Parameters.SetVectorParameter(L"SpotLight2Range", SpotLight2Range);
	material->Parameters.SetVectorParameter(L"SpotLight2Focus", SpotLight2Focus);
}


void LJMULevelDemo::updateLightSources()
{
	Vector3f carPosition = m_carActor->GetNode()->Position();
	Matrix3f carRotation = m_carActor->GetNode()->Rotation();
	m_currentCarDirection = carRotation * m_referenceCarDirection;
	m_currentCarDirection.Normalize();

	Vector3f adjustedDirection = m_currentCarDirection + Vector3f(0.1f, -0.1f, 0.0f);
	adjustedDirection.Normalize();
	SpotLightDirection = Vector4f(adjustedDirection, 1.0f);

	// Updating the spotlight position to emulate the car's headlights
	SpotLightPosition = Vector4f(carPosition.x - 1.0f, carPosition.y + 1.0f, carPosition.z, 1.0f);
}

void LJMULevelDemo::applyLights2AllMaterials()
{
	MaterialPtr material = m_carActor->GetBody()->GetMaterial();
	setLights2Material(material);
	m_carActor->GetBody()->SetMaterial(material);

	MaterialPtr material2 = m_platformActor->GetBody()->GetMaterial();
	setLights2Material(material2);
	m_platformActor->GetBody()->SetMaterial(material2);

	MaterialPtr material3 = m_containerActor->GetBody()->GetMaterial();
	setLights2Material(material3);
	m_containerActor->GetBody()->SetMaterial(material3);

	MaterialPtr material4 = m_containerActor->GetBody()->GetMaterial();
	setLights2Material(material4);
	m_treeTopActor->GetBody()->SetMaterial(material4);
}

MaterialPtr LJMULevelDemo::setupMaterialProperties(MaterialPtr material)
{
	float ambient_constant = 0.001f;
	float diffuse_constant = 4.0f;
	float specular_constant = 0.1f;
	float shininess_constant = 200.f;
	Vector4f emmisivity = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	material->Parameters.SetVectorParameter(L"SurfaceConstants", Vector4f(
		ambient_constant,
		diffuse_constant,
		specular_constant,
		shininess_constant));

	material->Parameters.SetVectorParameter(L"SurfaceEmmisiveColour", emmisivity);
	setLights2Material(material);
	return material;
}

void LJMULevelDemo::applyTexture2Material(MaterialPtr material, ResourcePtr texture)
{
	material->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
}

BasicMeshPtr LJMULevelDemo::generateOBJMesh(std::wstring pmeshname, Vector4f pmeshColour)
{
	FileSystem fs;
	LJMUMeshOBJ* tmesh = new LJMUMeshOBJ(fs.GetModelsFolder() + pmeshname);
	int tvertcount = tmesh->positions.size();

	auto tia = std::make_shared < DrawExecutorDX11 < BasicVertexDX11::Vertex>>();
	tia->SetLayoutElements(BasicVertexDX11::GetElementCount(), BasicVertexDX11::Elements);
	tia->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	tia->SetMaxVertexCount(tvertcount);

	BasicVertexDX11::Vertex tv;
	tv.color = pmeshColour;

	for (auto& tobject : tmesh->objects)
	{
		for (auto& tface : tobject.faces)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				tv.position = tmesh->positions[tface.PositionIndices[i]];
				tv.normal = tmesh->normals[tface.NormalIndices[i]];
				tv.texcoords = tmesh->coords[tface.CoordIndices[i]];
				tia->AddVertex(tv);
			}
		}
	}
	return tia;
}

MaterialPtr LJMULevelDemo::createLitTexturedMaterial() {

	RendererDX11* pRenderer = RendererDX11::Get();
	MaterialPtr material = MaterialPtr(new MaterialDX11());

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader(pRenderer->LoadShader(VERTEX_SHADER,
		std::wstring(L"LJMULitTexture.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_4_0")));

	pEffect->SetPixelShader(pRenderer->LoadShader(PIXEL_SHADER,
		std::wstring(L"LJMULitTexture.hlsl"),
		std::wstring(L"PSMAIN"),
		std::wstring(L"ps_4_0")));

	RasterizerStateConfigDX11 rsConfig;
	rsConfig.CullMode = D3D11_CULL_BACK;
	rsConfig.FillMode = D3D11_FILL_SOLID;

	int rasterizerState = m_pRenderer11->CreateRasterizerState(&rsConfig);
	if (rasterizerState == -1)
	{
		Log::Get().Write(L"Failed to create light rasterizer state");
		assert(false);
	}

	pEffect->m_iRasterizerState = rasterizerState;

	SamplerStateConfigDX11 SamplerConfig;
	SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	SamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerConfig.MaxAnisotropy = 16;

	int TextureSampler = RendererDX11::Get()->CreateSamplerState(&SamplerConfig);
	material->Parameters.SetSamplerParameter(L"TextureSampler", TextureSampler);

	// Enable the material to render the given view type, and set its effect
	material->Params[VT_PERSPECTIVE].bRender = true;
	material->Params[VT_PERSPECTIVE].pEffect = pEffect;

	return material;
}

std::wstring LJMULevelDemo::outputCarSpeedInfo()
{
	std::wstringstream out;
	out << L"Car Speed: " << m_carLinearSpeed << "MPH";
	return out.str();
}

std::wstring LJMULevelDemo::outputCarPosInfo()
{
	std::wstringstream out;
	// Getting the cars coords and storing them in a vector
	Vector3f carPosition = m_carActor->GetNode()->Position();

	// Converting the Vector values to printable integers on screen
	out << L"Car Position: ("
		<< static_cast<int>(carPosition.x) << L", "
		<< static_cast<int>(carPosition.y) << L", "
		<< static_cast<int>(carPosition.z) << L")";

	return out.str();
}

std::wstring LJMULevelDemo::outputCarRotInfo()
{
	std::wstringstream out;
	// Getting the cars rotation value and storing them in a matrix
	Matrix3f carRotation = m_carActor->GetNode()->Rotation();

	out << std::fixed << std::setprecision(2); // Setting floating point precision to 2 decimal places

	// Printing just 2 of the 9 values from the matrix to represent the cars rotation value
	out << L"Car Orientation: ("
		  << carRotation(0, 0) << L", "  // Top left value in matrix
		  << carRotation(0, 2) << L")";  // Top right value in matrix

	return out.str();
}
