/* misc. universal things
 * Header: "defs.h"
 *
 * Copyright (C) 1998-2001  D. Hugh Redelmeier.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include "lswtime.h"
#include <sys/types.h>

#include <libreswan.h>

#include "sysdep.h"
#include "constants.h"
#include "defs.h"
#include "log.h"
#include "whack.h"      /* for RC_LOG_SERIOUS */


bool all_zero(const unsigned char *m, size_t len)
{
	size_t i;

	for (i = 0; i != len; i++)
		if (m[i] != '\0')
			return FALSE;

	return TRUE;
}

/*  checks if the expiration date has been reached and
 *  warns during the warning_interval of the imminent
 *  expiry. strict=TRUE declares a fatal error,
 *  strict=FALSE issues a warning upon expiry.
 *
 * Note: not re-entrant because the message may be in a static buffer (buf).
 */
const char *check_expiry(time_t expiration_date, int warning_interval,
			bool strict)
{
	time_t tnow = now();
	int time_left;

	if (expiration_date == UNDEFINED_TIME)
		return "ok (expires never)";

	time_left = (expiration_date - tnow);
	if (time_left < 0)
		return strict ? "fatal (expired)" : "warning (expired)";

	if (time_left > time_t_day * warning_interval)
		return "ok";

	{
		static char buf[35]; /* temporary storage */
		const char *unit = "second";

		if (time_left > 2 * time_t_day) {
			time_left /= time_t_day;
			unit = "day";
		} else if (time_left > 2 * time_t_hour) {
			time_left /= time_t_hour;
			unit = "hour";
		} else if (time_left > 2 * time_t_minute) {
			time_left /= time_t_minute;
			unit = "minute";
		}
		snprintf(buf, sizeof(buf), "warning (expires in %d %s%s)",
			time_left, unit, (time_left == 1) ? "" : "s");
		return buf;
	}
}
