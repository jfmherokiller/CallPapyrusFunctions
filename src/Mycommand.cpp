
static JSContext* JS_NewCustomContext(JSRuntime* rt)
{
	JSContext* ctx = JS_NewContext(rt);
	if (!ctx)
		return nullptr;
	JS_AddIntrinsicBigFloat(ctx);
	JS_AddIntrinsicBigDecimal(ctx);
	JS_AddIntrinsicOperators(ctx);
	JS_EnableBignumExt(ctx, 1);
	/* system modules */
	js_init_module_std(ctx, "std");
	js_init_module_os(ctx, "os");
	js_init_module_storage(ctx, "storage");
	return ctx;
}

void EvalateJsExpression(RE::StaticFunctionTag*,RE::BSFixedString expression)
{
	JSRuntime* rt;
	JSContext* ctx;
	rt = JS_NewRuntime();
	if (!rt) {
		PrintStringToConsoleFile("qjs: cannot allocate JS runtime\n");
		exit(2);
	}
	js_std_set_worker_new_context_func(JS_NewCustomContext);
	js_std_init_handlers(rt);
	ctx = JS_NewCustomContext(rt);
	if (!ctx) {
		PrintStringToConsoleFile("qjs: cannot allocate JS context\n");
		exit(2);
	}

	/* loader for ES6 modules */
	JS_SetModuleLoaderFunc(rt, nullptr, js_module_loader, nullptr);


	/* make 'std' and 'os' visible to non module code */
	const std::string str =
		"import * as std from 'std';\n"
		"import * as os from 'os';\n"
		"globalThis.std = std;\n"
		"globalThis.os = os;\n";
	eval_buf(ctx, str.c_str(), strlen(str.c_str()), "<input>", JS_EVAL_TYPE_MODULE);
	
	eval_buf(ctx, expression.c_str(), strlen(expression.c_str()), "<cmdline>", JS_EVAL_TYPE_GLOBAL);
	js_std_loop_buffer(ctx,PrintStringToConsoleFileFree);
	js_std_free_handlers(rt);
	JS_FreeContext(ctx);
	JS_FreeRuntime(rt);
}
