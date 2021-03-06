/* lIBvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Andrew Birck <birck@uiuc.edu>
 *
 * $Id: madspin.c,v 1.32 2006/02/25 18:45:15 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>
#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <gettext.h>

#include <time.h>

#include <GL/gl.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

typedef struct {
	int			 initialized;

	int			 draw_mode;

	GLuint			 textures[2];
	VisVideo		*texture_images[2];

	int			 maxlines;
	float			 texsize;
	float			 xrot;
	float			 yrot;
	float			 zrot;
	float			 total;
	float			 frame;

	float			 gdata[256];

	VisTimer		*timer;

	/* Config */
	int			 num_stars;
	int			 speed;

	VisRandomContext	*rcontext;
} MadspinPrivate;

static int lv_madspin_init (VisPluginData *plugin);
static int lv_madspin_cleanup (VisPluginData *plugin);
static int lv_madspin_requisition (VisPluginData *plugin, int *width, int *height);
static int lv_madspin_resize (VisPluginData *plugin, int width, int height);
static int lv_madspin_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *lv_madspin_palette (VisPluginData *plugin);
static int lv_madspin_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static int  madspin_load_textures (MadspinPrivate *priv);
static int  madspin_sound (MadspinPrivate *priv, VisAudio *audio);
static int  madspin_draw (MadspinPrivate *priv, VisVideo *video);

/* Main plugin stuff */
const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = lv_madspin_requisition,
		.palette = lv_madspin_palette,
		.render = lv_madspin_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_GL
	};

	static VisPluginInfo info = {
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "madspin",
		.name = "libvisual madspin port",
		.author = N_("Original by: Andrew Birck <birck@uiuc.edu>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version = "0.1",
		.about = N_("Libvisual madspin plugin"),
		.help = N_("This plugin shows a nifty visual effect using openGL"),
		.license = VISUAL_PLUGIN_LICENSE_GPL,

		.init = lv_madspin_init,
		.cleanup = lv_madspin_cleanup,
		.events = lv_madspin_events,

		.plugin = VISUAL_OBJECT (&actor)
	};

	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RED_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_GREEN_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_BLUE_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DEPTH_SIZE, 16);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER, 1);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RGBA, 1);

	return &info;
}

static int lv_madspin_init (VisPluginData *plugin)
{
	MadspinPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("num stars",	512),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("speed",	715),
		VISUAL_PARAM_LIST_END
	};

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (MadspinPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->rcontext = visual_plugin_get_random_context (plugin);

	priv->maxlines = 1;
	priv->texsize = 0.25f;
	priv->xrot = 0.0f;
	priv->yrot = 0.0f;
	priv->zrot = 0.0f;
	priv->total = 0;
	priv->frame = 0;

	priv->timer = visual_timer_new ();

	visual_param_container_add_many (paramcontainer, params);

	priv->initialized = TRUE;

	madspin_load_textures (priv);

	return 0;
}

static int lv_madspin_cleanup (VisPluginData *plugin)
{
	MadspinPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	if (priv->initialized == TRUE) {
		visual_timer_free (priv->timer);

		visual_object_unref (VISUAL_OBJECT (priv->texture_images[0]));
		visual_object_unref (VISUAL_OBJECT (priv->texture_images[1]));

		glDeleteTextures (2, priv->textures);
	}

	visual_mem_free (priv);

	return 0;
}

static int lv_madspin_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	if (reqw < 1)
		reqw = 1;

	if (reqh < 1)
		reqh = 1;

	*width = reqw;
	*height = reqh;

	return 0;
}


static void bind_texture (GLuint texture, VisVideo *image)
{
	glBindTexture (GL_TEXTURE_2D, texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, 3, image->width, image->height, 0,
		      GL_RGB, GL_UNSIGNED_BYTE, visual_video_get_pixels (image));
}

static void lv_madspin_setup_gl (VisPluginData *plugin)
{
	MadspinPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	/* GL and the such */
	glMatrixMode (GL_PROJECTION);

	glLoadIdentity ();
	glOrtho (-4.0, 4.0, -4.0, 4.0, -18.0, 18.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	glDisable (GL_DEPTH_TEST);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel (GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth (1.0);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE);
	glEnable (GL_BLEND);
	glEnable (GL_TEXTURE_2D);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glGenTextures (2, priv->textures);
	bind_texture (priv->textures[0], priv->texture_images[0]);
	bind_texture (priv->textures[1], priv->texture_images[1]);
}

static int lv_madspin_resize (VisPluginData *plugin, int width, int height)
{
	glViewport (0, 0, width, height);

	lv_madspin_setup_gl (plugin);

	return 0;
}

static int lv_madspin_events (VisPluginData *plugin, VisEventQueue *events)
{
	MadspinPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_madspin_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "num stars"))
					priv->num_stars = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "speed"))
					priv->speed = visual_param_entry_get_integer (param);

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

static VisPalette *lv_madspin_palette (VisPluginData *plugin)
{
	return NULL;
}

static int lv_madspin_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	MadspinPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	madspin_sound (priv, audio);
	madspin_draw (priv, video);

	return 0;
}

static int madspin_load_textures (MadspinPrivate *priv)
{
	priv->texture_images[0] = visual_bitmap_load_new_video (STAR_DIR "/star1.bmp");
	if (!priv->texture_images[0]) {
		visual_log (VISUAL_LOG_ERROR, "Failed to load first texture");
		return -1;
	}

	priv->texture_images[1] = visual_bitmap_load_new_video (STAR_DIR "/star2.bmp");
	if (!priv->texture_images[1]) {
		visual_log (VISUAL_LOG_ERROR, "Failed to load second texture");
		return -1;
	}

	return 0;
}

static int madspin_sound (MadspinPrivate *priv, VisAudio *audio)
{
	int i;
	VisBuffer* buffer;
	VisBuffer* pcmb;
	float freq[256];
	float pcm[256];

	buffer = visual_buffer_new_wrap_data (freq, sizeof (freq));
	pcmb   = visual_buffer_new_wrap_data (pcm, sizeof (pcm));

	visual_audio_get_sample_mixed_simple (audio, pcmb, 2, VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_audio_get_spectrum_for_sample (buffer, pcmb, TRUE);

	visual_buffer_free (buffer);
	visual_buffer_free (pcmb);

	/* Make our data from the freq data */
	for (i = 0; i < 256; i++) {
		priv->gdata[i] = (float) freq[i] * 2000.0;

		if (priv->gdata[i] > 1.0)
			priv->gdata[i] = 1.0;
		if (i > 80)
			priv->gdata[i] = priv->gdata[i] * (2 + (i /256));
	}

	return 0;
}

static int madspin_draw (MadspinPrivate *priv, VisVideo *video)
{
	double csab, csapb;
	double snab, snapb;
	double sa, ca, s5;
	double s1r, s1g, s1b, s1a;
	double s2r, s2g, s2b, s2a;
	float point;
	int line;
	int trail = 40;
	float b;
	float aa, a;
	double x, y, z;
	int i;
	int ampl = 200;
	float elapsed_time;

	visual_timer_start (priv->timer);

	for (i = 1; i < 50; i++)
		priv->total += priv->gdata[i];

	if (priv->total > 2.5)
		priv->total = 2.5;

	priv->total /= 2.5f;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE);
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

	for (line = priv->maxlines; line > 0; line--) {
		for (point = 0; point <= priv->num_stars; point++) {
			b = 1.5f + point / 33.33333f;
			aa = trail - line * 1.0f;
			a = (aa + priv->frame) / 33.33333f;

			csab = cos (a / b);
			csapb = cos (a + b);
			snab = sin (a / b);
			snapb = sin (a + b);

			sa = sin (a * .1f);
			ca = cos (a * .1f);
			s5 = sin (a * .05f);

			x = sa * snapb * ampl * csab + ca * (csapb * ampl * b * csab + s5 * (ampl * (csab + 2 *snapb)));
			y = sa * csapb * ampl * csab + ca * (csapb * ampl * b * snab + s5 * (ampl * (csab + 2 *csapb)));
			z = ca * csapb * ampl * csab + ca * (snapb * ampl * b * snab + s5 * (ampl * (snab + 2 *snapb)));

			x /= 255.0f;
			y /= 255.0f;
			z /= 255.0f;

			glPushMatrix ();

			glTranslatef ((float) x, (float) y, (float) z);
			glBindTexture (GL_TEXTURE_2D, priv->textures[0]);

			s1r = ((point * 1.0f) / priv->num_stars);
			s1g = (priv->num_stars - point) / (priv->num_stars * 1.0f);
			s1b = ((point * 1.0f) / priv->num_stars) * 0.5f;
			s1a = ((priv->gdata[(int) (point / priv->num_stars * 220)] + (priv->total / 200.0f)) / 4.0f);

			s2r = sin (priv->frame / 400.0f);
			s2g = cos (priv->frame / 200.0f);
			s2b = cos (priv->frame / 300.0f);
			s2a = (priv->gdata[(int) (point / priv->num_stars * 220)] / 2.0f );

			if (s1a > 0.008f) {
				glBegin (GL_TRIANGLE_STRIP);
				glColor4f ((float) s1r, (float) s1g, (float) s1b, (float) s1a);
				priv->texsize = (((priv->gdata[(int) (point / priv->num_stars * 220)]))
						/ (2048.01f - (point * 4.0f))) *
						(((point - priv->num_stars) / (-priv->num_stars)) * 18.0f) + 0.15f;

				/* Top Right */
				glTexCoord2d (1, 1);
				glVertex3f (priv->texsize, priv->texsize, (float) z);
				/* Top Left */
				glTexCoord2d (0, 1);
				glVertex3f (-priv->texsize, priv->texsize, (float) z);
				/* Bottom Right */
				glTexCoord2d (1, 0);
				glVertex3f (priv->texsize, -priv->texsize, (float) z);
				/* Bottom Left */
				glTexCoord2d (0, 0);
				glVertex3f (-priv->texsize, -priv->texsize, (float) z);

				glEnd ();
			}

			glBindTexture (GL_TEXTURE_2D, priv->textures[1]);
			glRotatef (priv->frame + point, 0.0f, 0.0f, 1.0f);

			if (s2a > 0.005f) {
				glBegin (GL_TRIANGLE_STRIP);
				glColor4f ((float) s2r, (float) s2g, (float) s2b, (float) s2a);
				priv->texsize = (((priv->gdata[(int) (point / priv->num_stars * 220)]))
						/ (2048.01f - (point * 4.0f))) *
						(((point - priv->num_stars) / (-priv->num_stars)) * 18.0f) + 0.35f;

				priv->texsize *= ((visual_random_context_int(priv->rcontext) % 100) / 100.0f) * 2.0f;

				/* Top Right */
				glTexCoord2d (1, 1);
				glVertex3f (priv->texsize, priv->texsize, (float) z);
				/* Top Left */
				glTexCoord2d (0, 1);
				glVertex3f (-priv->texsize, priv->texsize, (float) z);
				/* Bottom Right */
				glTexCoord2d (1, 0);
				glVertex3f (priv->texsize, -priv->texsize, (float) z);
				/* Bottom Left */
				glTexCoord2d (0, 0);
				glVertex3f (-priv->texsize, -priv->texsize, (float) z);

				glEnd ();
			}

			/* Move back to main position */
			glPopMatrix ();
		}
	}

	glLoadIdentity ();

	elapsed_time = (float) visual_timer_elapsed_usecs (priv->timer) / 1000000;

	if (elapsed_time < 0)
		elapsed_time = 0;

	priv->frame += elapsed_time * priv->speed;

	return 0;
}

