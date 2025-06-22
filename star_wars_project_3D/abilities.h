#pragma once
#ifndef  ABILITIES_H
#define ABILITIES_H

struct Abilities
{
	vf3d trans_plane;
	Billboard* held_Object = nullptr;
	float held_dist = 0;
	void billboard_select(cmn::Engine3D* ptr,std::vector<Billboard>& billboard)
	{
		
		
		
		if (ptr->GetKey(olc::SPACE).bPressed)
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

		if (ptr->GetKey(olc::SPACE).bReleased)
		{
			held_Object = nullptr;
		}
		
		

	}

	void Object_highlight(cmn::Engine3D* ptr, std::vector<Billboard>& billboard)
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

