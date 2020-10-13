#pragma once

#include "q2_shared.h"
#define	GAME_API_VERSION	3
#define NUM_ITEMS 53
#define MAX_WEAPONS 10

#define MAX_LIGHT_SOURCES 512
#define	MAX_DLIGHTS		32
#define	MAX_ENTITIES	128
#define	MAX_PARTICLES	4096

#define MAX_CAST_GROUPS		16
#define MAX_CHARACTERS		64		// this includes players
#define	DEFAULT_MAX_ATTACK_DISTANCE		1024			// stand and attack enemies within this range
#define	DEFAULT_MAX_SIGHTING_DISTANCE	2048			// can't see things outside this range

// edict->svflags

#define	SVF_NOCLIENT			0x00000001	// don't send entity to clients, even if it has effects
#define	SVF_DEADMONSTER			0x00000002	// treat as CONTENTS_DEADMONSTER for collision
#define	SVF_MONSTER				0x00000004	// treat as CONTENTS_MONSTER for collision
//ZOID
#define SVF_PROJECTILE			0x00000008  // entity is simple projectile, used for network optimization
// if an entity is projectile, the model index/x/y/z/pitch/yaw are sent, encoded into
// seven (or eight) bytes.  This is to speed up projectiles.  Currently, only the
// hyperblaster makes use of this.  use for items that are moving with a constant
// velocity that don't change direction or model
//ZOID

// edict->solid values

typedef enum
{
	MOVETYPE_NONE,			// never moves
	MOVETYPE_NOCLIP,		// origin and angles change with no interaction
	MOVETYPE_PUSH,			// no clip to world, push on box contact
	MOVETYPE_STOP,			// no clip to world, stops on box contact

	MOVETYPE_WALK,			// gravity, bad movetype
	MOVETYPE_STEP,			// gravity, special edge handling
	MOVETYPE_FLY,
	MOVETYPE_TOSS,			// gravity
	MOVETYPE_FLYMISSILE,	// extra size to monsters
	MOVETYPE_BOUNCE,		// added this (the comma at the end of line)
	MOVETYPE_WALLBOUNCE,
	MOVETYPE_TOSS_SLIDE		// Ridah, testing
} movetype_t;

typedef enum
{
	WEAPON_READY,
	WEAPON_ACTIVATING,
	WEAPON_DROPPING,
	WEAPON_FIRING,
	// RAFAEL 01-11-99
	WEAPON_RELOADING,
	WEAPON_RELOAD_CYCLE,
	WEAPON_RELOADING_SPISTOL
} weaponstate_t;

typedef struct voice_table_s voice_table_t;

// JOSEPH 11-MAR-99
struct voice_table_s
{
	voice_table_t	*response_table;		// match responses to this table
	char *text;
	char *text2;
	char *filename;
	int num_responses;	// 0 for anything
	int responses[5];	// list of indexes of sounds that would possibly match this comment
						// NOTE: these could reference sounds in other tables!
	float	last_played;
	BOOL	response;	// set if this "response" can only be played after another comment

	int	type;

	int	soundindex;		// for faster playing of sounds that have already been played this level
	int	gameinc_soundindex;	// so we can reset soundindexes on a new level
};
// END JOSEPH

typedef struct gitem_s
{
	const char		*classname;	// spawning name
	BOOL(*pickup)(struct edict_s *ent, struct edict_s *other);
	void(*use)(struct edict_s *ent, struct gitem_s *item);
	void(*drop)(struct edict_s *ent, struct gitem_s *item);
	void(*weaponthink)(struct edict_s *ent);
	const char		*pickup_sound;
	const char		*world_model;
	int			world_model_flags;
	const char		*view_model;

	// client side info
	const char		*icon;
	const char		*pickup_name;	// for printing on pickup
	int			count_width;		// number of digits to display by icon

	int			quantity;		// for ammo how much, for weapons how much is used per shot
	const char		*ammo;			// for weapons
	int			flags;			// IT_* flags

	int			weapmodel;		// weapon model index (for weapons)

	void		*info;
	int			tag;

	const char		*precaches;		// string of all models, sounds, and images this item will use
} gitem_t;

typedef enum
{
	SOLID_NOT,			// no interaction with other objects
	SOLID_TRIGGER,		// only touch when inside, after moving
	SOLID_BBOX,			// touch on edge
	SOLID_BSP			// bsp clip, touch on edge
} solid_t;



//===============================================================

// link_t is only used for entity area links now
typedef struct link_s
{
	struct link_s	*prev, *next;
} link_t;

#define	MAX_ENT_CLUSTERS	16


typedef struct edict_s edict_t;
typedef struct gclient_s gclient_t;

typedef enum
{
	resp_yes,
	resp_no
} response_t;

typedef struct
{
	int		flags;			// changes the way the CNS handles the entity

	short	cache_node;				// last successful closest node
	float	cache_node_time;		// time of last cached node
	float	cache_valid_time;		// time a cached node is deemed valid

	short	goal_index;				// 1 + (index of the node to head for)

	csurface_t	*surface;			// current surface standing on
} nav_data_t;


typedef struct
{
	int		flags;
	void	*current_node;			// last node dropped, or closest node
	vec3_t	old_org;
	edict_t	*old_groundentity;
	float	last_max_z;				// used for ducking/standing

	vec3_t	ducking_org;

	edict_t	*jump_ent;

	// debugging only
	edict_t	*debug_dest;
} nav_build_data_t;


typedef struct cast_memory_s
{
	int			cast_ent;		// "other" character index in the g_edicts[]

	vec3_t		last_known_origin;
	int			flags;			// deafult to 0, set various flags under certain conditions
	int			memory_type;	// one of MEMORY_TYPE_* constants, defines which cast_memory type this is in, for fast references

	float		timestamp;		// time that this information was updated 
								// (when gaining information from another character, 
								// this information is simply copied across, so we 
								// don't share information infinitely)
	float		timestamp_dist;	// distance at last sighting

	float		not_holstered_attack_time;
	float		ignore_time;	// pretend we can't see them until this time

	byte		inc;			// this can be used for anything, like counting the times we've warned this person, etc

	void(*response)(edict_t *self, edict_t *other, response_t response);		// when a player talks to us, call this response routine

	short		last_known_closest_node;	// so we can speed things up

	struct cast_memory_s
		*next, *prev;
} cast_memory_t;

struct matrix3x3_s {
	float matrix_x[3];
	float matrix_y[3];
	float matrix_z[3];
};

struct matrix4x4_s
{
	float m[16];
};

typedef enum {
	ca_uninitialized,
	ca_disconnected, 	// not talking to a server
	ca_connecting,		// sending request packets to the server
	ca_connected,		// netchan_t established, waiting for svc_serverdata
	ca_active			// game views should be displayed
} connstate_t;

typedef struct
{
	char		userinfo[MAX_INFO_STRING];
	char		netname[16];
	int			hand;
	int			version;		// collected from Userinfo, used to determine

	BOOL	connected;			// a loadgame will leave valid entities that
									// just don't have a connection yet

									// values saved and restored from edicts when changing levels
	int			health;
	int			max_health;
	int			savedFlags;

	int			selected_item;
	int			inventory[MAX_ITEMS];

	// ammo capacities
	int			max_bullets;
	int			max_shells;
	int			max_rockets;
	int			max_grenades;
	int			max_cells;
	int			max_slugs;
	// RAFAEL
	int			max_magslug;
	int			max_trap;

	gitem_t		*weapon;
	gitem_t		*lastweapon;

	// JOSEPH 25-SEP-98
	gitem_t  *holsteredweapon;

	// JOSEPH 3-FEB-99
	int			currentcash;
	// END JOSEPH

	int			power_cubes;	// used for tracking the cubes in coop games
	int			score;			// for calculating total unit score in coop games

	int			game_helpchanged;
	int			helpchanged;
	int			weapon_clip[MAX_WEAPONS];
	int			pistol_mods;

	// shared flags for episode
	int			episode_flags;
	int			silencer_shots;

	// number of hired guys
	int			friends;

	// teamplay
	int			team;
	int			bagcash;		// cash being held in bag

	int			hmg_shots;

	// bagman stuff
	float		timeatsafe;		// record how long we've been standing at the safe
	int			friendly_vulnerable;

} client_persistant_t;

typedef struct
{
	client_persistant_t	coop_respawn;	// what to set client->pers to on a respawn
	int			enterframe;			// level.framenum the client entered the game
	int			score;				// frags, etc
	vec3_t		cmd_angles;			// angles sent over in the last command
	int			game_helpchanged;
	int			helpchanged;

	// teamplay
	int			deposited;		// amount this player has deposited

								// voting system
	char		ban_id;			// so we only let them vote once
	float		last_ban;

} client_respawn_t;

typedef struct
{
	// fixed data
	vec3_t		start_origin;
	vec3_t		start_angles;
	vec3_t		end_origin;
	vec3_t		end_angles;

	int			sound_start;
	int			sound_middle;
	int			sound_end;
	int			sound_start2;
	int			sound_middle2;
	int			sound_end2;

	float		accel;
	float		speed;
	float		decel;
	float		distance;

	float		wait;

	// state data
	int			state;
	vec3_t		dir;
	float		current_speed;
	float		move_speed;
	float		next_speed;
	float		remaining_distance;
	float		decel_distance;
	void(*endfunc)(edict_t *);
} moveinfo_t;

typedef struct
{
	void(*aifunc)(edict_t *self, float dist);
	float	dist;
	void(*thinkfunc)(edict_t *self);
} mframe_t;

typedef struct
{
	int			firstframe;
	int			lastframe;
	mframe_t	*frame;
	//	float		*lateral_array;
	void(*endfunc)(edict_t *self);
} mmove_t;

typedef struct
{
	mmove_t		*currentmove;
	int			aiflags;
	int			nextframe;
	float		scale;

	void(*idle)(edict_t *self);
	void(*search)(edict_t *self);
	void(*dodge)(edict_t *self, edict_t *other, float eta);

	bool(*attack)(edict_t *self);		// standing attack
	void(*long_attack)(edict_t *self);	// running attack (run, and attack if facing enemy and path is clear)

	void(*sight)(edict_t *self, edict_t *other);
	bool(*checkattack)(edict_t *self);

	// Ridah, new AI system
	mmove_t		*oldcurrentmove;		// used for thug_sit

	void(*duck)(edict_t *self);		// called when beginning to duck (may be part of normal movement)
	void(*talk)(edict_t *self);		// will start a random talking animation, and play a sound

	void(*avoid)(edict_t *self, edict_t *other, BOOL face);    // will start a random talking animation, and play a sound

	void(*catch_fire)(edict_t *self, edict_t *other);

	float		last_talk_turn;
	edict_t		*talk_ent;					// person we are talking to

	float		max_attack_distance;		// try and move closer to enemy if outside this range
	float		max_sighting_distance;		// can't see things out of this range

	float		standing_max_z;				// set to self->maxs[2] upon spawning
	float		last_avoid, last_reverse;
	edict_t		*avoid_ent;

	float		goal_ent_pausetime;			// avoid goal_ent if > level.time

	int			last_side_attack;
	float		last_side_attack_time;

	cast_memory_t	*friend_memory;
	cast_memory_t	*enemy_memory;
	cast_memory_t	*neutral_memory;

	// these allow a generic AI system for all characters to interact with the animations
	mmove_t		*move_stand;
	mmove_t		*move_crstand;

	mmove_t		*move_run;
	mmove_t		*move_runwalk;
	mmove_t		*move_crwalk;

	mmove_t		*move_jump;

	mmove_t		*move_crouch_down;
	mmove_t		*move_stand_up;

	mmove_t		*move_avoid_walk;
	mmove_t		*move_avoid_run;
	mmove_t		*move_avoid_reverse_walk;
	mmove_t		*move_avoid_reverse_run;
	mmove_t		*move_avoid_crwalk;

	mmove_t		*move_lside_step;
	mmove_t		*move_rside_step;

	mmove_t		*move_start_climb;
	mmove_t		*move_end_climb;

	mmove_t		*move_evade;
	mmove_t		*move_stand_evade;		// not quite a hostile enemy yet, but they may have their weapon out
	void(*backoff)(edict_t *self, edict_t *other);
	// done.

	float		pausetime;

	vec3_t		saved_goal;
	vec3_t		last_sighting;
	float		idle_time;
	int			linkcount;

	// Rafael
	// was missing this variable
	float		trail_time;

} cast_info_t;

struct gclient_s
{
	// known to server
	player_state_t	ps;				// communicated by server to clients
	int				ping;

	// private to game
	client_persistant_t	pers;
	client_respawn_t	resp;
	pmove_state_t		old_pmove;	// for detecting out-of-pmove changes

	BOOL		showscores;			// set layout stat
	BOOL		showinventory;		// set layout stat
	BOOL		showhelp;
	BOOL		showhelpicon;

	int			ammo_index;

	int			buttons;
	int			oldbuttons;
	int			latched_buttons;

	BOOL	weapon_thunk;

	gitem_t		*newweapon;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int			damage_armor;		// damage absorbed by armor
	int			damage_parmor;		// damage absorbed by power armor
	int			damage_blood;		// damage taken out of health
	int			damage_flame;		// Ridah, damage from flamethrower
	int			damage_knockback;	// impact damage
	vec3_t		damage_from;		// origin for vector calculation

	float		killer_yaw;			// when dead, look at killer

	weaponstate_t	weaponstate;
	vec3_t		kick_angles;	// weapon kicks
	vec3_t		kick_origin;
	float		v_dmg_roll, v_dmg_pitch, v_dmg_time;	// damage kicks
	float		fall_time, fall_value;		// for view drop on fall
	float		damage_alpha;
	float		bonus_alpha;
	// JOSEPH 6-JAN-99
	int			bonus_alpha_color;
	vec3_t		damage_blend;
	vec3_t		v_angle;			// aiming direction
	float		bobtime;			// so off-ground doesn't change it
	vec3_t		oldviewangles;
	vec3_t		oldvelocity;

	float		next_drown_time;
	int			old_waterlevel;
	int			breather_sound;

	int			machinegun_shots;	// not used anymore

									// animation vars
	int			anim_end;
	int			anim_priority;
	BOOL		anim_duck;
	BOOL		anim_run;
	BOOL		anim_reverse;		// Ridah, for running backwards
	BOOL		anim_slide;			// Ridah, strafing
	float		last_climb_anim_z;	// Z value at last climbing animation change
	int			last_weapontype;	// Ridah, so we change animations when they change weapons

									// powerup timers
	float		quad_framenum;
	float		invincible_framenum;
	float		breather_framenum;
	float		enviro_framenum;

	BOOL	grenade_blew_up;
	float		grenade_time;
	// RAFAEL
	float		quadfire_framenum;
	BOOL	trap_blew_up;
	float		trap_time;

	int			weapon_sound;

	float		pickup_msg_time;

	// JOSEPH 2-FEB-99
	float		hud_enemy_talk_time;
	float		hud_self_talk_time;
	// END JOSEPH

	float		flood_locktill;		// locked from talking
	float		flood_when[10];		// when messages were said
	int			flood_whenhead;		// head pointer for when said
	char		flood_lastmsg[160];	// so we don't repeat ourselves (annoying)

	float		respawn_time;		// can respawn when time > this

	BOOL	flashlight;

	// RAFAEL	28-dec-98
	BOOL	gun_noise;

	// Ridah	
	float		jetpack_power;		// recharges at 0.5 per second (while not in use), uses at 1.0 per second, maxes at 5.0 seconds of power
	BOOL	jetpack_warned;

	// RAFAEL 01-11-99
	BOOL	reload_weapon;
	int			clip_index;

	float		last_wave;

	// chase
	edict_t		*chase_target;
	BOOL	update_chase;
};


struct edict_s
{
	entity_state_t	s;
	struct gclient_s	*client;	// NULL if not a player
									// the server expects the first part
									// of gclient_s to be a player_state_t
									// but the rest of it is opaque

	BOOL		inuse;
	int			linkcount;

	// FIXME: move these fields to a server private sv_entity_t
	link_t		area;				// linked to a division node or leaf

	int			num_clusters;		// if -1, use headnode instead
	int			clusternums[MAX_ENT_CLUSTERS];
	int			headnode;			// unused if num_clusters != -1
	int			areanum, areanum2;

	//================================

	int			svflags;
	vec3_t		mins, maxs;
	vec3_t		absmin, absmax, size;
	solid_t		solid;
	int			clipmask;
	edict_t		*owner;

	float		voice_pitch;		// used to pitch voices up/down, 1.0 = same, 2.0 = chipmunk (double speed)

									// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
									// EXPECTS THE FIELDS IN THAT ORDER!

									//================================
	int			movetype;
	int			flags;

	char		*model;
	float		freetime;			// sv.time when the object was freed

									//
									// only used locally in game, not by server
									//
	char		*message;
	char		*classname;
	int			spawnflags;

	float		timestamp;

	float		angle;			// set in qe3, -1 = up, -2 = down
	char		*target;
	char		*targetname;
	char		*killtarget;
	char		*team;
	char		*pathtarget;
	char		*deathtarget;
	char		*combattarget;
	edict_t		*target_ent;

	float		speed, accel, decel;
	vec3_t		movedir;
	vec3_t		pos1, pos2;

	vec3_t		velocity;
	vec3_t		avelocity;
	int			mass;
	float		air_finished;
	float		gravity;		// per entity gravity multiplier (1.0 is normal)
								// use for lowgrav artifact, flares

	edict_t		*goalentity;
	edict_t		*movetarget;
	float		yaw_speed;
	float		ideal_yaw;

	float		nextthink;
	void(*prethink) (edict_t *ent);
	void(*think)(edict_t *self);
	void(*blocked)(edict_t *self, edict_t *other);	//move to moveinfo?
	void(*touch)(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
	void(*use)(edict_t *self, edict_t *other, edict_t *activator);
	void(*pain)(edict_t *self, edict_t *other, float kick, int damage, int mdx_part, int mdx_subobject);
	void(*die)(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int mdx_part, int mdx_subobject);

	float		touch_debounce_time;		// are all these legit?  do we need more/less of them?
	float		pain_debounce_time;
	float		damage_debounce_time;
	float		fly_sound_debounce_time;	//move to clientinfo
	float		last_move_time;

	int			health;
	int			max_health;
	int			gib_health;
	int			deadflag;
	BOOL		show_hostile;

	float		powerarmor_time;

	char		*map;			// target_changelevel

	int			viewheight;		// height above origin where eyesight is determined
	int			takedamage;
	int			dmg;
	int			radius_dmg;
	float		dmg_radius;
	int			sounds;			//make this a spawntemp var?
	int			count;

	edict_t		*chain;
	edict_t		*enemy;
	edict_t		*oldenemy;
	edict_t		*activator;
	edict_t		*groundentity;
	int			groundentity_linkcount;
	edict_t		*teamchain;
	edict_t		*teammaster;

	edict_t		*mynoise;		// can go in client only
	edict_t		*mynoise2;

	int			noise_index;
	int			noise_index2;
	float		volume;
	float		attenuation;

	// timing variables
	float		wait;
	float		delay;			// before firing targets
	float		random;

	float		teleport_time;

	int			watertype;
	int			waterlevel;

	vec3_t		move_origin;
	vec3_t		move_angles;

	// move this to clientinfo?
	int			light_level;

	int			style;			// also used as areaportal number

	gitem_t		*item;			// for bonus items

								// common data blocks
	moveinfo_t		moveinfo;
	cast_info_t		cast_info;

	// BEGIN:	Xatrix/Ridah/Navigator/17-mar-1998
	nav_data_t			nav_data;			// CNS-specific data, present for all entities
	nav_build_data_t	*nav_build_data;	// used only by entities that create nodes (clients)
	void	*active_node_data;	// points to the active node data of the current unit
											// END:		Xatrix/Ridah/Navigator/17-mar-1998

											// JOSEPH 19-MAR-99
	vector  rotate;
	vector  nodeorigin;
	vector  angletarget;
	vector  save_avel;
	vector  savecameraorigin;
	vector  cameraangle;
	vector  cameraorigin;
	vector  cameravel;
	vector  cameravelrel;
	float  duration;	// Ridah, used this for Bike thruster
	float  lastduration;
	float  alphalevel;
	int   avelflag;
	int   durationflag;  // Ridah, used this for Thug reload checking
	int   firstnode;
	int   handleflag;
	int   handle2flag;
	int   doorflag;
	int   firetype;
	int   fallerflag;
	int	  fallingflag;
	int   footsteptype;
	int   fxdensity;
	int   currentcash;
	int   pullable;
	int		deadticks;
	int		healspeed;
	int		healtimer;
	int		nokickbackflag;
	int		thudsurf;
	int		thudsnd;
	int		misstime;
	int		missteam;
	int     debugprint;
	int	    onarampage;
	float   timemissing;
	char     *target2;
	edict_t	 *target2_ent;
	edict_t  *missent;
	edict_t  *handle;
	edict_t  *handle2;
	edict_t  *save_self;
	edict_t  *save_other;
	char     *type;
	char	*localteam;
	float	reactdelay;
	char	head;
	int		key; // Door key [-1 = trigger unlocked][0 = unlocked][1+ = key to unlock]
	int     lightit;
	int		option;
	int		noshadow;
	// JOSEPH 17-MAY-99
	int		monsterprop;
	solid_t	 savesolid;
	int     surfacetype;
	int     pullingflag;
	// END JOSEPH	

	// Flamethrower stuff
	char	onfiretime;
	edict_t	*onfireent;		// Ridah, added this so we can record the entity that set us on fire

							// Ridah, new AI system
	edict_t		*leader;
	char		*leader_target;				// for setting up AI's to follow other AI's from in the .map
	edict_t		*last_goal;					// set each time we AI_Run(), was the last goal entity (eg. leader or enemy)

	int			order;						// last order issued
	float		order_timestamp;			// time of last order, so only followers that can see us will respond to the order
											// updated when the player issues an order, and when the AI character follows it

	edict_t		*moveout_ent;				// set to the target position when we have issued a "moveout" command

	int			cast_group;					// index of group in which this cast member belongs (set by .map)
	int			character_index;			// our position in the global character array

	float		last_talk_time;
	int			profanity_level;			// each time the profanity button is pressed, this is incremented
	voice_table_t	*last_voice;			// voice_table_t structure of last speech

	int			skin;						// set self->s.skinnum to this upon spawning

	int			moral;						// one of MORAL_* values, set in editor

	char		*guard_target;				// point this to the targetname of the guard_ent this character should guard
	int			guard_radius;				// deeemed to be guarding if inside this radius
	edict_t		*guard_ent;					// guard this if set

	char		*sight_target;				// will head for this entity upon sighting an enemy

	edict_t		*goal_ent;					// if this is set, we should ignore everything apart from an enemy, and head for it
	edict_t		*combat_goalent;			// we should go here instead of going straight to our enemy
	edict_t		*cover_ent;					// entity we are taking cover from
	char		*next_combattarget;
	float		last_getcombatpos;
	float		last_gethidepos;
	float		dont_takecover_time;

	// Ridah, this gets copied to the client.pers->episode_flags for client when they change maps
	int			episode_flags;				// up to 32 bit flags, that represent various things according to the episode being played

	char		*name;						// Special character name eg. "Bernie"
	int			name_index;					// Each name has a name_index (unique for each episode), for fast lookups
	char		*scriptname;				// used to identify hard-coded scripting events

	edict_t		*last_territory_touched;	// set when we touch an "ai_territory" brush, so gangs react when we're in there territory
	float		time_territory_touched;
	int			current_territory;

	edict_t		*response_ent;
	float		last_response_time;
	response_t	last_response;

	edict_t		*start_ent;					// entity marking our starting position

	vec3_t		last_step_pos;				// for AI footsteps
	float		last_step_time;
	float		last_stand_evade;

	int			noroute_count;
	int			last_rval;

	float		fall_height;				// max Z value of last jump
	float		last_offground, last_onground;

	// Ridah, 6-jun-99, so they know where to return to when done hiding
	edict_t		*holdpos_ent;

	// RAFAEL
	int			activate_flags;
	float		biketime;
	int			bikestate;

	// Ridah, Vehicles
	int			vehicle_index;				// references a vehicle_t structure in the global array

	int			acc;				// accuracy variable - troop quality variable	
	int			cal;				// damage caused by weapon - calibur

	char		*art_skins;			// holds 3-digit skins for head, torso and legs

	float		gun_noise_delay;

	float		noise_time;
	vec3_t		noise_pos;
	int			noise_type;
	vec3_t		noise_angles;

	int			gender;				// so we know what sorts of sounds to play, uses GENDER_*

									// combat AI stuff
	float		combat_last_visible;
	vec3_t		combat_last_visible_pos;
	int			combat_flags;

	float		take_cover_time;

	// so we can trigger an event when a character gets below a certain health value
	int			health_threshold;
	char		*health_target;
	int			health_threshold2;
	char		*health_target2;
	int			health_threshold3;
	char		*health_target3;

	float		stand_if_idle_time;		// stand if crouching and not doing much

};


//===============================================================

//
// functions provided by the main engine
//
typedef struct
{
	// special messages
	void(*bprintf) (int printlevel, char *fmt, ...);
	void(*dprintf) (char *fmt, ...);
	void(*cprintf) (edict_t *ent, int printlevel, char *fmt, ...);
	void(*centerprintf) (edict_t *ent, char *fmt, ...);
	void(*sound) (edict_t *ent, int channel, int soundindex, float volume, float attenuation, float timeofs);
	void(*positioned_sound) (vec3_t origin, edict_t *ent, int channel, int soundinedex, float volume, float attenuation, float timeofs);

	// config strings hold all the index strings, the lightstyles,
	// and misc data like the sky definition and cdtrack.
	// All of the current configstrings are sent to clients when
	// they connect, and changes are sent to all connected clients.
	void(*configstring) (int num, char *string);

	void(*error) (char *fmt, ...);

	// the *index functions create configstrings and some internal server state
	int(*modelindex) (char *name);
	int(*soundindex) (char *name);
	int(*imageindex) (char *name);

	int(*skinindex) (int modelindex, char *name);

	void(*setmodel) (edict_t *ent, char *name);

	// collision detection
	trace_t(*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask);
	int(*pointcontents) (vec3_t point);
	bool(*inPVS) (vec3_t p1, vec3_t p2);
	bool(*inPHS) (vec3_t p1, vec3_t p2);
	void(*SetAreaPortalState) (int portalnum, bool open);
	bool(*AreasConnected) (int area1, int area2);

	// an entity will never be sent to a client or used for collision
	// if it is not passed to linkentity.  If the size, position, or
	// solidity changes, it must be relinked.
	void(*linkentity) (edict_t *ent);
	void(*unlinkentity) (edict_t *ent);		// call before removing an interactive edict
	int(*BoxEdicts) (vec3_t mins, vec3_t maxs, edict_t **list, int maxcount, int areatype);
	void(*Pmove) (pmove_t *pmove);		// player movement code common with client prediction

										// network messaging
	void(*multicast) (vec3_t origin, multicast_t to);
	void(*unicast) (edict_t *ent, bool reliable);
	void(*WriteChar) (int c);
	void(*WriteByte) (int c);
	void(*WriteShort) (int c);
	void(*WriteLong) (int c);
	void(*WriteFloat) (float f);
	void(*WriteString) (char *s);
	void(*WritePosition) (vec3_t pos);	// some fractional bits
	void(*WriteDir) (vec3_t pos);		// single byte encoded, very coarse
	void(*WriteAngle) (float f);

	// managed memory allocation
	void	*(*TagMalloc) (int size, int tag);
	void(*TagFree) (void *block);
	void(*FreeTags) (int tag);

	// Ridah
	void(*ClearObjectBoundsCached) (void);
	void(*StopRender) (void);

	// console variable interaction
	cvar_t	*(*cvar) (char *var_name, char *value, int flags);
	cvar_t	*(*cvar_set) (char *var_name, char *value);
	cvar_t	*(*cvar_forceset) (char *var_name, char *value);

	// ClientCommand and ServerCommand parameter access
	int(*argc) (void);
	char	*(*argv) (int n);
	char	*(*args) (void);	// concatenation of all argv >= 1

								// add commands to the server console as if they were typed in
								// for map changing, etc
	void(*AddCommandString) (char *text);

	void(*DebugGraph) (float value, int color);

	// Ridah, MDX
	void(*GetObjectBounds) (char *mdx_filename, model_part_t *model_part);
	// done.

	// Ridah, so we can save the game when traversing to a new level (so hiredguy's get restored when restarting a map)
	void(*SaveCurrentGame) (void);
} game_import_t;

//
// functions exported by the game subsystem
//
typedef struct game_export_s
{
	int			apiversion;

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	void(*Init) (void);
	void(*Shutdown) (void);

	// each new level entered will cause a call to SpawnEntities
	void(*SpawnEntities) (char *mapname, char *entstring, char *spawnpoint);

	// Read/Write Game is for storing persistant cross level information
	// about the world state and the clients.
	// WriteGame is called every time a level is exited.
	// ReadGame is called on a loadgame.
	void(*WriteGame) (char *filename, bool autosave);
	void(*ReadGame) (char *filename);

	// ReadLevel is called after the default map information has been
	// loaded with SpawnEntities
	void(*WriteLevel) (char *filename);
	void(*ReadLevel) (char *filename);

	bool(*ClientConnect) (edict_t *ent, char *userinfo);
	void(*ClientBegin) (edict_t *ent);
	void(*ClientUserinfoChanged) (edict_t *ent, char *userinfo);
	void(*ClientDisconnect) (edict_t *ent);
	void(*ClientCommand) (edict_t *ent);
	void(*ClientThink) (edict_t *ent, usercmd_t *cmd);

	void(*RunFrame) (void);

	// ServerCommand will be called when an "sv <command>" command is issued on the
	// server console.
	// The game can issue gi.argc() / gi.argv() commands to get the rest
	// of the parameters
	void(*ServerCommand) (void);

	int			*(*GetNumObjectBounds) (void);
	void		*(*GetObjectBoundsPointer) (void);

	int(*GetNumJuniors) (void);

	//
	// global variables shared between game and server
	//

	// The edict array is allocated in the game dll so it
	// can vary in size from one game to another.
	// 
	// The size will be fixed when ge->Init() is called
	struct edict_s	*edicts;
	int			edict_size;
	int			num_edicts;		// current number, <= max_edicts
	int			max_edicts;

	edict_s* GetEntity(int entity_id)
	{
		uintptr_t entity = uintptr_t(edicts);
		entity += entity_id * edict_size;
		return (edict_s*)entity;
	}

} game_export_t;

extern game_import_t *	gi;
extern game_export_t *	globals;

#define POWERSUIT_SCALE		4.0F

#define SHELL_RED_COLOR		0xF2
#define SHELL_GREEN_COLOR	0xD0
#define SHELL_BLUE_COLOR	0xF3

#define SHELL_RG_COLOR		0xDC
//#define SHELL_RB_COLOR		0x86
#define SHELL_RB_COLOR		0x68
#define SHELL_BG_COLOR		0x78

//ROGUE
#define SHELL_DOUBLE_COLOR	0xDF // 223
#define	SHELL_HALF_DAM_COLOR	0x90
#define SHELL_CYAN_COLOR	0x72
//ROGUE

#define SHELL_WHITE_COLOR	0xD7

typedef struct
{
	int			down[2];		// key nums holding it down
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame
	int			state;
} kbutton_t;

typedef struct entity_s
{
	struct model_s		*model;			// opaque type outside refresh
	float				angles[3];

	/*
	** most recent data
	*/
	float				origin[3];		// also used as RF_BEAM's "from"
	int					frame;			// also used as RF_BEAM's diameter

										/*
										** previous data for lerping
										*/
	float				oldorigin[3];	// also used as RF_BEAM's "to"
	int					oldframe;

	/*
	** misc
	*/
	float	backlerp;				// 0.0 = current, 1.0 = old
	int		skinnum;				// also used as RF_BEAM's palette index

	int		lightstyle;				// for flashing entities
	float	alpha;					// ignore if RF_TRANSLUCENT isn't set

	struct image_s *skin;			// NULL for inline skin
	int		flags;

	char	pad_0[164];

} entity_t;

#define ENTITY_FLAGS  68

typedef struct
{
	vec3_t	origin;
	vec3_t	color;
	float	intensity;
} dlight_t;

typedef struct
{
	vec3_t	origin;
	int		color;
	float	alpha;
} particle_t;

typedef struct
{
	float		rgb[3];			// 0.0 - 2.0
	float		white;			// highest of rgb
} lightstyle_t;

typedef struct
{
	int			x, y, width, height;// in virtual screen coordinates
	float		fov_x, fov_y;
	float		vieworg[3];
	float		viewangles[3];
	float		blend[4];			// rgba 0-1 full screen blend
	float		time;				// time is uesed to auto animate
	int			rdflags;			// RDF_UNDERWATER, etc

	byte		*areabits;			// if not NULL, only areas with set bits will be drawn

	lightstyle_t	*lightstyles;	// [MAX_LIGHTSTYLES]

	int			num_entities;
	entity_t	*entities;

	int			num_dlights;
	dlight_t	*dlights;

	int			num_particles;
	particle_t	*particles;

	int num_unk1;
	void* unk1s;

	int num_unk2;
	void* unk2s;

	int num_unk3;
	void* unk3s;

	vec3_t v_forward;
	vec3_t v_right;
	vec3_t v_up;
} refdef_t;



#define	API_VERSION		3

//
// these are the functions exported by the refresh module
//
typedef struct
{
	// if api_version is different, the dll cannot be used
	int		api_version;

	int(*Init) (void *hinstance, void *wndproc);
	void(*Shutdown) (void);
	void(*BeginRegistration) (char *map);
	struct model_s *(*RegisterModel) (char *name);
	struct image_s *(*RegisterSkin) (char *name);
	struct image_s *(*RegisterPic) (char *name);
	void(*SetSky) (char *name, float rotate, vec3_t axis);
	void(*EndRegistration) (void);

	void(*pad1);
	void(*pad2);

	void(*RenderFrame) (refdef_t *fd);

	void(*DrawGetPicSize) (int *w, int *h, char *name);	// will return 0 0 if not found
	void(*DrawPic) (int x, int y, char *name);
	void(*DrawStretchPic) (int x, int y, int w, int h, char *name);

	void(*pad3);
	void(*pad4);
	void(*pad5);
	void(*pad6);

	void(*DrawChar) (int x, int y, int c);

	void(*DrawTileClear) (int x, int y, int w, int h, char *name);
	void(*pad7);
	void(*pad8);

	void(*DrawFill) (int x, int y, int w, int h, int c);
	void(*DrawFadeScreen) (void);

	// Draw images for cinematic rendering (which can have a different palette). Note that calls
	void(*DrawStretchRaw) (int x, int y, int w, int h, int cols, int rows, byte *data);

	/*
	** video mode and refresh state management entry points
	*/
	void(*CinematicSetPalette)(const unsigned char *palette);	// NULL = game palette
	void(*BeginFrame)(float camera_separation);
	void(*EndFrame) (void);

	void(*AppActivate)(bool activate);

} refexport_t;

//
// these are the functions imported by the refresh module
//
typedef struct
{
	void(*Sys_Error) (int err_level, char *str, ...);

	void(*Cmd_AddCommand) (char *name, void(*cmd)(void));
	void(*Cmd_RemoveCommand) (char *name);
	int(*Cmd_Argc) (void);
	char	*(*Cmd_Argv) (int i);
	void(*Cmd_ExecuteText) (int exec_when, char *text);

	void(*Con_Printf) (int print_level, char *str, ...);

	// files will be memory mapped read only
	// the returned buffer may be part of a larger pak file,
	// or a discrete file from anywhere in the quake search path
	// a -1 return means the file does not exist
	// NULL can be passed for buf to just determine existance
	int(*FS_LoadFile) (char *name, void **buf);
	void(*FS_FreeFile) (void *buf);

	// gamedir will be the current directory that generated
	// files should be stored to, ie: "f:\quake\id1"
	char	*(*FS_Gamedir) (void);

	cvar_t	*(*Cvar_Get) (char *name, char *value, int flags);
	cvar_t	*(*Cvar_Set)(char *name, char *value);
	void(*Cvar_SetValue)(char *name, float value);

	bool(*Vid_GetModeInfo)(int *width, int *height, int mode);
	void(*Vid_MenuInit)(void);
	void(*Vid_NewWindow)(int width, int height);
} refimport_t;


// this is the only function actually exported at the linker level
typedef	refexport_t(*GetRefAPI_t) (refimport_t);

extern refimport_t *	imports;
extern refexport_t *	exports;

#define	MAX_MAP_AREAS		512 //kingpin doubles the max count

//this struct has changed a lot, compared to quake 2
typedef struct
{
	//yes, BOOL, not bool
	//qboolean enum is 4 bytes long
	BOOL			valid;			// cleared if delta parsing was invalid
	int				serverframe;
	int				servertime;		// server time the message is valid for (in msec)
	int				deltaframe;
	byte			areabits[MAX_MAP_AREAS / 8];		// portalarea visibility bits
	char pad_0014[56]; //filled with zeros
	player_state_t	playerstate; //changed a lot!
	int				num_entities;
	int				parse_entities;	// non-masked index into cl_parse_entities array
} frame_t;


typedef struct
{
	int			framenum;
	float		time;

	char		level_name[MAX_QPATH];	// the descriptive name (Outer Base, etc)
	char		mapname[MAX_QPATH];		// the server name (base1, etc)
	char		nextmap[MAX_QPATH];		// go here when fraglimit is hit

										// intermission state
	float		intermissiontime;		// time the intermission was started
	char		*changemap;
	int			exitintermission;
	vec3_t		intermission_origin;
	vec3_t		intermission_angle;

	edict_t		*sight_client;	// changed once each frame for coop games

	edict_t		*sight_entity;
	int			sight_entity_framenum;
	edict_t		*sound_entity;
	int			sound_entity_framenum;
	edict_t		*sound2_entity;
	int			sound2_entity_framenum;

	int			pic_health;

	int			total_secrets;
	int			found_secrets;

	int			total_goals;
	int			found_goals;

	int			total_monsters;
	int			killed_monsters;

	edict_t		*current_entity;	// entity running from G_RunFrame
	int			body_que;			// dead bodies

	int			power_cubes;		// ugly necessity for coop

									// BEGIN:	Xatrix/Ridah/Navigator/19-mar-1998
									// this stores all the node data for the current level
	void	*node_data;
	// END:		Xatrix/Ridah/Navigator/19-mar-1998

	// Ridah
	int			num_characters;		// progressive total, updated each time a character enters or leaves the game
									// NOTE: includes players!

	edict_t*		characters[MAX_CHARACTERS];	// indexes in g_edicts to all characters in the game

												// Global Cast Memory - stores a lookup table pointing to character-character memories
												//
												//	This allows us to easily indentify whether a particular character can see another character
												//
												//                                 [    SOURCE    ][     DEST     ]
	cast_memory_t	*global_cast_memory[MAX_CHARACTERS][MAX_CHARACTERS];

	int			episode;			// current episode being played
	int			unit;				// current unit within the episode
									// Ridah, done.

									// RAFAEL 11-05-98
	vec3_t	light_orgs[MAX_LIGHT_SOURCES];
	vec3_t	light_colors[MAX_LIGHT_SOURCES];
	float	light_values[MAX_LIGHT_SOURCES];
	//float	light_radius[MAX_LIGHT_SOURCES];
	byte	light_styles[MAX_LIGHT_SOURCES];
	int		num_light_sources;
	// END 11-05-98

	// RAFAEL
	float	cut_scene_time;
	// int		exit_cut_scene;
	vec3_t	cut_scene_origin;
	vec3_t	cut_scene_angle;
	vec3_t	player_oldpos;
	vec3_t	player_oldang;

	float		pawn_time;
	vec3_t		pawn_origin;
	BOOL	pawn_exit;

	BOOL	bar_lvl;
	float		speaktime;
	// JOSEPH 24-FEB-99
	int         cut_scene_camera_switch;
	int			cut_scene_end_count;
	// END JOSEPH
	// JOSEPH 19-MAR-99-B
	float       fadeendtime;
	float       totalfade;
	int			inversefade;
	// END JOSEPH

	// JOSEPH 13-JUN-99
	int helpchange;
	// END JOSEPH
} level_locals_t;

