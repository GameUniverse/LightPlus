

#include "windows.h"
#include "stdio.h"
#include "..\\..\\..\\include\\light.h"
#pragma comment( lib, "..\\..\\..\\lib\\light.lib" )
#include "stopcock.h"
#include "slinger.h"
#include "teeterboard.h"


// 机关测试演示程序
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	LightEngine           *engine;
	LightScene            *scene;
	LightCamera           *camera;
	LightMesh             *sceneMesh;
	LightSky              *sky;
	LightMesh             *boxMesh1;
	LightMesh             *boxMesh2;
	LightMesh             *sphereMesh;
	LightMesh             *boardMesh;
	LightBrush            *brush;
	LightPhysicsMaterial  *physicsMaterial;
	LightTriangleMesh     *triangleMesh;
	LightActor            *actor;
	LightActor            *physicsScene;
	LightTexture          *texture;
	LightInput            *input;
	LightText             *text;
	char string[100];
	int  mode = 0;        // 控制模式
	float velocity = 300; // 移动速度
	vec3_t rotate = { 0, 0, 0 };
	DWORD lasttime = 0;

	// 创建光线引擎实例
	LightEngineDesc engineDesc;
	engineDesc.displayMode.mode           = LIGHT_WINDOW_MODE;  // 窗口显示模式
	engineDesc.displayMode.window.x       = 200;                // 窗口左上角 x 坐标
	engineDesc.displayMode.window.y       = 150;                // 窗口左上角 y 坐标
	engineDesc.displayMode.window.width   = 640;                // 窗口宽度
	engineDesc.displayMode.window.height  = 480;                // 窗口高度
	engine = LightEngineCreate( engineDesc );

	// 使用 3DSMAX 坐标系统(Z->UP）
	engine->coordinateSystem( LIGHT_3DSMAX_COORDINATE_SYSTEM );

	// 启用关键色（默认禁用）
	engine->enable( LIGHT_KEY_COLOR );
	// 设置关键色（透明色，默认值为黑色 0，0，0 ）
	engine->setKeyColor( 255, 0, 0 );

	// 关闭垂直同步（默认开启）
	engine->disable( LIGHT_SWAP_INTERVAL );

	// 创建输入接口
	LightInputDesc inputDesc;
	input = engine->createInput( inputDesc );

	// 创建摄像机对象
	LightCameraDesc cameraDesc;
	camera = engine->createCamera( cameraDesc );

	// 创建文字对象
	LightTextDesc textDesc;
	text = engine->createText( textDesc );

	// 创建几何画笔
	LightBrushDesc brushDesc;
	brush = engine->createBrush( brushDesc );

	// 创建一个场景
	LightSceneDesc sceneDesc;
	memset( &sceneDesc, 0, sizeof(sceneDesc) );	
	scene = engine->createScene( sceneDesc );

	// 将静态网格文件读入内存
	LightMeshDesc meshDesc;
	sceneMesh = engine->createMesh( meshDesc );
	sceneMesh->load( "..\\resource\\scene\\ground\\ground2.3ds" );

	// 创建天空盒
	LightSkyDesc skyDesc;
	memset( &skyDesc, 0, sizeof(skyDesc) );
	skyDesc.type = LIGHT_SKY_BOX;                    // 天空类型
	VEC3( skyDesc.box.size, 64, 64, 64 );            // 天空盒大小
	LightTextureDesc textureDesc;
	skyDesc.top = engine->createTexture( textureDesc );
	skyDesc.top->load( "..\\resource\\sky\\fine_top.tga" );
	skyDesc.front = engine->createTexture( textureDesc );
	skyDesc.front->load( "..\\resource\\sky\\fine_front.tga" );
	skyDesc.back = engine->createTexture( textureDesc );
	skyDesc.back->load( "..\\resource\\sky\\fine_back.tga" );
	skyDesc.left = engine->createTexture( textureDesc );
	skyDesc.left->load( "..\\resource\\sky\\fine_left.tga" );
	skyDesc.right = engine->createTexture( textureDesc );
	skyDesc.right->load( "..\\resource\\sky\\fine_right.tga" );
	skyDesc.bottom = engine->createTexture( textureDesc );
	skyDesc.bottom->load( "..\\resource\\sky\\fine_bottom.tga" );
	sky = engine->createSky( skyDesc );

	// 创建物理材质
	LightPhysicsMaterialDesc  physicsMaterialDesc;
	memset( &physicsMaterialDesc, 0, sizeof( physicsMaterialDesc ) );	
	physicsMaterialDesc.restitution      = 0.8f; // 弹力系数（缺省值为 0.5）
	physicsMaterialDesc.staticFriction   = 35;   // 静态磨擦力系数（缺省值为 0.5）
	physicsMaterialDesc.dynamicFriction  = 1;    // 动态磨擦力系数（缺省值为 0.5）
	physicsMaterial = scene->createPhysicsMaterial( physicsMaterialDesc );

	// 创建球体
	LightActorDesc actorDesc;
	texture = engine->createTexture( textureDesc );
	texture->load( "..\\resource\\image\\wood.jpg" );	
	memset( &actorDesc, 0, sizeof(actorDesc) );
	actorDesc.type = LIGHT_SHAPE_SPHERE; // 角色类型
	actorDesc.dynamic = TRUE;
	VEC3( actorDesc.pos, 9, -51, 20 );   // 角色位置
	actorDesc.sphere.radius = 1.5;       // 球体半径
	actorDesc.quality_slices = 15;       // 球体显示品质
	actorDesc.quality_stacks = 15;
	VEC3( actorDesc.rotate, 1.0f, 1.0f, 0.0f ); // 旋转参数（旋转轴）
	actorDesc.angle = 45;                // 旋转参数（旋转角度）
	actorDesc.mass = 1.0;                // 角色质量
	actorDesc.hide = FALSE;              // 隐藏标志
	// 设置几何体物理材质
	actorDesc.physicsMaterialIndex = physicsMaterial->getMaterialIndex();
	actor = scene->createActor( actorDesc );

	// 创建物理场景
	triangleMesh = sceneMesh->createTriangleMesh();
	memset( &actorDesc, 0, sizeof(actorDesc) );
	actorDesc.type = LIGHT_SHAPE_MESH; // 角色类型
	actorDesc.mesh.triangleMesh = triangleMesh;
	actorDesc.physicsMaterialIndex = 0; // 使用缺省材质
	physicsScene = scene->createActor( actorDesc );

	// 将静态网格文件读入内存
	boxMesh1 = engine->createMesh( meshDesc );
	boxMesh1->load( "..\\resource\\mesh\\object\\box.3ds" );
	boxMesh2 = engine->createMesh( meshDesc );
	boxMesh2->load( "..\\resource\\mesh\\object\\box.3ds" );

	// 创建矩形活塞机关
	stopcock stopcock;
	vec3_t position1, position2, position, box;
	VEC3( position1, -8.0f, -50.0f, 6.0f );
	VEC3( position2, -5.0f, -50.0f, 6.0f );
	VEC3( position, -8.0f, -50.0f, 6.0f );
	VEC3( box, 2.3f, 2.0f, 2.0f );
	stopcock.create( position1, position2, 8, 8, 0.2f, 1, scene, boxMesh1, position, box, 4 );

	// 将静态网格文件读入内存
	sphereMesh = engine->createMesh( meshDesc );	
	//boxMesh->Load( "..\\resource\\object\\box.3ds" );
	sphereMesh->load( "..\\resource\\mesh\\object\\shape.3ds" );

	// 创建吊物机关
	slinger object;
	vec3_t obj_pos, homeplace;
	VEC3( obj_pos, 0.0f, -50.0f, 20.0f );
	VEC3( homeplace, 0.0f, -50.0f, 8.0f );
	VEC3( box, 2.0f, 2.0f, 5.0f );
	object.create( obj_pos, engine, scene, sphereMesh, brush, homeplace, box, 1 );

	// 将静态网格文件读入内存	
	boardMesh = engine->createMesh( meshDesc );
	boardMesh->load( "..\\resource\\mesh\\object\\teeterboard.3ds" );

	// 创建跷跷板机关
	teeterboard teeterboard;
	vec3_t force;
	VEC3( position1, 10.42f, -49.8f, 6.2f );
	VEC3( position2, 10.42f, -52.3f, 6.2f );
	VEC3( box, 6, 2.5f, 0.3f );
	VEC3( position, 2, 0, 0 );
	VEC3( force, 0, 0, -1 );
	teeterboard.create( position1, position2, scene, boardMesh, box, 1, position, force );

	// 设置摄像机位置和方向	
	camera->lookAt( 4, -75, 15,  0, 0, 0,  0, 0, 1 );

	// 设置最后更新时间
	lasttime = engine->getTickCount();

	do
	{
		// 更新物理引擎状态
		engine->updatePhysics();

		// 清除屏幕内容
		engine->clear( LIGHT_COLOR_BUFFER_BIT | LIGHT_DEPTH_BUFFER_BIT );

		// 渲染天空盒
		sky->render();

		// 渲染场景
		texture->active(); // （首先激活长方体纹理）
		//scene->Render();
		actor->render();

		// 渲染场景网格
		sceneMesh->render();

		// 更新机关状态
		stopcock.update();		
		// 显示机关
		stopcock.render();

		// 更新机关状态
		object.update();
		// 显示机关
		object.render();

		// 更新机关状态
		teeterboard.update();		
		// 显示机关
		teeterboard.render();

		// 显示帧速（FPS）
		sprintf( string, "帧速: %d 帧/秒", engine->getFramePerSecond() );
		text->color( 0, 255, 0 );
		text->setTextPos( 40, 400 );
		text->drawText( string );

		// 显示提示信息		
		text->setTextPos( 300, 370 );
		if( mode == 0 )
			text->drawText( "控制模式：施加力( addForce )" );
		else
		if( mode == 1 )
			text->drawText( "控制模式：设置线速度( setLinearVelocity )" );
		else
		if( mode == 2 )
			text->drawText( "控制模式：设置角速度( setAngularVelocity )" );
		text->setTextPos( 300, 400 );
		text->drawText( "W S A D = 滚动方向，空格 = 跳跃，M = 切换控制模式" );

		// 更新显示
		engine->swapBuffers();

		// 处理玩家输入指令		
		DWORD time;
		unsigned int key;

		// 查询当前系统时间
		time = engine->getTickCount();

		// 防止时间计数器溢出
		if( time < lasttime )
			lasttime = time;

		// 移动角色控制对象
		float gravity = -9.8f; // 重力
		float elapsedTime;    // 从最后一次更新以来消逝的时间
		float forward = 0, left = 0;

		// 控制球的运动方向
		if( input->getKeyState( KEY_UP ) || input->getKeyState( KEY_W ) )
			forward = 1;
		if( input->getKeyState( KEY_DOWN ) || input->getKeyState( KEY_S ) )
			forward = -1;
		if( input->getKeyState( KEY_LEFT ) || input->getKeyState( KEY_A ) )
			left = -1;
		if( input->getKeyState( KEY_RIGHT ) || input->getKeyState( KEY_D ) )
			left = 1;

		key = input->getKey();
		// 如果按下 M 键，则切换控制模式
		if( key == KEY_M )
		{
			mode++;
			if( mode > 2 )
				mode = 0;
		}
		// 如果按下空格键，则跳起
		if( key == KEY_SPACE )
			actor->addForce( 0, 0, 7000, 0 );

		// 以千分之一秒为单位进行更新
		elapsedTime = time - lasttime;
		elapsedTime /= 1000;

		// 计算力向量
		vec3_t vector, linearVelocity, angularVelocity;
		vector[0] = left * velocity;
		vector[1] = forward * velocity;
		vector[2] = gravity * elapsedTime;
		VEC3( linearVelocity, vector[0] * 0.05f, vector[1] * 0.05f, vector[2] * 0.05f );
		VEC3( angularVelocity, -vector[1], vector[0], vector[2] );

		if( elapsedTime > 0 )
		{
			if( mode == 0 )
				// 施加力
				actor->addForce( vector[0], vector[1], vector[2] );
			else
			if( mode == 1 )
				// 设置线速度
				actor->setLinearVelocity( linearVelocity );
			else
			if( mode == 2 )
				// 设置角速度
				actor->setAngularVelocity( angularVelocity );
			lasttime = time;
		}
	}
	// 配送消息
	while( engine->dispatchMessage() && !input->getKeyState( KEY_ESCAPE ) );

	// 释放分配的对象
	scene->releaseActor( physicsScene );               // 释放物理角色
	scene->releaseActor( actor );
	scene->releasePhysicsMaterial( physicsMaterial );  // 释放物理材质
	sceneMesh->releaseTriangleMesh( triangleMesh );    // 释放物理网格
	engine->releaseScene( scene );                     // 释放物理场景所占内存
	engine->releaseMesh( sceneMesh );                  // 释放静态网格对象
	engine->releaseTexture( texture );                 // 释放纹理
	engine->releaseSky( sky );	                       // 释放天空盒对象
	engine->releaseInput( input );                     // 释放输入接口
	engine->releaseCamera( camera );                   // 释放摄像机对象
	engine->releaseText( text );                       // 释放文字对象

	// 释放光线引擎对象
	LightEngineRelease( engine );

	return 0;
}