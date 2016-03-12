
#include "a_sharedglobal.h"
#include "p_local.h"
#include "g_level.h"
#include "r_sky.h"
#include "p_lnspec.h"
#include "b_bot.h"
#include "p_checkposition.h"


IMPLEMENT_CLASS(AFastProjectile)


//----------------------------------------------------------------------------
//
// AFastProjectile :: Tick
//
// Thinker for the ultra-fast projectiles used by Heretic and Hexen
//
//----------------------------------------------------------------------------

void AFastProjectile::Tick ()
{
	int i;
	fixed_t xfrac;
	fixed_t yfrac;
	fixed_t zfrac;
	int changexy;

	ClearInterpolation();
	fixed_t oldz = Z();

	if (!(flags5 & MF5_NOTIMEFREEZE))
	{
		//Added by MC: Freeze mode.
		if (bglobal.freeze || level.flags2 & LEVEL2_FROZEN)
		{
			return;
		}
	}


	// [RH] Ripping is a little different than it was in Hexen
	FCheckPosition tm(!!(flags2 & MF2_RIP));

	int shift = 3;
	int count = 8;
	if (radius > 0)
	{
		while ( ((abs(vel.x) >> shift) > radius) || ((abs(vel.y) >> shift) > radius))
		{
			// we need to take smaller steps.
			shift++;
			count<<=1;
		}
	}

	// Handle movement
	if (vel.x || vel.y || (Z() != floorz) || vel.z)
	{
		xfrac = vel.x >> shift;
		yfrac = vel.y >> shift;
		zfrac = vel.z >> shift;
		changexy = xfrac || yfrac;
		int ripcount = count >> 3;
		for (i = 0; i < count; i++)
		{
			if (changexy)
			{
				if (--ripcount <= 0)
				{
					tm.LastRipped.Clear();	// [RH] Do rip damage each step, like Hexen
				}
				
				if (!P_TryMove (this, X() + xfrac,Y() + yfrac, true, NULL, tm))
				{ // Blocked move
					if (!(flags3 & MF3_SKYEXPLODE))
					{
						if (tm.ceilingline &&
							tm.ceilingline->backsector &&
							tm.ceilingline->backsector->GetTexture(sector_t::ceiling) == skyflatnum &&
							Z() >= tm.ceilingline->backsector->ceilingplane.ZatPoint(PosRelative(tm.ceilingline)))
						{
							// Hack to prevent missiles exploding against the sky.
							// Does not handle sky floors.
							Destroy ();
							return;
						}
						// [RH] Don't explode on horizon lines.
						if (BlockingLine != NULL && BlockingLine->special == Line_Horizon)
						{
							Destroy ();
							return;
						}
					}

					P_ExplodeMissile (this, BlockingLine, BlockingMobj);
					return;
				}
			}
			AddZ(zfrac);
			UpdateWaterLevel (oldz);
			oldz = Z();
			if (Z() <= floorz)
			{ // Hit the floor

				if (floorpic == skyflatnum && !(flags3 & MF3_SKYEXPLODE))
				{
					// [RH] Just remove the missile without exploding it
					//		if this is a sky floor.
					Destroy ();
					return;
				}

				SetZ(floorz);
				P_HitFloor (this);
				P_ExplodeMissile (this, NULL, NULL);
				return;
			}
			if (Top() > ceilingz)
			{ // Hit the ceiling

				if (ceilingpic == skyflatnum &&  !(flags3 & MF3_SKYEXPLODE))
				{
					Destroy ();
					return;
				}

				SetZ(ceilingz - height);
				P_ExplodeMissile (this, NULL, NULL);
				return;
			}
			if (changexy && ripcount <= 0) 
			{
				ripcount = count >> 3;
				Effect();
			}
		}
	}
	if (!CheckNoDelay())
		return;		// freed itself
	// Advance the state
	if (tics != -1)
	{
		if (tics > 0) tics--;
		while (!tics)
		{
			if (!SetState (state->GetNextState ()))
			{ // mobj was removed
				return;
			}
		}
	}
}


void AFastProjectile::Effect()
{
	//if (pr_smoke() < 128)	// [RH] I think it looks better if it's consistent
	{
		FName name = GetClass()->MissileName;
		if (name != NAME_None)
		{
			fixed_t hitz = Z()-8*FRACUNIT;

			if (hitz < floorz)
			{
				hitz = floorz;
			}
			// Do not clip this offset to the floor.
			hitz += GetClass()->MissileHeight;
		
			PClassActor *trail = PClass::FindActor(name);
			if (trail != NULL)
			{
				AActor *act = Spawn (trail, X(), Y(), hitz, ALLOW_REPLACE);
				if (act != NULL)
				{
					act->angle = this->angle;
				}
			}
		}
	}
}

