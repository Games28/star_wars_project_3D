#pragma once
#ifndef PLAYER_H
#define PLAYER_H

class Player
{

public:
	Player() = default;



	void player_camera_switch(cmn::vf3d& cam_pos)
	{
		if (!player_camera)
		{
			if (!player_camera)
			{
				player_pos = cam_pos - vf3d(0, player_height, 0);
				player_vel = { 0,0,0 };
			}

			player_camera ^= true;
		}
	}

	void player_collision(cmn::Engine3D* pge,float dt, std::vector<Mesh> meshes)
	{
		vf3d new_pos = pge->cam_pos;
		vf3d movement;
		

		//move forward, backward
		vf3d fb_dir(std::cosf(pge->cam_yaw), 0, std::sinf(pge->cam_yaw));
		if (pge->GetKey(olc::Key::W).bHeld) movement += 5.f * dt * fb_dir;
		if (pge->GetKey(olc::Key::S).bHeld) movement -= 3.f * dt * fb_dir;

		//move left, right
		vf3d lr_dir(fb_dir.z, 0, -fb_dir.x);
		if (pge->GetKey(olc::Key::A).bHeld) movement += 4.f * dt * lr_dir;
		if (pge->GetKey(olc::Key::D).bHeld) movement -= 4.f * dt * lr_dir;

		if (player_on_ground)
		{
			float record;
			const cmn::Triangle* closest = nullptr;
			for (int i = 0; i < meshes.size(); i++)
			{
				for (const auto& t : meshes[i].tris)
				{
					vf3d close_pt = t.getClosePt(player_pos);
					vf3d sub = close_pt - player_pos;
					float dist2 = sub.mag2();
					if (!closest || dist2 < record)
					{
						record = dist2;
						closest = &t;
					}
				}
			}

			if (closest)
			{
				vf3d norm = closest->getNorm();
				movement -= norm * norm.dot(movement);
			}

		}

		
			player_pos += movement;
		

		//jumping
		if (pge->GetKey(olc::Key::SHIFT).bPressed)
		{
			if (player_airtime < 0.1f)
			{
				player_vel.y = 300 * dt;
				player_on_ground = false;
			}
		}

		//player pos is at feet, so offset camera to head
		if (pge->scene_bound.contains(movement))
		{
			pge->cam_pos = player_pos + vf3d(0, player_height, 0);
		}
		


	}

	void player_jumping(std::vector<Mesh> meshes, float dt)
	{
		if (player_camera)
		{
			if (!player_on_ground)
			{
				player_vel += gravity * dt;
			}

			player_pos += player_vel * dt;

			//airtrime
			player_airtime += dt;

			player_on_ground = false;

			for (int i = 0; i < meshes.size(); i++)
			{
				for (const auto& t : meshes[i].tris) {
					vf3d close_pt = t.getClosePt(player_pos);
					vf3d sub = player_pos - close_pt;
					float dist2 = sub.mag2();


					if (dist2 < player_rad * player_rad) {
						float fix = player_rad - std::sqrtf(dist2);
						player_pos += fix * t.getNorm();
						player_vel = { 0, 0, 0 };
						player_on_ground = true;
						player_airtime = 0;
					}
				}
			}
		}
	}

	void nagvigator(cmn::Engine3D* ptr,std::vector<Mesh> meshes,vf3d bill_board_pos)
	{
		                               
		
	}

	void set_player_cam()
	{
		player_camera = false;
	}

	bool player_Cam()
	{
		return player_camera;
	}

	private:
	//player physics
	bool player_camera = false;
	bool player_on_ground = false;
	vf3d player_pos = { 0,0,0 };
	vf3d player_vel = { 0,0,0 };
	vf3d gravity{ 0, -9.8f, 0 };
	const float player_rad = .1f;
	const float player_height = .25f;
	float player_airtime = 0;


};
#endif // !PLAYER_H
