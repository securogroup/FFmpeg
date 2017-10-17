/*
 * Copyright (c) 2017 SecuroGroup Ltd
 * Copied largely from Plex's modifications to FFMPEG.
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "securo.h"
#include "ffmpeg.h"

#include <sys/types.h>
#include <limits.h>
#include "strings.h"
#include "libavcodec/mpegvideo.h"
#include "libavformat/http.h"
#include "libavutil/timestamp.h"
#include "libavformat/internal.h"
#include "libavutil/thread.h"

SecuroContext securoContext = {0};


char* SEC_IssueHttpRequest(const char* url, const char* verb)
{
    char* reply = NULL;
    AVIOContext *ioctx = NULL;
    AVDictionary *settings = NULL;
    int size = 0;
    int ret;

    av_dict_set(&settings, "method", verb, 0);
    av_dict_set(&settings, "timeout", "1000000", 0);

    ret = avio_open2(&ioctx, url, AVIO_FLAG_READ,
                     NULL,
                     &settings);

    if (ret < 0)
        goto fail;

    size = avio_size(ioctx);
    if (size < 0)
        size = 4096;
    else if (!size)
        goto fail;

    reply = av_malloc(size);

    ret = avio_read(ioctx, reply, size);

    if (ret < 0)
        *reply = 0;

    avio_close(ioctx);
    av_dict_free(&settings);
    return reply;

fail:
    avio_close(ioctx);
    av_dict_free(&settings);
    reply = av_malloc(1);
    *reply = 0;
    return reply;
}


int securo_opt_progress_url(void *optctx, const char *opt, const char *arg)
{
    securoContext.progress_url = (char*)arg;
    return 0;
}
