-module(makedoc).
-import(edoc, [application/3]).
-export([start/0]).

start() ->
	application(sqlrelay, '../../../src/api/erlang', []).
