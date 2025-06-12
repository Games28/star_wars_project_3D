#pragma once
#ifndef BILLBOARD_STRUCT_H
#define BILLBOARD_STRUCT_H

struct Billboard {
	vf3d pos;
	int id;
	float w=1, h=1;
	std::vector<cmn::Triangle> tri2;

	void update(vf3d cam_pos) {
		tri2.clear();
		vf3d norm=(pos-cam_pos).norm();
		vf3d up(0, 1, 0);
		vf3d rgt=norm.cross(up).norm();
		up=rgt.cross(norm);

		vf3d tl=pos+w/2*rgt+h/2*up;
		vf3d tr=pos+w/2*rgt-h/2*up;
		vf3d bl=pos-w/2*rgt+h/2*up;
		vf3d br=pos-w/2*rgt-h/2*up;

		cmn::v2d tl_t{0, 0};
		cmn::v2d tr_t{0, 1};
		cmn::v2d bl_t{1, 0};
		cmn::v2d br_t{1, 1};
		cmn::Triangle t1, t2;
		t1={tl, br, tr, tl_t, br_t, tr_t}; t1.id=id;
		t2={tl, bl, br, tl_t, bl_t, br_t}; t2.id=id;
		tri2.push_back(t1); tri2.push_back(t2);
	}
};
#endif
