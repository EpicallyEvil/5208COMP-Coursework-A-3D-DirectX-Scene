//------------Include the Application Header File----------------------------
#include "LJMULevelDemo.h"

//Test 2
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
	return(std::wstring(L"5108COMP: Coursework Template"));
}

/////////////////////////////////////
// Assemble our Input Layouts for this
// Stage of the Pipeline.
/////////////////////////////////////
void LJMULevelDemo::inputAssemblyStage()
{			
	//-----SETUP OUR GEOMETRY FOR THIS SCENE-----------------------------------------
	m_platformActor = new GeometryActor();
	m_platformActor->SetColor(Vector4f(0.6f, 0.3f, 0.1f, 1.0f));
	m_platformActor->DrawRect( Vector3f(0.0f, 0.0f, 0.0f),
														Vector3f(1.0f, 0.0f, 0.0f),
														Vector3f(0.0f, 0.0f, -1.0f),
														Vector2f(m_platformWidth, m_platformLength));
	m_platformActor->UseSolidMaterial();
	m_platformActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);

	this->m_pScene->AddActor(m_platformActor);

	PointLight* tlight = new PointLight();
	tlight->GetSpecular() = Vector4f(1.2f, 1.2f, 1.2, 1);
	tlight->GetDiffuse() = Vector4f(1.2f, 1.2f, 1.2, 1);
	tlight->GetAmbient() = Vector4f(1.2f, 1.2f, 1.2, 1);
	tlight->GetNode()->Position() = Vector3f(-128.0f, 128.0f, -128.0f);
	m_pScene->AddLight(tlight);

	m_carActor = new GeometryActor();
	m_carActor->SetColor(Vector4f(0.0f, 1.0f, 0.0f, 1.0f)); //Green
	m_carActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(4.0f, 8.0f, 12.0f));
	m_carActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);

	this->m_pScene->AddActor(m_carActor);

	for (int i = -180; i < 180; i = i + 45)
	{
		float x = 200.0f * cos(i * DEG_TO_RAD);
		float z = 200.0f * sin(i * DEG_TO_RAD);
		m_checkpoints.push_back(Vector3f(x, 0, z));
	}

	for (int i = 0; i < m_checkpoints.size(); i++)
	{
		GeometryActor* pathSegmentActor = new GeometryActor();
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

	m_carLinearSpeed = 80.0f;
	m_carAngularSpeed = 1.5f;

	// m_carState = 0 : Car is moving forward
	// m_carState = 1 : Car is turning around
	m_carState = 0;

	float treetrunklength = 8.0f;
	float treetrunksize = 1.5f;

	m_treeTrunkActor = new GeometryActor();
	m_treeTrunkActor->SetColor(Vector4f(0.65f, 0.16f, 0.16f, 1.0f));
	m_treeTrunkActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, treetrunklength, 1.0f));
	m_treeTrunkActor->GetNode()->Position() = Vector3f(0.0f, treetrunklength * treetrunksize, 0.0f);
	m_treeTrunkActor->GetNode()->Scale() = Vector3f(1, 1, 1) * treetrunksize;
	this->m_pScene->AddActor(m_treeTrunkActor);

	m_treeTopActor = new GeometryActor();
	m_treeTopActor->SetColor(Vector4f(0.0f, 0.5f, 0.0f, 1.0f));
	m_treeTopActor->DrawCylinder(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0, 12,0), 6, 0, 5, 20);
	m_treeTopActor->DrawDisc(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0, -1, 0), 6);
	m_treeTopActor->GetNode()->Position() = Vector3f(0.0f, treetrunklength, 0.0f);
	this->m_pScene->AddActor(m_treeTopActor);

	Glyph3::Node3D* treeTopNode = m_treeTopActor->GetNode();
	m_treeTrunkActor->GetNode()->AttachChild(treeTopNode);

}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
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
	Matrix3f trotation;
	trotation.RotationZ(tpf);
	m_treeTrunkActor->GetNode()->Rotation() *= trotation;

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