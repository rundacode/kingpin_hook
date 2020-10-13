#include "visuals.h"
#include "vars.h"
#include "q2_shared.h"
#include "engine.h"

namespace visuals
{
	bool w2s(vec3_t point, float& x, float& y)
	{
		if (!globalvars::refdef)
			return false;

		vec3_t trans;
		vec_t xc, yc;
		vec_t px, py;
		vec_t z;

		px = static_cast<float>(tan(globalvars::refdef->fov_x * M_PI / 360.0f));
		py = static_cast<float>(tan(globalvars::refdef->fov_y * M_PI / 360.0f));

		VectorSubtract(point, globalvars::refdef->vieworg, trans);

		xc = globalvars::refdef->width / 2.0f;
		yc = globalvars::refdef->height / 2.0f;

		z = DotProduct(trans, globalvars::refdef->v_forward);

		if (z <= 0.001)
			return false;
		// Notice how i invert this dotproduct (that's what solved my issue)
		x = xc - -DotProduct(trans, globalvars::refdef->v_right) * xc / (z * px);
		y = yc - DotProduct(trans, globalvars::refdef->v_up) * yc / (z * py);
		return true;
	}

	void draw_cast()
	{
		for (int i = 1; i < globalvars::cast_amount; i++)
		{
			float x, y = 0;

			edict_t* cast = globalvars::cast_list[i];
			if (!cast || !cast->inuse || cast->health <= 0)
				continue;

			vec3_t origin = { cast->s.origin.x, cast->s.origin.y, cast->s.origin.z };

			if (w2s(origin, x, y))
			{
				util::draw_string(static_cast<int>(x), static_cast<int>(y), 62, true, "%s | %d", cast->name, cast->health);
			}
		}
	}

	void draw_rats()
	{
		for (int i = 1; i < globalvars::game_api->num_edicts; i++)
		{
			float x, y = 0;

			edict_t* edict = &globalvars::game_api->edicts[i];
			if (!edict || !edict->inuse || edict->health <= 0)
				continue;

			//most likely very inefficient
			if (edict->s.model_parts[0].modelindex == globalvars::game_import->modelindex((char*)"models/actors/rat/rat.mdx"))
			{
				vec3_t origin = { edict->s.origin.x, edict->s.origin.y, edict->s.origin.z};

				if (w2s(origin, x, y))
				{
					util::draw_string(static_cast<int>(x), static_cast<int>(y), 62, true, "nigger rat");
				}
			}
		}
	}


}