// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

// the cs_ functions are not defined in any header file, I guess
// C allows that...  C++ does not however, so here they are
extern	CS_INT	cs_ctx_alloc(CS_INT,CS_CONTEXT **);
extern	CS_INT	ct_init(CS_CONTEXT *,CS_INT);
extern	CS_INT	ct_callback(CS_CONTEXT *,CS_CONNECTION *,
					CS_INT,CS_INT,CS_VOID *);
extern	CS_INT	cs_config(CS_CONTEXT *,CS_INT,CS_INT,CS_VOID *,CS_INT,CS_INT *);
extern	CS_INT	ct_con_alloc(CS_CONTEXT *,CS_CONNECTION **);
extern	CS_INT	ct_con_props(CS_CONNECTION *,CS_INT,CS_INT,CS_VOID *,CS_INT,CS_INT *);
extern	CS_INT	cs_loc_alloc(CS_CONTEXT *,CS_LOCALE **);
extern	CS_INT	cs_locale(CS_CONTEXT *,CS_INT,CS_LOCALE *,CS_INT,CS_CHAR *,CS_INT,CS_INT *);
extern	CS_INT	ct_connect(CS_CONNECTION *,CS_CHAR *,CS_INT);
extern	CS_INT	ct_close(CS_CONNECTION *,CS_INT);
extern	CS_INT	ct_con_drop(CS_CONNECTION *);
extern	CS_INT	ct_exit(CS_CONTEXT *,CS_INT);
extern	CS_INT	cs_ctx_drop(CS_CONTEXT *);
extern	CS_INT	ct_cmd_alloc(CS_CONNECTION *,CS_COMMAND **);
extern	CS_INT	ct_command(CS_COMMAND *,CS_INT,CS_CHAR *,CS_INT,CS_INT);
extern	CS_INT	ct_send(CS_COMMAND *);
extern	CS_INT	ct_results(CS_COMMAND *,CS_INT *);
extern	CS_INT	ct_res_info(CS_COMMAND *,CS_INT,CS_VOID *,CS_INT,CS_INT *);
extern	CS_INT	ct_describe(CS_COMMAND *,CS_INT,CS_DATAFMT *);
extern	CS_INT	ct_bind(CS_COMMAND *,CS_INT,CS_DATAFMT *,CS_VOID *,CS_INT *,CS_SMALLINT *);
extern	CS_INT	ct_fetch(CS_COMMAND *,CS_INT,CS_INT,CS_INT,CS_INT *);
extern	CS_INT	ct_cmd_drop(CS_COMMAND *);
extern	CS_INT	cs_convert(CS_CONTEXT *,CS_DATAFMT *,CS_VOID *,CS_DATAFMT *,CS_VOID *,CS_INT *);
extern	CS_INT	cs_loc_drop(CS_CONTEXT *,CS_LOCALE *);
extern	CS_INT	ct_cancel(CS_CONNECTION *,CS_COMMAND *,CS_INT);
