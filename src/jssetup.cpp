/*
 * QuickJS stand alone interpreter
 *
 * Copyright (c) 2017-2020 Fabrice Bellard
 * Copyright (c) 2017-2020 Charlie Gordon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#include <unistd.h>
#elif defined(__linux__)
#include <malloc.h>
#include <unistd.h>
#endif

#include "cutils.h"
#include "quickjs-libc.h"
#ifdef CONFIG_STORAGE
#include "storage/quickjs-storage.h"
#endif
#include "quickjs-version.h"
#include "quickjs.h"




extern int eval_buf(JSContext* ctx, const char* buf, size_t buf_len,
    const char* filename, int eval_flags)
{
    JSValue val;
    int ret;
	char* output;
    if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE) {
        /* for the modules, we compile then run to be able to set
           import.meta */
        val = JS_Eval(ctx, buf, buf_len, filename,
            eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
        if (!JS_IsException(val)) {
            js_module_set_import_meta(ctx, val, TRUE, TRUE);
            val = JS_EvalFunction(ctx, val);
        }
    }
    else {
        val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
    }
    if (JS_IsException(val)) {
        output = js_std_dump_error_buffer(ctx);
		PrintStringToConsoleFile(output);
		free(output);
        ret = -1;
    }
    else {
        PrintToConsole(ctx, val);
        ret = 0;
    }
    JS_FreeValue(ctx, val);
    return ret;
}