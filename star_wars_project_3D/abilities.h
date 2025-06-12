#pragma once
#ifndef  ABILITIES_H
#define ABILITIES_H

struct Abilities
{
	Mesh* held_Object = nullptr;
	float held_dist = 0;
	void Object_highlight(cmn::Engine3D* ptr,std::vector<Billboard> billboard)
	{
		float record = -1;
		held_Object = nullptr;
		Mesh m;
		for (auto& b : billboard)
		{
			float dist = m.intersectRay(ptr->cam_pos, ptr->cam_dir,b.tri2);
			if (dist > 0)
			{
				if (record < 0)
				{
					record = dist;
					held_Object = &m;
					held_dist = dist;
				} 
			}
		}


		for (auto& b : billboard)
		{
			
		    
			float dist = m.intersectRay(ptr->cam_pos, ptr->cam_dir, b.tri2);
			std::cout << "dist: " << dist << std::endl;
			
			if (dist > 0)
			{
				std::cout << "id: " << m.id << std::endl;
				
				//int id = m.id;
				//for (int i = 1; i < ptr->ScreenWidth() - 1; i++) {
				//	for (int j = 1; j < ptr->ScreenHeight() - 1; j++) {
				//		bool curr = ptr->id_buffer[i + ptr->ScreenWidth() * j] == id;
				//		bool lft = ptr->id_buffer[i - 1 + ptr->ScreenWidth() * j] == id;
				//		bool rgt = ptr->id_buffer[i + 1 + ptr->ScreenWidth() * j] == id;
				//		bool top = ptr->id_buffer[i + ptr->ScreenWidth() * (j - 1)] == id;
				//		bool btm = ptr->id_buffer[i + ptr->ScreenWidth() * (j + 1)] == id;
				//		if (curr != lft || curr != rgt || curr != top || curr != btm) {
				//			ptr->Draw(i, j, olc::WHITE);
				//		}
				//	}
				//}
			}
		}
	}
};

#endif // ! ABILITIES_H

