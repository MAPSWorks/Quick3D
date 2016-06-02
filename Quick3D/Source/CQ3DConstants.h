
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------
// Noms de classe des composants

#define ClassName_CComponent				"CComponent"
#define ClassName_CPhysicalComponent		"CPhysicalComponent"
#define ClassName_CBoundingBox				"CBoundingBox"
#define ClassName_CCamera					"CCamera"

#define ClassName_CLight					"CLight"
#define ClassName_CFog						"CFog"

#define ClassName_CVertex					"CVertex"
#define ClassName_CFace						"CFace"
#define ClassName_CMesh						"CMesh"
#define ClassName_CMeshInstance				"CMeshInstance"
#define ClassName_CBoundedMeshInstances		"CBoundedMeshInstances"

#define ClassName_CWater					"CWater"
#define ClassName_CParticle					"CParticle"
#define ClassName_CParticleSystem			"CParticleSystem"
#define ClassName_CRain						"CRain"
#define ClassName_CBox						"CBox"
#define ClassName_CCone						"CCone"
#define ClassName_CSkyBox					"CSkyBox"

#define ClassName_CAutoTerrain				"CAutoTerrain"
#define ClassName_CTerrain					"CTerrain"
#define ClassName_CWorldChunk				"CWorldChunk"

#define ClassName_CArmature					"CArmature"

#define ClassName_CMaterial					"CMaterial"
#define ClassName_CMaterialInstance			"CMaterialInstance"
#define ClassName_CTiledMaterial			"CTiledMaterial"
#define ClassName_CWaterMaterial			"CWaterMaterial"

#define ClassName_CRenderContext			"CRenderContext"

#define ClassName_CController				"CController"
#define ClassName_CStandardController		"CStandardController"
#define ClassName_CForceController			"CForceController"
#define ClassName_CCartoController			"CCartoController"
#define ClassName_CTankController			"CTankController"
#define ClassName_CAircraftController		"CAircraftController"

#define ClassName_CAnimator					"CAnimator"
#define ClassName_CBasicAnimator			"CBasicAnimator"

#define ClassName_CMan						"CMan"

#define ClassName_CSeaVehicle				"CSeaVehicle"
#define ClassName_CTerrestrialVehicle		"CTerrestrialVehicle"
#define ClassName_CVehicle					"CVehicle"

#define ClassName_CAircraft					"CAircraft"
#define ClassName_CWing						"CWing"
#define ClassName_CElevator					"CElevator"
#define ClassName_CRudder					"CRudder"
#define ClassName_CEngine					"CEngine"
#define ClassName_CJetEngine				"CJetEngine"

#define ClassName_CTrajectorable			"CTrajectorable"
#define ClassName_CTrajectory				"CTrajectory"
#define ClassName_CWaypoint					"CWaypoint"

#define ClassName_CElectricalComponent		"CElectricalComponent"
#define ClassName_CElectricalBus			"CElectricalBus"
#define ClassName_CElectricalConsumer		"CElectricalConsumer"
#define ClassName_CElectricalContactor		"CElectricalContactor"

#define ClassName_CHydraulicComponent		"CHydraulicComponent"

//-------------------------------------------------------------------------------------------------
// Param�tres communs

#define ParamName_Acceleration				"Acceleration"
#define ParamName_AccelerationFactor		"AccelerationFactor"
#define ParamName_Active					"Active"
#define ParamName_AileronPosition			"AileronPosition"
#define ParamName_Altitude					"Altitude"
#define ParamName_Ambient					"Ambient"
#define ParamName_CenterOfMass				"CenterOfMass"
#define ParamName_Class						"Class"
#define ParamName_Closed					"Closed"
#define ParamName_CollisionsActive			"CollisionsActive"
#define ParamName_Component					"Component"
#define ParamName_Components				"Components"
#define ParamName_Controller				"Controller"
#define ParamName_Diffuse					"Diffuse"
#define ParamName_Displace                  "Displace"
#define ParamName_Display					"Display"
#define ParamName_DynTex					"DynTex"
#define ParamName_DynamicTexture			"DynamicTexture"
#define ParamName_Face						"Face"
#define ParamName_Faces						"Faces"
#define ParamName_Factor					"Factor"
#define ParamName_ForceFactor				"ForceFactor"
#define ParamName_General					"General"
#define ParamName_Geoloc					"Geoloc"
#define ParamName_Height					"Height"
#define ParamName_Ignore					"Ignore"
#define ParamName_InputScale                "InputScale"
#define ParamName_IR						"IR"
#define ParamName_Iterations                "Iterations"
#define ParamName_Latitude					"Latitude"
#define ParamName_Level						"Level"
#define ParamName_Levels					"Levels"
#define ParamName_Longitude					"Longitude"
#define ParamName_Map						"Map"
#define ParamName_Material					"Material"
#define ParamName_MaxThrustKG				"MaxThrustKG"
#define ParamName_Mesh						"Mesh"
#define ParamName_MaxClamp                  "MaxClamp"
#define ParamName_MinClamp                  "MinClamp"
#define ParamName_Name						"Name"
#define ParamName_Operand   				"Operand"
#define ParamName_OutputScale               "OutputScale"
#define ParamName_Parameters				"Parameters"
#define ParamName_Parent					"Parent"
#define ParamName_Path						"Path"
#define ParamName_Pause						"Pause"
#define ParamName_Physics					"Physics"
#define ParamName_PhysicsActive				"PhysicsActive"
#define ParamName_Playing					"Playing"
#define ParamName_Position					"Position"
#define ParamName_PositionTarget			"PositionTarget"
#define ParamName_PowerInput				"PowerInput"
#define ParamName_ProceduralMesh			"ProceduralMesh"
#define ParamName_Resolution				"Resolution"
#define ParamName_Rotation					"Rotation"
#define ParamName_RotationFactor			"RotationFactor"
#define ParamName_RotationTarget			"RotationTarget"
#define ParamName_Scale						"Scale"
#define ParamName_Servo						"Servo"
#define ParamName_Specular					"Specular"
#define ParamName_Sphere					"Sphere"
#define ParamName_Step						"Step"
#define ParamName_Target					"Target"
#define ParamName_Thrust					"Thrust"
#define ParamName_Type						"Type"
#define ParamName_Translation				"Translation"
#define ParamName_Updater					"Updater"
#define ParamName_Value                     "Value"
#define ParamName_Velocity					"Velocity"
#define ParamName_VelocityFactor			"VelocityFactor"
#define ParamName_Vertex					"Vertex"
#define ParamName_Vertices					"Vertices"
#define ParamName_x							"x"
#define ParamName_y							"y"
#define ParamName_z							"z"
#define ParamName_r							"r"
#define ParamName_g							"g"
#define ParamName_b							"b"
#define ParamName_u							"u"
#define ParamName_v							"v"

//-------------------------------------------------------------------------------------------------
// Noms de shaders

#define SP_Standard		"Standard"

//-------------------------------------------------------------------------------------------------
// Ev�nements Q3D

#define Q3D_Event							"Event"
#define Q3D_LeftMouseButton					"LMB"
#define Q3D_RightMouseButton				"RMB"
#define Q3D_MiddleMouseButton				"MMB"

#define Q3DEvent_Up							"UP"
#define Q3DEvent_Down						"DOWN"
#define Q3DEvent_UpFast						"UP_FAST"
#define Q3DEvent_DownFast					"DOWN_FAST"
#define Q3DEvent_Forward					"FORWARD"
#define Q3DEvent_Backward					"BACKWARD"
#define Q3DEvent_StrafeRight				"STRAFE_RIGHT"
#define Q3DEvent_StrafeLeft					"STRAFE_LEFT"
#define Q3DEvent_TurnRight					"TURN_RIGHT"
#define Q3DEvent_TurnLeft					"TURN_LEFT"
#define Q3DEvent_LookUp						"LOOK_UP"
#define Q3DEvent_LookDown					"LOOK_DOWN"

#define Q3DEvent_AileronRight				"AILERON_RIGHT"
#define Q3DEvent_AileronLeft				"AILERON_LEFT"
#define Q3DEvent_NoseUp						"NOSE_UP"
#define Q3DEvent_NoseDown					"NOSE_DOWN"
#define Q3DEvent_RudderRight				"RUDDER_RIGHT"
#define Q3DEvent_RudderLeft					"RUDDER_LEFT"

#define Q3DEvent_LookFront					"LOOK_FRONT"
#define Q3DEvent_LookFrontRight				"LOOK_FRONT_RIGHT"
#define Q3DEvent_LookRight					"LOOK_RIGHT"
#define Q3DEvent_LookBackRight				"LOOK_BACK_RIGHT"
#define Q3DEvent_LookBack					"LOOK_BACK"
#define Q3DEvent_LookBackLeft				"LOOK_BACK_LEFT"
#define Q3DEvent_LookLeft					"LOOK_LEFT"
#define Q3DEvent_LookFrontLeft				"LOOK_FRONT_LEFT"
#define Q3DEvent_LookFrontDown				"LOOK_FRONT_DOWN"