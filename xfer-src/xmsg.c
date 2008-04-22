/*
 * Copyright (c) 2008 Zmanda, Inc.  All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA.
 *
 * Contact information: Zmanda Inc., 465 S Mathlida Ave, Suite 300
 * Sunnyvale, CA 94086, USA, or: http://www.zmanda.com
 */

#include "amxfer.h"
#include "amanda.h"

/* TODO: use glib chunk allocator */

/* NOTE TO IMPLEMENTERS:
 *
 * When adding a new attribute, make changes in the following places:
 *  - add the attribute to the XMsg struct in xmsg.h
 *  - add the attribute to the comments for the appropriate xmsg_types
 *  - free the attribute in xmsg_free.
 */

/*
 * Methods
 */

XMsg *
xmsg_new(
    XferElement *elt,
    xmsg_type type,
    int version)
{
    XMsg *msg = g_new0(XMsg, 1);
    msg->elt = elt;
    msg->type = type;
    msg->version = version;

    /* messages hold a reference to the XferElement, to avoid dangling
     * pointers. */
    g_object_ref((GObject *)elt);

    return msg;
}

void
xmsg_free(
    XMsg *msg)
{
    /* unreference the source */
    g_object_unref((GObject *)msg->elt);

    /* and free any allocated attributes */
    if (msg->repr) g_free(msg->repr);
    if (msg->message) g_free(msg->message);

    /* then free the XMsg itself */
    g_free(msg);
}

char *
xmsg_repr(
    XMsg *msg)
{
    if (!msg) return "(nil)"; /* better safe than sorry */

    /* this just shows the "header" fields for now */
    if (!msg->repr) {
	char *typ = NULL;
	switch (msg->type) {
	    case XMSG_INFO: typ = "INFO"; break;
	    case XMSG_ERROR: typ = "ERROR"; break;
	    case XMSG_DONE: typ = "DONE"; break;
	    default: typ = "**UNKNOWN**"; break;
	}

	msg->repr = vstrallocf("<XMsg@%p type=XMSG_%s elt=%s version=%d>",
	    msg, typ, xfer_element_repr(msg->elt), msg->version);
    }

    return msg->repr;
}
