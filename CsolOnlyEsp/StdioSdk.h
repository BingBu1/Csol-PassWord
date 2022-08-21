#pragma once
typedef struct r_studio_interface_s
{
	int				version;
	int				(*StudioDrawModel)			(int flags);
	int				(*StudioDrawPlayer)			(int flags, struct entity_state_s* pplayer);
	int				(*StudioDrawUkon)				(int flags, int Unk);//游戏并没有调用
	int				(*StudioPlayerModle)			();
} r_studio_interface_t;