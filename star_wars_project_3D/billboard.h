#pragma once
#ifndef BILLBOARD_STRUCT_H
#define BILLBOARD_STRUCT_H

struct Billboard {
    vf3d pos;
    int id;
    float w = 1, h = 1;
    cmn::Triangle tris[2];

    olc::Sprite* base_sprite = nullptr;
    olc::Sprite* seleted_sprite = nullptr;

    //billboard intersectray
    float intersectRay(const vf3d& orig, const vf3d& dir) const {
        //sort by closest
        float record = -1;
        for (int i = 0; i < 2; i++) {
            const auto& t = tris[i];
            float dist = segIntersectTri(orig, orig + dir, t);
            if (dist > 0) {
                if (record < 0 || dist < record) {
                    record = dist;
                }
            }
        }

        return record;
    }

    void update(vf3d cam_pos) {
        vf3d norm = (pos - cam_pos).norm();
        vf3d up(0, 1, 0);
        vf3d rgt = norm.cross(up).norm();
        up = rgt.cross(norm);

        vf3d tl = pos + w / 2 * rgt + h / 2 * up;
        vf3d tr = pos + w / 2 * rgt - h / 2 * up;
        vf3d bl = pos - w / 2 * rgt + h / 2 * up;
        vf3d br = pos - w / 2 * rgt - h / 2 * up;

        //texture coords
        cmn::v2d tl_t{ 0, 0 };
        cmn::v2d tr_t{ 0, 1 };
        cmn::v2d bl_t{ 1, 0 };
        cmn::v2d br_t{ 1, 1 };

        //tessellate
        tris[0] = { tl, br, tr, tl_t, br_t, tr_t }; tris[0].id = id;
        tris[1] = { tl, bl, br, tl_t, bl_t, br_t }; tris[1].id = id;
    }

    void sprite_update(cmn::Engine3D* ptr,std::vector<cmn::Triangle>& tries_to_project, std::vector<olc::Sprite*>& texture_atlas)
    {
        texture_atlas.push_back(base_sprite);



        tries_to_project.push_back(tris[0]);
        tries_to_project.push_back(tris[1]);
    }

    olc::Sprite* change_sprite()
    {

    }

    void render(cmn::Engine3D* ptr)
    {

    }

   

};
#endif
