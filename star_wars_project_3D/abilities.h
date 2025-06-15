#pragma once
#ifndef  ABILITIES_H
#define ABILITIES_H

struct Abilities
{
	vf3d trans_plane;
	Billboard* held_Object = nullptr;
	float held_dist = 0;
	void billboard_select(cmn::Engine3D* ptr,std::vector<Billboard> billboard)
	{
		
		
		
		if (ptr->GetKey(olc::V).bPressed)
		{
			float record = -1;
			held_Object = nullptr;
			for (auto& b : billboard)
			{
				float dist = b.intersectRay(ptr->cam_pos, ptr->cam_dir);
				if (dist > 0)
				{
					if (record < 0)
					{
						record = dist;
						held_Object = &b;
						held_dist = dist;
					}
				}
			}
		}

		if (held_Object != nullptr)
		{
			
			held_Object->pos = ptr->cam_pos + held_dist * ptr->cam_dir;
			
 		}

		if (ptr->GetKey(olc::V).bReleased)
		{
			held_Object = nullptr;
		}
		
		if (held_Object)
		{
			//project screen ray onto tranlsation plane
			float prev_ndc_x = 1 - 2 * ptr->previous_cam.x / ptr->ScreenWidth();
			float prev_ndc_y = 1 - 2 * ptr->previous_cam.y / ptr->ScreenHeight();
			vf3d prev_clip(prev_ndc_x, prev_ndc_y, 1);
			vf3d prev_world = prev_clip * ptr->invVP;
			prev_world /= prev_world.w;
			vf3d prev_pt = segIntersectPlane(ptr->cam_pos, prev_world, trans_plane, ptr->cam_dir);
			float curr_ndc_x = 1 - 2 * ptr->current_cam.x / ptr->ScreenWidth();
			float curr_ndc_y = 1 - 2 * ptr->current_cam.y / ptr->ScreenHeight();
			vf3d curr_clip(curr_ndc_x, curr_ndc_y, 1);
			vf3d curr_world = curr_clip * ptr->invVP;
			curr_world /= curr_world.w;
			vf3d curr_pt = segIntersectPlane(ptr->cam_pos, curr_world, trans_plane, ptr->cam_dir);

			held_Object->pos += curr_pt - prev_pt;
		}

	}

	void Object_highlight(cmn::Engine3D* ptr, std::vector<Billboard> billboard)
	{
		for (auto& b : billboard)
		{

			
			float dist = b.intersectRay(ptr->cam_pos, ptr->cam_dir);
			

			if (dist > 0)
			{
				
				int id = b.id;
				for (int i = 1; i < ptr->ScreenWidth() - 1; i++) {
					for (int j = 1; j < ptr->ScreenHeight() - 1; j++) {
						bool curr = ptr->id_buffer[i + ptr->ScreenWidth() * j] == id;
						bool lft = ptr->id_buffer[i - 1 + ptr->ScreenWidth() * j] == id;
						bool rgt = ptr->id_buffer[i + 1 + ptr->ScreenWidth() * j] == id;
						bool top = ptr->id_buffer[i + ptr->ScreenWidth() * (j - 1)] == id;
						bool btm = ptr->id_buffer[i + ptr->ScreenWidth() * (j + 1)] == id;
						if (curr != lft || curr != rgt || curr != top || curr != btm) {
							ptr->Draw(i, j, olc::WHITE);
						}
					}
				}
			}
		}
	}
};

#endif // ! ABILITIES_H

