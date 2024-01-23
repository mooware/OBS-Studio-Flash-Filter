/*****************************************************************************
Copyright (C) 2017 by Eric Rasmuson <erasmuson@gmail.com>

This file is part of OBS.

OBS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <util/platform.h>

struct screen_flash_filter_data {
    obs_source_t *context;

    gs_effect_t *effect;

    uint32_t cx;
    uint32_t cy;

    gs_texrender_t *currentRender;
    gs_texrender_t *oldRender1;
    gs_texrender_t *oldRender2;
    gs_texrender_t *oldRender3;
    gs_texrender_t *oldRender4;
    gs_texrender_t *oldRender5;

    gs_texture_t *currentTex1;
    gs_texture_t *oldTex1;
    gs_texture_t *oldTex2;
    gs_texture_t *oldTex3;
    gs_texture_t *oldTex4;
    gs_texture_t *oldTex5;

    float threshold1;
    struct vec4 colorTarget;

    bool processed_frame;
    bool firstRun;
};

static const char *screen_flash_filter_getname(void *unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("Screen Flash Filter");
}

static void screen_flash_filter_update(void *data, obs_data_t *settings)
{
    struct screen_flash_filter_data *filter = data;

    obs_source_t *target = obs_filter_get_target(filter->context);
    uint32_t cx;
    uint32_t cy;

    cx = obs_source_get_base_width(target);
    cy = obs_source_get_base_height(target);

    filter->cx = cx;
    filter->cy = cy;

    double threshold1 = obs_data_get_double(settings, "threshold1");
    filter->threshold1 = (float)threshold1;

    uint32_t CT = (uint32_t)obs_data_get_int(settings, "colorTarget");
    vec4_from_rgba(&filter->colorTarget, CT);
}

static void free_textures(struct screen_flash_filter_data *f)
{
    UNUSED_PARAMETER(f);

    obs_enter_graphics();
    obs_leave_graphics();
}

static void screen_flash_filter_destroy(void *data)
{
    struct screen_flash_filter_data *filter = data;

    if (filter->effect) {
        obs_enter_graphics();
        gs_effect_destroy(filter->effect);
        obs_leave_graphics();
    }

    free_textures(filter);

    bfree(data);
}

static void *screen_flash_filter_create(obs_data_t *settings, obs_source_t *context)
{
    struct screen_flash_filter_data *filter = bzalloc(sizeof(struct screen_flash_filter_data));
    char *effect_path = obs_module_file("screen_flash_filter.effect");

    filter->firstRun = true;
    filter->processed_frame = false;

    filter->context = context;

    obs_enter_graphics();

    filter->effect = gs_effect_create_from_file(effect_path, NULL);
    if (filter->effect) {
        filter->currentRender = gs_texrender_create(GS_RGBA, GS_ZS_NONE);

        filter->currentTex1 = gs_texrender_get_texture(gs_texrender_create(GS_RGBA, GS_ZS_NONE));

        filter->oldTex1 = gs_texrender_get_texture(gs_texrender_create(GS_RGBA, GS_ZS_NONE));
        filter->oldTex2 = gs_texrender_get_texture(gs_texrender_create(GS_RGBA, GS_ZS_NONE));
        filter->oldTex3 = gs_texrender_get_texture(gs_texrender_create(GS_RGBA, GS_ZS_NONE));
        filter->oldTex4 = gs_texrender_get_texture(gs_texrender_create(GS_RGBA, GS_ZS_NONE));
        filter->oldTex5 = gs_texrender_get_texture(gs_texrender_create(GS_RGBA, GS_ZS_NONE));
    }

    obs_leave_graphics();

    bfree(effect_path);

    if (!filter->effect) {
        screen_flash_filter_destroy(filter);
        return NULL;
    }

    screen_flash_filter_update(filter, settings);
    return filter;
}

static void screen_flash_filter_tick(void *data, float t)
{
    UNUSED_PARAMETER(data);
    UNUSED_PARAMETER(t);
}

static void screen_flash_filter_render(void *data, gs_effect_t *effect)
{
    struct screen_flash_filter_data *filter = data;

    obs_source_t *target = obs_filter_get_target(filter->context);
    obs_source_t *parent = obs_filter_get_parent(filter->context);

    if (filter->currentRender != NULL)
        filter->currentTex1 = gs_texrender_get_texture(filter->currentRender);
    if (filter->oldRender1 != NULL)
        filter->oldTex1 = gs_texrender_get_texture(filter->oldRender1);
    if (filter->oldRender2 != NULL)
        filter->oldTex2 = gs_texrender_get_texture(filter->oldRender2);
    if (filter->oldRender3 != NULL)
        filter->oldTex3 = gs_texrender_get_texture(filter->oldRender3);
    if (filter->oldRender4 != NULL)
        filter->oldTex4 = gs_texrender_get_texture(filter->oldRender4);
    if (filter->oldRender5 != NULL)
        filter->oldTex5 = gs_texrender_get_texture(filter->oldRender5);

    gs_eparam_t *param;

    if (!parent || !target) {
        obs_source_skip_video_filter(filter->context);
        return;
    }

    //effect filter
    if (!obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING))
        return;
    obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING);

    param = gs_effect_get_param_by_name(filter->effect, "currentTex1");
    gs_effect_set_texture(param, filter->currentTex1);

    param = gs_effect_get_param_by_name(filter->effect, "oldTex1");
    gs_effect_set_texture(param, filter->oldTex1);
    param = gs_effect_get_param_by_name(filter->effect, "oldTex2");
    gs_effect_set_texture(param, filter->oldTex2);
    param = gs_effect_get_param_by_name(filter->effect, "oldTex3");
    gs_effect_set_texture(param, filter->oldTex3);
    param = gs_effect_get_param_by_name(filter->effect, "oldTex4");
    gs_effect_set_texture(param, filter->oldTex4);
    param = gs_effect_get_param_by_name(filter->effect, "oldTex5");
    gs_effect_set_texture(param, filter->oldTex5);

    param = gs_effect_get_param_by_name(filter->effect, "threshold1");
    gs_effect_set_float(param, filter->threshold1);

    param = gs_effect_get_param_by_name(filter->effect, "colorTarget");
    gs_effect_set_vec4(param, &filter->colorTarget);

    obs_source_process_filter_end(filter->context, filter->effect, 0, 0);

    uint32_t cx;
    uint32_t cy;

    cx = obs_source_get_base_width(target);
    cy = obs_source_get_base_height(target);

    filter->cx = cx;
    filter->cy = cy;

    filter->currentRender = NULL;

    gs_texrender_t *texRender1 = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
    filter->currentRender = texRender1;

    gs_texrender_reset(filter->currentRender);

    gs_blend_state_push();
    gs_blend_function(GS_BLEND_ONE, GS_BLEND_ZERO);

    if (gs_texrender_begin(filter->currentRender, filter->cx, filter->cy)) {
        struct vec4 clear_color;

        vec4_zero(&clear_color);
        gs_clear(GS_CLEAR_COLOR, &clear_color, 0.0f, 0);
        gs_ortho(0.0f, (float)filter->cx, 0.0f, (float)filter->cy, -100.0f, 100.0f);

        obs_source_video_render(target);

        gs_texrender_end(filter->currentRender);
    }

    gs_blend_state_pop();

    gs_texrender_destroy(filter->oldRender5);

    filter->oldRender5 = filter->oldRender4;
    filter->oldRender4 = filter->oldRender3;
    filter->oldRender3 = filter->oldRender2;
    filter->oldRender2 = filter->oldRender1;
    filter->oldRender1 = filter->currentRender;

    filter->processed_frame = true;

    UNUSED_PARAMETER(effect);
}

static obs_properties_t *screen_flash_filter_properties(void *data)
{
    obs_properties_t *props = obs_properties_create();

    obs_properties_add_color(props, "colorTarget", "Flash color to filter");
    obs_properties_add_float_slider(props, "threshold1", "Threshold (Usually between .10 and .30)", 0.0f, 0.3f, 0.01f);

    UNUSED_PARAMETER(data);
    return props;
}

static void screen_flash_filter_defaults(obs_data_t *settings)
{
    /*
    <Title> <Platform>
        <Color>        <Hex>        <R>    <G>    <B>
    Time Crisis (PSX)
        White        0XFFF8F8F8    248 248 248
    Project Horned Owl (PSX)
        Blue        0xFF000CC7    0    12    199
    Time Crisis 2 (PS2)
        Grey        0xFF838383    131 131 131
    Point Blank (PSX)
        White/Grey    0xFFDEDEDE    222    222    222
    */

    obs_data_set_default_double(settings, "threshold1", 0.1);
    obs_data_set_default_int(settings, "colorTarget", 0xFFFFFFFF);
}

struct obs_source_info screen_flash_filter = {.id = "screen_flash_filter",
                                              .type = OBS_SOURCE_TYPE_FILTER,
                                              .output_flags = OBS_SOURCE_VIDEO,
                                              .get_name = screen_flash_filter_getname,
                                              .create = screen_flash_filter_create,
                                              .destroy = screen_flash_filter_destroy,
                                              .update = screen_flash_filter_update,
                                              .video_render = screen_flash_filter_render,
                                              .video_tick = screen_flash_filter_tick,
                                              .get_properties = screen_flash_filter_properties,
                                              .get_defaults = screen_flash_filter_defaults};
