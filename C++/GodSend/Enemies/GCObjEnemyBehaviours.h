#ifndef _GCOBJENEMYBEHAVIOURS_H_
#define _GCOBJENEMYBEHAVIOURS_H_

//enum used to keep track of enemy behaviours
enum EEnemyBehaviours
{
	EEmyBehav_platform_start = 1,
	EEmyBehav_platform_end,
	EEmyBehav_direction_switch,
	EEmyBehav_direction_switch_left,
	EEmyBehav_direction_switch_right,
	EEmyBehav_die,
	EEmyBehav_stun,
	EEmyBehav_attack
};

#endif