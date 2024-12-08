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


//Add a Using Directive to avoid typing Glyph3 for basic constructs
using namespace Glyph3;
//Include our own application Namespace
using namespace LJMUDX;

LJMULevelDemo AppInstance; 

//---------CONSTRUCTORS-------------------------------------------------------

///////////////////////////////////////
//
///////////////////////////////////////
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
		pathSegmentActor->DrawCylinder(cylinderStartPoint, cylinderEndPoint, 0.4f, 0.4f);
		pathSegmentActor->UseSolidMaterial();
		pathSegmentActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
		this->m_pScene->AddActor(pathSegmentActor);

		float carTextSize = 10.0f;
		m_carLabelText = new TextActor();
		m_carLabelText->SetColor(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		m_carLabelText->SetText(L"P1");
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

	m_carLinearSpeed = 40.0f;
	m_carAngularSpeed = 1.5f;

	// m_carState = 0 : Car is moving forward
	// m_carState = 1 : Car is turning around
	m_carState = 0;

	float treetrunklength = 8.0f;
	float treetrunksize = 1.5f;

	/*m_treeTrunkActor = new GeometryActor();
	m_treeTrunkActor->UseTexturedMaterial(m_treetrunkTexture);
	m_treeTrunkActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, treetrunklength, 1.0f));
	m_treeTrunkActor->GetNode()->Position() = Vector3f(0.0f, treetrunklength * treetrunksize, 0.0f);
	m_treeTrunkActor->GetNode()->Scale() = Vector3f(1, 1, 1) * treetrunksize;
	this->m_pScene->AddActor(m_treeTrunkActor);

	m_treeTopActor = new GeometryActor();
	m_treeTopActor->UseTexturedMaterial(m_leavesTexture);
	m_treeTopActor->DrawCylinder(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0, 12,0), 6, 0, 5, 20);
	m_treeTopActor->DrawDisc(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0, -1, 0), 6);
	m_treeTopActor->GetNode()->Position() = Vector3f(0.0f, treetrunklength, 0.0f);
	this->m_pScene->AddActor(m_treeTopActor);

	Glyph3::Node3D* treeTopNode = m_treeTopActor->GetNode();
	m_treeTrunkActor->GetNode()->AttachChild(treeTopNode);*/

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

	// Array of tree coordinates
	const int NumOfTrees = 15;
	Vector3f treePos[NumOfTrees] = {
		{ -24.80f, 0.0f, -56.32f },
		{ 72.00f, 0.0f, -38.56f },
		{ 94.72f, 0.0f, 30.08f },
		{ 139.36f, 0.0f, 78.08f },
		{ 239.52f, 0.0f, 102.08f },
		{ 32.32f, 0.0f, 61.28f },
		{ 96.64f, 0.0f, -98.56f },
		{ 206.08f, 0.0f, -38.56f },
		{ 244.96f, 0.0f, 19.52f },
		{ -65.12f, 0.0f, -173.44f },
		{ -190.88f, 0.0f, -217.12f },
		{ -227.36f, 0.0f, -148.96f },
		{ -208.64f, 0.0f, -6.40f },
		{ -142.40f, 0.0f, 80.00f },
		{ -48.80f, 0.0f, 133.76f }
	};

	// Loop through each tree coordinate
	for (int i = 0; i < NumOfTrees; i++)
	{

		// Creating tree trunk actor
		m_treeTrunkActor = new LJMUGeometryActor();
		m_treeTrunkActor->SetColor(Vector4f(0.65f, 0.16f, 0.16f, 1.0f));
		m_treeTrunkActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, treetrunklength, 1.0f));
		m_treeTrunkActor->GetNode()->Position() = treePos[i] + Vector3f(0.0f, treetrunklength * treetrunksize, 0.0f);
		m_treeTrunkActor->GetNode()->Scale() = Vector3f(1, 1, 1) * treetrunksize;
		m_treeTrunkActor->UseTexturedMaterial(m_treetrunkTexture);
		this->m_pScene->AddActor(m_treeTrunkActor);

		// Creating tree top actor
		m_treeTopActor = new LJMUGeometryActor();
		m_treeTopActor->SetColor(Vector4f(0.0f, 0.5f, 0.0f, 1.0f));
		m_treeTopActor->DrawCylinder(Vector3f(0, 0, 0), Vector3f(0, 12, 0), 6, 0, 5, 20);
		m_treeTopActor->DrawDisc(Vector3f(0, 0, 0), Vector3f(0, -1, 0), 6);
		m_treeTopActor->GetNode()->Position() = Vector3f(0.0f, treetrunklength, 0.0f);
		m_treeTopActor->UseTexturedMaterial(m_leavesTexture);
		this->m_pScene->AddActor(m_treeTopActor);

		// Attaching the tree top to trunk
		Glyph3::Node3D* treeTopNode = m_treeTopActor->GetNode();
		m_treeTrunkActor->GetNode()->AttachChild(treeTopNode);

		Matrix3f trotation;
		trotation.RotationZ(m_totalPlayTime);
		m_treeTrunkActor->GetNode()->Rotation() *= trotation;
	}

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
	

	this->m_pCamera->SetProjectionParams(0.1f, 1000.0f, m_iscreenWidth / m_iscreenHeight,
		                                   static_cast<float>(GLYPH_PI) / 2.0f);
	
	this->m_pScene->AddCamera(this->m_pCamera);	
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

	if (tpf > 10.0f / 60.0f)
	{
		// if this is triggered, most likely the user us debugging the code
		// hence time per frame is much larger than 10x 60fps
		tpf = 1 / 60.0f;
	}

	m_totalPlayTime += tpf;

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

	// Updating text above car
	m_carLabelText->GetNode()->Position() = m_carActor->GetNode()->Position() + Vector3f(-5, 20, 0);

	// Rotating   t r e e
	/*Matrix3f trotation;
	trotation.RotationZ(tpf);
	m_treeTrunkActor->GetNode()->Rotation() *= trotation;*/

	updateLightSources();
	applyLights2AllMaterials();

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

	/*std::vector<Spotlight> Spotlights;*/

	// Directional Light properties
	Vector3f directionalLightDir = Vector3f(0.0f, -0.5f, 0.5f);
	directionalLightDir.Normalize();
	DirectionalLightDirection = Vector4f(directionalLightDir, 1.0f);
	DirectionalLightColour = Vector4f(0.002f, 0.002f, 0.002f, 1.0f);

	// Point Light properties
	PointLightPosition = Vector4f(0.0f, 30.0f, 0.0f, 1.0f);
	PointLightColour - Vector4f(1.0f, 0.1f, 0.1f, 1.0f);
	PointLightRange - Vector4f(60.0f, 0.0f, 0.0f, 0.0f);
	// The range of the point light source is set to 30.0
	// We only use the first component of the 4D vector because
	// we cannot just set a scalar number to the GPU

	// Spot Light properties
	Spotlight spotlight1;
	Vector3f spotLightDir = Vector3f(2.0f, -1.0f, 2.0f);
	spotLightDir.Normalize();
	SpotLightDirection = Vector4f(spotLightDir, 1.0f);
	SpotLightColour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	SpotLightPosition = Vector4f(100.0f, 10.0f, 100.0f, 1.0f);
	SpotLightRange = Vector4f(150.0f, 0.0f, 0.0f, 0.0f);
	SpotLightFocus = Vector4f(20.0f, 0.0f, 0.0f, 0.0f);
	// The focus of the spot light source is set to 10.0
	// We only use the first component of the 4F vector because
	// we cannot just send a scalar number to the GPU

	Spotlight spotlight2;
	Vector3f spotLightDir = Vector3f(2.0f, -1.0f, 2.0f);
	spotLightDir.Normalize();
	SpotLightDirection = Vector4f(spotLightDir, 1.0f);
	SpotLightColour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	SpotLightPosition = Vector4f(-100.0f, 10.0f, -100.0f, 1.0f);
	SpotLightRange = Vector4f(150.0f, 0.0f, 0.0f, 0.0f);
	SpotLightFocus = Vector4f(20.0f, 0.0f, 0.0f, 0.0f);

	// Add to collection
	Spotlights.push_back(spotlight1);
	Spotlights.push_back(spotlight2);

}

void LJMULevelDemo::setLights2Material(MaterialPtr material)
{
	material->Parameters.SetVectorParameter(L"DirectionalLightColour", DirectionalLightColour);
	material->Parameters.SetVectorParameter(L"DirectionalLightDirection", DirectionalLightDirection);

	material->Parameters.SetVectorParameter(L"PointLightColour", PointLightColour);
	material->Parameters.SetVectorParameter(L"PointLightPosition", PointLightPosition);
	material->Parameters.SetVectorParameter(L"PointLightRange", PointLightRange);

	for (size_t i = 0; i < Spotlights.size(); ++i) {
		std::wstring prefix = L"SpotLight" + std::to_wstring(i + 1);
		material->Parameters.SetVectorParameter(L"SpotLightColour", SpotLightColour);
		material->Parameters.SetVectorParameter(L"SpotLightDirection", SpotLightDirection);
		material->Parameters.SetVectorParameter(L"SpotLightPosition", SpotLightPosition);
		material->Parameters.SetVectorParameter(L"SpotLightRange", SpotLightRange);
		material->Parameters.SetVectorParameter(L"SpotLightFocus", SpotLightFocus);
	}
}


void LJMULevelDemo::updateLightSources()
{

	// Spot Light properties
	float spotlightRotationSpeed = 0.5f;
	float xdir = sin(m_totalPlayTime * spotlightRotationSpeed);
	float ydir = -0.1f;
	float zdir = cos(m_totalPlayTime * spotlightRotationSpeed);

	Vector3f spotLightDir = Vector3f(xdir, ydir, zdir);
	spotLightDir.Normalize();
	SpotLightDirection = Vector4f(spotLightDir, 1.0f);

}

void LJMULevelDemo::applyLights2AllMaterials()
{
	MaterialPtr material = m_carActor->GetBody()->GetMaterial();
	setLights2Material(material);
	m_carActor->GetBody()->SetMaterial(material);
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


