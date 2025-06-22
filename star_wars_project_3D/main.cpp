#include "3d/engine_3d.h"
using cmn::vf3d;
using cmn::Mat4;

#include "mesh.h"

#include "billboard.h"
#include "Player.h"
#include "abilities.h"
#include "pathfinding.h"



constexpr float Pi=3.1415927f;

struct Demo3D : cmn::Engine3D {
	Demo3D() {
		sAppName="3D Demo";
	}

	//camera positioning
	
	std::vector<Mesh> meshes;
	std::vector<Billboard> sprites;

	std::vector<olc::Sprite*> texture_atlas;

	

	bool show_bounds=false;

	bool seek_active = false;
	
	PathFinder path;

	Player player;
	Abilities ability;


	bool user_create() override {
		cam_pos={5, 3, 3};
		light_pos=cam_pos;

		
		path.Setup();



		try {
			{
				Mesh a = Mesh::loadFromOBJ("./assets/models/desert.obj");
				a.translation = { 0, 0, 0 };
				a.scale = { 1.0f,1.0f,1.0f };
				a.updateTransforms();
				a.id = 0;
				a.updateTriangles();
				//texture_atlas.push_back(new olc::Sprite("./assets/textures/sandtexture.png"));
				//a.fitToBounds(scene_bound);
				//a.colorNormals();

				//meshes.push_back(a);
			}
			
			{
				Mesh b=Mesh::loadFromOBJ("assets/models/tathouse1.txt");
				b.translation={8, 2.5, 7};
				b.scale={0.25f, 0.25f, 0.25f};
				b.updateTransforms();
				b.id=1;
				b.updateTriangles();
				//texture_atlas.push_back(new olc::Sprite("assets/textures/Tbuilding.png"));

				//meshes.push_back(b);
			}

			{
				Mesh c = Mesh::loadFromOBJ("./assets/models/tathouse2.txt");
				c.translation = { 5, 2.5, 1 };
				c.scale = { 0.25f,0.25f,0.25f };
				c.updateTransforms();
				c.id=2;
				c.updateTriangles();
				//texture_atlas.push_back(new olc::Sprite("./assets/textures/Tbuilding.png"));

				//meshes.push_back(c);
			}

		

			//3d sprites
			Billboard p1{vf3d(+5, 3, 4), 0};

			texture_atlas.push_back(new olc::Sprite("assets/sprites/trooper_idle.png"));
			sprites.push_back(p1);

			Billboard p2({+5, 3, 7}, 4);
			//texture_atlas.push_back(new olc::Sprite("assets/sprites/trooper_idle.png"));
			//sprites.push_back(p2);
		} catch(const std::exception& e) {
			std::cout<<e.what()<<'\n';
			return false;
		}

		for (int i = 1; i < meshes.size(); i++)
		{
			path.add_Obstacle(meshes[i].translation);
		}


		return true;
	}
	
	bool user_update(float dt) override {

		previous_cam = cam_pos;
		//look up, down
		if(GetKey(olc::Key::UP).bHeld) cam_pitch+=dt;
		if(GetKey(olc::Key::DOWN).bHeld) cam_pitch-=dt;
		//clamp cam_pitch values
		if(cam_pitch>Pi/2) cam_pitch=Pi/2-.001f;
		if(cam_pitch<-Pi/2) cam_pitch=.001f-Pi/2;

		//look left, right
		if(GetKey(olc::Key::LEFT).bHeld) cam_yaw-=dt;
		if(GetKey(olc::Key::RIGHT).bHeld) cam_yaw+=dt;

		//polar to cartesian
		cam_dir=vf3d(
			std::cosf(cam_yaw)*std::cosf(cam_pitch),
			std::sinf(cam_pitch),
			std::sinf(cam_yaw)*std::cosf(cam_pitch)
		);
		
		

		//turn off and on player camera physics
		if (GetKey(olc::Key::Z).bPressed)
		{
			//test
			//
			player.player_camera_switch(cam_pos);
			
		}

		if (GetKey(olc::Key::X).bPressed)
		{
			player.set_player_cam();
		}

		vf3d new_cam_pos = cam_pos;
		//player camera physics
		if (player.player_Cam())
		{
			
			player.player_collision(this, dt ,meshes);
			

		}
		else
		{

			//move up, down
			if (GetKey(olc::Key::SPACE).bHeld) new_cam_pos.y += 4.f * dt;
			if (GetKey(olc::Key::SHIFT).bHeld) new_cam_pos.y -= 4.f * dt;


			vf3d fb_dir(std::cosf(cam_yaw), 0, std::sinf(cam_yaw));
			if (GetKey(olc::Key::W).bHeld) new_cam_pos += 5.f * dt * fb_dir;
			if (GetKey(olc::Key::S).bHeld) new_cam_pos -= 3.f * dt * fb_dir;

			//move left, right
			vf3d lr_dir(fb_dir.z, 0, -fb_dir.x);
			if (GetKey(olc::Key::A).bHeld) new_cam_pos += 4.f * dt * lr_dir;
			if (GetKey(olc::Key::D).bHeld) new_cam_pos -= 4.f * dt * lr_dir;
			if (scene_bound.contains(new_cam_pos))
			{
				cam_pos = new_cam_pos;
				current_cam = cam_pos;
			}
		}

		

		try
		{
			invVP = Mat4::inverse(mat_view * mat_proj);
		}
		catch (const std::exception& e)
		{
			invVP_avail = false;
		}

		//update world mouse ray
		if (invVP_avail)
		{
			float ndc_x = 1 - 2.f * GetMouseX() / ScreenWidth();
			float ndc_y = 1 - 2.f * GetMouseY() / ScreenHeight();

			vf3d clip(ndc_x, ndc_y, 1);
			vf3d world = clip * invVP;
			world /= world.w;

			
		}


		player.player_jumping(meshes, dt);
		


		


		//set light pos
		if(GetKey(olc::Key::L).bHeld) light_pos=cam_pos;

		if(GetKey(olc::Key::B).bPressed) show_bounds^=true;

		if (GetKey(olc::Key::M).bPressed) seek_active = !seek_active;


		

		//pathfinder test
		for (auto& s : sprites)
		{
			path.update(this, s.pos);
		}

		if (GetKey(olc::Key::P).bPressed) path.pathfinding_active = !path.pathfinding_active;
		if (GetKey(olc::Key::O).bPressed) path.following_path = !path.following_path;

		if (path.pathfinding_active)
		{
			path.solve_star();
			path.setup_path();
			
		}

		if (path.following_path)
		{
			path.path_following(sprites,dt);

		}
		

		//player.nagvigator(this, meshes, sprites[0].pos);
		//if (seek_active)
		//{
		//	for (auto& s : sprites)
		//	{
		//		s.seek(cam_pos);
		//		s.force_update(dt);
		//	}
		//
		//}

		//update sprite directions and triangles
		for(auto& s:sprites) {
			
			s.update(cam_pos);
		}

		ability.billboard_select(this, sprites);


		return true;
	}

	//add geometry to scene
	bool user_geometry() override {
		for(const auto& m:meshes) {
			tris_to_project.insert(tris_to_project.end(),
				m.tris.begin(), m.tris.end());
		}

		for(auto& s:sprites) {
			tris_to_project.push_back(s.tris[0]);
			tris_to_project.push_back(s.tris[1]);
		}

		if(show_bounds) {
			for(const auto& m:meshes) {
				addAABB(m.getAABB(), olc::WHITE);
			}
		}

		return true;
	}

	bool user_render() override {
		//grey background
		Clear(olc::Pixel(90, 90, 90));

		//draw the 3d stuff
		resetBuffers();

		for(const auto& t:tris_to_draw) {
		
			FillTexturedDepthTriangle(
				t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
				t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
				t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w,
				texture_atlas[t.id], t.col, t.id
			);
		}

		for(const auto& l:lines_to_draw) {
			DrawDepthLine(
				l.p[0].x, l.p[0].y, l.t[0].w,
				l.p[1].x, l.p[1].y, l.t[1].w,
				l.col, l.id
			);
		}

		ability.Object_highlight(this, sprites);


		path.render_node_map(this);

		return true;




	}
};

int main() {
	Demo3D d3d;
	if(d3d.Construct(800, 600, 1, 1, false, true)) d3d.Start();

	return 0;
}